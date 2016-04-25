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
	VideoCapture cap(0);
	if (!cap.isOpened()){
		printf("NOT OPENED");
		return;
	}
	int th = 0;
	bool adp = false;
	for (;;){
		Mat frame;
		cap >> frame;
		cvtColor(frame, frame, CV_BGR2GRAY);

		
		
		if (waitKey(30) == 't')adp = !adp;
		createTrackbar("threshold", "Sheet_01", &th, 256);

		if (!adp){
			threshold(frame, frame, (double)(th), 128, THRESH_BINARY);
		}else{
			adaptiveThreshold(frame, frame, (double)(th), ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 1);
		}

		imshow("Sheet_01", frame);
		if (waitKey(30) == 27)break;
	}

}