#include "opencv2/opencv.hpp"
#include "FrameTimer.h"
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

int stream() {
	FrameTimer ft;
	cv::VideoCapture cap;
	// open the default camera, use something different from 0 otherwise;
	// Check VideoCapture documentation.
	int deviceID = 1;
	int apiID = cv::CAP_ANY;
	cap.open(deviceID, apiID);
	if (!cap.isOpened()) {
		std::cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	cv::dnn::Net model_;
	cv::dnn::DetectionModel detection_model_ = cv::dnn::DetectionModel(model_);
	model_ = cv::dnn::readNetFromDarknet("yolov4-tiny.cfg","yolov4-tiny.weights");
	detection_model_ = cv::dnn::DetectionModel(model_);
	detection_model_.setInputParams(1 / 255.0, cv::Size2i(416, 416), cv::Scalar(0, 0, 0), true, false);
	cv::Scalar color(0, 255, 0, 128);

	std::vector<std::string> names = { "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
	"fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
	"elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
	"skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
	"tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
	"sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
	"potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
	"microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
	"hair drier", "toothbrush" };

	for (;;)
	{
		ft.Mark();
		cv::Mat frame;
		cap >> frame;
		if (frame.empty()) break; // end of video stream
		std::vector<int> classids;
		std::vector<float> confidences;
		std::vector<cv::Rect> boxes;
		detection_model_.detect(frame, classids, confidences, boxes, 0.25,0.6);
		for (size_t i = 0; i < boxes.size();++i) {
			cv::rectangle(frame, boxes[i], color, 2);			
			cv::putText(frame, names[classids[i]], cv::Point(boxes[i].x + 5, boxes[i].y + 25), cv::FONT_HERSHEY_DUPLEX, 1, color, 1, 8, false);
			cv::putText(frame, std::to_string(ft.GetFR()), cv::Point(boxes[i].x + 5, boxes[i].y + 50), cv::FONT_HERSHEY_DUPLEX, 1, color, 1, 8, false);
		}
		cv::imshow("this is you, smile! :)", frame);
		if (cv::waitKey(10) == 27) break; // stop capturing by pressing ESC 
	}
	// the camera will be closed automatically upon exit
	// cap.close();
}



int main()
{
	stream();
	return 0;

	const auto src = cv::imread("desktop5x5.png", cv::IMREAD_COLOR);
	cv::Rect roiRect;
	{
		cv::Mat mask;
		cv::cvtColor(src, mask, cv::COLOR_BGR2HSV);
		cv::inRange(mask, cv::Scalar(103,155,185), cv::Scalar(108,195,230), mask);
		cv::blur(mask, mask, { 5,5 });
		cv::threshold(mask, mask, 10.0, 255.0, cv::THRESH_BINARY);

		std::vector<std::vector<cv::Point>> contours; // Vector for storing contour
		cv::findContours(mask, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		for (const auto& c : contours)
		{
			cv::Rect r = cv::boundingRect(c);
			if (r.area() > roiRect.area() && std::abs(1.0f - float(r.width) / float(r.height)) < 0.05f)
			{
				roiRect = r;
			}
		}
	}

	const auto roi = src(roiRect);
	cv::Mat mask;
	cv::cvtColor(roi, mask, cv::COLOR_BGR2GRAY);
	cv::threshold(mask, mask, 160.0, 255.0, cv::THRESH_BINARY_INV);
	//cv::erode(mask, mask, {});
	std::vector<std::vector<cv::Point>> contours; // Vector for storing contour
	std::vector<cv::Vec4i> hierarchy; // 1: next sibbling, 2. sibbling before, 3: first child, 4: parent (-1)
	cv::findContours(mask, contours,hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	std::vector<cv::Rect> squareRects;
	for (int i = 0; i != -1; i = hierarchy[i][0]) // this will give us the top line contours (not the letters)
	{
		squareRects.push_back( cv::boundingRect(contours[i]));
	}
	auto disp = roi;
	for (const auto& r : squareRects)
	{
		cv::rectangle(disp, r, { 0,0,255 }, 1);
	}
	const int n = std::sqrt(float(squareRects.size() + 1));
	cv::imshow("extracted puzzle view", disp);
	std::cout << "Number of cards: " << squareRects.size() << std::endl;
	std::cout << "Puzzle format " << n << "x" << n << std::endl;
	//cv::threshold(roi,mask,)
	cv::waitKey();
	return 0;
}