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
	namedWindow("Sheet_01");
	namedWindow("Sheet_02");
	VideoCapture cap(0);
	if (!cap.isOpened()){
		printf("NOT OPENED");
		return;
	}
	int th = 0;
	bool adp = false;
	int pixN = 0;
	int c = 51;

	for (;;){
		Mat frame_01,frame_02;

		//SHEET_01
		cap >> frame_01;
		cvtColor(frame_01, frame_01, CV_BGR2GRAY);
		if (waitKey(30) == 't')adp = !adp;
		createTrackbar("threshold", "Sheet_01", &th, 256);
		if (!adp){
			threshold(frame_01, frame_01, (double)(th), 128, THRESH_BINARY);
		}else{
			createTrackbar("pixelNeighborhood", "Sheet_01", &pixN, 100);
			createTrackbar("constant", "Sheet_01", &c, 100);

			adaptiveThreshold(frame_01, frame_01, (double)(th), ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3+2*pixN, c-50);
		}
		imshow("Sheet_01", frame_01);
		
		//SHEET_02
		cap >> frame_02;
		cvtColor(frame_02, frame_02, CV_BGR2GRAY);
		adaptiveThreshold(frame_02, frame_02, 120, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 13, -26);
		
		
		imshow("Sheet_02", frame_02);


		if (waitKey(30) == 27)break;
	}

}