#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>

using namespace cv;
using namespace std;

int subpixSampleSafe(const cv::Mat &pSrc, const Point2f &p){
	int x = int(floorf(p.x));
	int y = int(floorf(p.y));
	if (x < 0 || x >= pSrc.cols - 1 || y < 0 || y >= pSrc.rows - 1) return 127;

	int dx = int(256 * (p.x - floorf(p.x)));
	int dy = int(256 * (p.y - floorf(p.y)));

	unsigned char* i = (unsigned char*) ((pSrc.data + y*pSrc.step) + x);
	int a = i[0] + ((dx * (i[1] - i[0])) >> 8);
	i += pSrc.step;
	int b = i[0] + ((dx*(i[1] - i[0])) >> 8);
	return a + ((dy * (b - a)) >> 8);
}

void main()
{
	
	
	VideoCapture cap(0);
	if (!cap.isOpened()){
		printf("NOT OPENED");
		return;
	}
	int th = 0;
	bool adp = false;
	int pixN = 0;
	int c = 51;
	int exercise = 1;
	int numEx  = 2;

	CvMemStorage* memStorage = cvCreateMemStorage();

	for (;;){
		

		switch (exercise+1)
		{
		case 1://SHEET_01
		{
			namedWindow("Sheet_01");
			Mat frame_01;
			cap >> frame_01;
			cvtColor(frame_01, frame_01, CV_BGR2GRAY);
			createTrackbar("threshold", "Sheet_01", &th, 256);
			if (!adp){
				threshold(frame_01, frame_01, (double)(th), 128, THRESH_BINARY);
			}
			else{
				createTrackbar("pixelNeighborhood", "Sheet_01", &pixN, 100);
				createTrackbar("constant", "Sheet_01", &c, 100);

				adaptiveThreshold(frame_01, frame_01, (double)(th), ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3 + 2 * pixN, c - 50);
			}

			imshow("Sheet_01", frame_01);
			break;
		}
		case 2://SHEET_02
		{
			//Greyscale
			namedWindow("Sheet_02");
			namedWindow("Stripe");
			bool isFirstStripe = true;
			Mat frame_02;
			cap >> frame_02;
			Mat frame_02_g, frame_02_gt;
			cvtColor(frame_02, frame_02_g, CV_BGR2GRAY);
			adaptiveThreshold(frame_02_g, frame_02_gt, 140, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 143, 20);

			CvSeq* contours;
			CvMat frame_02_gt_(frame_02_gt);

			cvFindContours(&frame_02_gt_, memStorage, &contours, sizeof(CvContour), CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);

			for (; contours; contours = contours->h_next){

				CvSeq* result = cvApproxPoly(contours, sizeof(CvContour), memStorage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);

				if (result->total != 4)continue;

				Mat result_ = cvarrToMat(result);
				Rect r = boundingRect(result_);

				if (r.height<40 || r.width<40 || r.width > frame_02_gt.cols - 10 || r.height > frame_02_gt.rows - 10){
					continue;
				}

				const Point *rect = (const Point*)result_.data;
				int npts = result_.rows;

				polylines(frame_02, &rect, &npts, 1, true, CV_RGB(255, 0, 0), 2, CV_AA, 0);

				float lineParams[16];
				Mat lineParamsMat(Size(4, 4), CV_32F, lineParams);

				for (int i = 0; i < 4; ++i){
					circle(frame_02, rect[i], 3, CV_RGB(0, 255, 0), -1);

					double dx = (double)(rect[(i + 1) % 4].x - rect[i].x) / 7.0;
					double dy = (double)(rect[(i + 1) % 4].y - rect[i].y) / 7.0;

					Point2f points[6];

					int stripeLength = (int)(0.8*sqrt(dx*dx + dy*dy));
					if (stripeLength < 5)stripeLength = 5;
					stripeLength |= 1;
					cv::Size stripeSize;
					stripeSize.width = 3;
					stripeSize.height = stripeLength;

					int nStop = stripeLength >>1;
					int nStart = -nStop;

					cv::Point2f stripeVecX;
					cv::Point2f stripeVecY;
					double diffLength = sqrt(dx*dx + dy*dy);
					stripeVecX.x = dx / diffLength;
					stripeVecX.y = dy / diffLength;
					stripeVecY.x = stripeVecX.y;
					stripeVecY.y = -stripeVecX.x;


					Mat iplStripe(stripeSize, CV_8UC1);
					for (int j = 1; j < 7; ++j){
						double px = (double)rect[i].x + (double)j*dx;
						double py = (double)rect[i].y + (double)j*dy;

						Point p;
						p.x = (int)px;
						p.y = (int)py;
						//circle(frame_02, p, 2, CV_RGB(0, 0, 255), -1);

						for (int m = -1; m <= 1; ++m){
							for (int n = nStart; n <= nStop; ++n){
								Point2f subPixel;
								subPixel.x = (double)p.x + (double)m*stripeVecX.x + (double)n*stripeVecY.x;
								subPixel.y = (double)p.y + (double)m*stripeVecX.y + (double)n*stripeVecY.y;

								Point p2;
								p2.x = (int)subPixel.x;
								p2.y = (int)subPixel.y;
								if (isFirstStripe)
									circle(frame_02, p2, 1, CV_RGB(255, 0, 255), -1);
								else
									circle(frame_02, p2, 1, CV_RGB(0, 255, 255), -1);

								int pixel = subpixSampleSafe(frame_02_g, subPixel);
								int w = m + 1;
								int h = n + (stripeLength >> 1);
								
								iplStripe.at<uchar>(h, w) = (uchar)pixel;
							}
						}

						//--ApplySobelOperator--
						vector<double> sobelVals(stripeLength-2);
						for (int k = 1; k < stripeLength-1; k++){
							unsigned char* stripePtr = &(iplStripe.at<uchar>(k - 1, 0));
							double r1 = -stripePtr[0] - 2 * stripePtr[1] - stripePtr[2];
							stripePtr += 2 * iplStripe.step;
							double r3 = stripePtr[0] + 2 * stripePtr[1] + stripePtr[2];
							sobelVals[k - 1] = r1 + r3;

						}
						
						double maxVal = -1;
						int maxIndex = 0;
						for (int n = 0; n < stripeLength - 2; ++n){
							if (sobelVals[n] > maxVal){
								maxVal = sobelVals[n];
								maxIndex = n;
							}
						}

						double y0, y1, y2;
						y0 = (maxIndex <= 0) ? 0 : sobelVals[maxIndex - 1];
						y1 = sobelVals[maxIndex];
						y2 = (maxIndex >= stripeLength - 3) ? 0 : sobelVals[maxIndex + 1];

						double pos = (y2 - y0) / (4 * y1 - 2 * y0 - 2 * y2);

						if (pos != pos)continue;

						Point2f edgeCenter;
						int maxIndexShift = maxIndex - (stripeLength >> 1);

						edgeCenter.x = (double)p.x + (((double)maxIndexShift + pos)*stripeVecY.x);
						edgeCenter.y = (double)p.y + (((double)maxIndexShift + pos)*stripeVecY.y);
						
						circle(frame_02, edgeCenter, 1, CV_RGB(255, 0, 0));

						if (isFirstStripe){
							Mat iplTmp;
							resize(iplStripe, iplTmp, Size(100, 300));
							imshow("Stripe", iplTmp);
							isFirstStripe = false;
						}

					}

				}


			}

			cvClearMemStorage(memStorage);
			imshow("Sheet_02", frame_02);
			isFirstStripe = true;
			break;
		}
		default:
			break;
		}
		if (waitKey(30) == 't')adp = !adp;
		if (waitKey(30) == 'r'){ exercise = ((exercise + 1)%(numEx)); destroyAllWindows(); }//= ((exercise + 1) % exercise_last) + 1;
		if (waitKey(30) == 27)break;
	}
	cvReleaseMemStorage(&memStorage);
}