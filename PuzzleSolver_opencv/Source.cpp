#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

int main()
{
	const auto src = cv::imread("desktop01.png", cv::IMREAD_COLOR);
	cv::Mat mask;

	cv::cvtColor(src, mask, cv::COLOR_BGR2HSV);
	cv::inRange(mask, cv::Scalar(103,155,185), cv::Scalar(108,195,230), mask);
	cv::blur(mask, mask, { 5,5 });
	cv::threshold(mask, mask, 10.0, 255.0, cv::THRESH_BINARY);
	cv::Rect roiRect;
	{

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
	auto dim = src;
	cv::rectangle(dim, roiRect, { 0,0,255 }, 1);
	cv::imshow("desktopview", dim);
	cv::waitKey();
	return 0;
}