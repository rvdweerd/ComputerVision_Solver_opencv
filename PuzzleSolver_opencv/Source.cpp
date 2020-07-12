#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

int main()
{
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