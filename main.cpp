#include "Enums.hpp"

#include <iostream>
#include <cstdlib>

#include <unistd.h>

#include <opencv2/opencv.hpp>

#define DEBUG(x) do{ printf("DEBUG - %s\n", x); }while(false)

#define SOURCE		1
#define HSV			1
#define MASK		1
#define CONTOUR		1
#define RESULT		1

void process(std::string ipath, std::string opath)
{
	cv::Mat src = cv::imread(ipath);
	if(src.empty())
	{
		throw std::runtime_error("Cannot open image!");
	}

	///*
	//DISPLAY NORMAL SOURCE
	DEBUG("START NORMAL DISPLAY");

	std::string normal_name = opath + "_src.jpg";

#if SOURCE
	cv::imwrite(normal_name, src);
#endif

	DEBUG("END NORMAL DISPLAY");
	//*/

	///*
	//HSV CONVERT
	DEBUG("START HSV CONVERT");

	std::string hsv_name = opath + "_hsv.jpg";
	cv::Mat hsv, hsv_display;

	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	cv::cvtColor(hsv, hsv_display, cv::COLOR_HSV2BGR);

#if HSV
	cv::imwrite(hsv_name, hsv_display);
#endif

	DEBUG("END HSV CONVERT");
	//*/

	///*
	//CUT OUT COLOURS
	DEBUG("START COLOUR MASKING");

	std::string masked_name = opath + "_mask.jpg";
	cv::Mat r_mask_l, r_mask_h, b_mask_l, b_mask_h, y_mask_l, y_mask_h, mask, masked, masked_display;

	cv::inRange(hsv, cv::Scalar(0, 120, 70), cv::Scalar(10, 255, 255), r_mask_l);
	cv::inRange(hsv, cv::Scalar(170, 120, 70), cv::Scalar(180, 255, 255), r_mask_h);
	//cv::inRange(hsv, cv::Scalar(20, 150, 0), cv::Scalar(30, 255, 255), b_mask_l);
	cv::inRange(hsv, cv::Scalar(20, 150, 0), cv::Scalar(30, 255, 255), b_mask_h);
	//cv::inRange(hsv, cv::Scalar(100, 100, 100), cv::Scalar(140, 255, 255), y_mask_l);
	cv::inRange(hsv, cv::Scalar(100, 100, 100), cv::Scalar(140, 255, 255), y_mask_h);
	mask = r_mask_l + r_mask_h /*+ b_mask_l*/ + b_mask_h /*+ y_mask_l*/ + y_mask_h;
	cv::bitwise_and(hsv, hsv, masked, mask);

	cv::cvtColor(masked, masked_display, cv::COLOR_HSV2BGR);

#if MASK
	cv::imwrite(masked_name, masked_display);
#endif

	DEBUG("END COLOUR MASKING");
	//*/

	///*
	//FIND CONTOURS
	DEBUG("START CONTOUR SEARCH");

	std::string contour_name = opath + "_contours.jpg";
	cv::Mat grayscale, binary, contour;
	std::vector<std::vector<cv::Point>> contours;

	cv::cvtColor(masked_display, grayscale, cv::COLOR_BGR2GRAY);
	cv::threshold(grayscale, binary, 1, 255, cv::THRESH_BINARY);

	cv::findContours(binary, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	std::vector<int> ids;
	double area = 0;
	double img_area = src.rows*src.cols;

#if 1
	std::cout << "Image Area: " << img_area << std::endl;
	std::cout << "Lower bounds: " << 0.05*img_area << std::endl;
	std::cout << "Upper bounds: " << 0.95*img_area << std::endl;
#endif

	for(size_t i = 0; i < contours.size(); i++)
	{
		area = cv::contourArea(contours[i]);
#if 0
		std::cout << "Area " << i << ": " << area << std::endl;
#endif
		if(area > 0.007*img_area && area < 0.95*img_area)
		{
			ids.push_back(i);
		}
	}

	std::vector<std::vector<cv::Point>> contours_poly;
	for(size_t i = 0; i < ids.size(); i++)
	{
		std::vector<cv::Point> base;
		contours_poly.push_back(base);
	}
	std::vector<cv::Rect> boundRect;

	for(size_t i = 0; i < ids.size(); i++)
	{
		cv::approxPolyDP(contours[ids[i]], contours_poly[i], 3, true);
		boundRect.push_back(boundingRect(contours_poly[i]));
	}

	contour = cv::Mat::zeros(binary.size(), CV_8UC3);
	for(size_t i = 0; i < ids.size(); i++)
	{
		cv::rectangle(contour, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(255, 255, 255), 2);
	}


#if CONTOUR
	cv::imwrite(contour_name, contour);
#endif

	DEBUG("END CONTOUR SEARCH");
	//*/

	///*
	//CROP TO CONTOUR
	DEBUG("START CROP");

	std::string crop_name = opath + "_crop";
	std::vector<cv::Mat> cropped;

	for(size_t i = 0; i < ids.size(); i++)
	{
		cropped.push_back(src(boundRect[i]));
	}

#if RESULT
	for(size_t i = 0; i < ids.size(); i++)
	{
		std::string i_name = crop_name + "_" + std::to_string(i) + ".jpg";
		cv::imwrite(i_name, cropped[i]);
	}
#endif

	DEBUG("END CROP");
	//*/
}

int main()
{
	//PROCESS
	std::string ipath = "./inputs/img_";
	std::string opath = "./outputs/img_";

	for(int i = 1; i < 516; i++)
	{
		std::cout << "Input: " << ipath + std::to_string(i) + ".jpg" << std::endl;
		std::cout << "Output: " << opath + std::to_string(i) + "/out" << std::endl;
		process(ipath + std::to_string(i) + ".jpg", opath + std::to_string(i) + "/out");
	}

	/*
	ipath += std::to_string(0) + ".jpg";
	opath += std::to_string(0);
	std::cout << "Input: " << ipath << std::endl;
	std::cout << "Output: " << opath << std::endl;
	process(ipath, opath);
	*/

	return 0;
}
