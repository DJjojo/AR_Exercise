#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>

using namespace cv;
using namespace std;

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
	vector<vector<Point>> contours;

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
			Mat frame_02;
			cap >> frame_02;
			cvtColor(frame_02, frame_02, CV_BGR2GRAY);
			adaptiveThreshold(frame_02, frame_02, 140, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 143, 20);

			//FindContours
			vector<vector<Point>> contours_help;			
			findContours(frame_02, contours_help, RETR_TREE, CHAIN_APPROX_SIMPLE);			
			contours.resize(contours_help.size());
			for (int i = 0; i < contours_help.size(); i++){
				approxPolyDP(Mat(contours_help[i]), contours[i], 5, true);
			}

			vector<Rect> rects;
			for (int i = 0; i < contours.size(); i++){
				if (contours.at[i].size() != 4){
					rects.push_back(boundingRect(contours.at(i)));
				}
			}

			//DrawContours
			Scalar color(255, 0, 0);
			imshow("Sheet_02", frame_02);
			break;
		}
		default:
			break;
		}
		if (waitKey(30) == 't')adp = !adp;
		if (waitKey(30) == 'r'){ exercise = ((exercise + 1)%(numEx)); destroyAllWindows(); }//= ((exercise + 1) % exercise_last) + 1;
		if (waitKey(30) == 27)break;
	}

}