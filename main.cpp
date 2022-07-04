#include "Enums.hpp"

#include <visualizer.hpp>

#include <iostream>
#include <cstdlib>
using namespace std;

#include <unistd.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)

#include "opencv2/opencv.hpp"
using namespace cv;

int main()
{
	Mat src = cv::imread("data/stop_sign.jpg");
	Mat src2 = cv::imread("data/1l.jpg");
	if(src.empty())
	{
		throw runtime_error("Cannot open image!");
	}

	visualizer::load_cfg("data/main.visualizer.yaml");

	const uint16_t width = 10;
	const uint16_t height = 20;
	static uint8_t pix[width*height*3];
	for(uint16_t y = 0; y < height; y++)
	{
		for(uint16_t x = 0; x < width; x++)
		{
			uint32_t i = (y*width + x)*3;
			// Red.
			pix[i+0] = 0;
			pix[i+1] = 0;
			pix[i+2] = 255;
		}
	}

	for(uint16_t y = 3; y < height-3; y++)
	{
		for(uint16_t x = 3; x < width-3; x++)
		{
			uint32_t i = (y*width + x)*3;
			// Blue.
			pix[i+0] = 255;
			pix[i+1] = 0;
			pix[i+2] = 0;
		}
	}
	
//////////////////////////////////////////////////////////////////////////////////	

	/*
	//HSV CONVERT
	Mat hsv;

	cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

	visualizer::img::show("s", hsv);
	*/
	
	/*
	//EDGE DETECTION
	Mat edge_x, edge_y, edge;
	
	cv::Scharr(src, edge_x, -1, 1, 0);
	cv::Scharr(src, edge_y, -1, 0, 1);
	
	cv::addWeighted(edge_x, 0.5, edge_y, 0.5, 0, edge);

	visualizer::img::show("s", edge);
	*/

	/*
	//SIFT
	auto siftPtr = cv::SIFT::create();
    std::vector<cv::KeyPoint> keypoints;
    siftPtr->detect(src, keypoints);

	Mat sift;
	cv::drawKeypoints(src, keypoints, sift);

	visualizer::img::show("s", sift);
	*/
	
	/*
	//SEGMENTATION
	Mat segKernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
	
	Mat laplace;
	cv::filter2D(src, laplace, CV_32F, segKernel);
	Mat sharp;
	src.convertTo(sharp, CV_32F);
	Mat result = sharp - laplace;
	
	result.convertTo(result, CV_8UC3);
	laplace.convertTo(laplace, CV_8UC3);
	
	visualizer::img::show("s", result);
	
	Mat binary;
	cv::cvtColor(result, binary, COLOR_BGR2GRAY);
	cv::threshold(binary, binary, 40, 255, THRESH_BINARY | THRESH_OTSU);
	
	visualizer::img::show("v", binary);
	*/

//////////////////////////////////////////////////////////////////////////////////	

	visualizer::img::show("src", pix, width, height);
	visualizer::img::show("h", src);
	visualizer::img::show("r", src2);

	int th_start_h0 = 20;

	while(true)
	{
		//th_start_h0 = 20;
		visualizer::slider::slider(
			"/win0/upper_half/upper_rigth_corner/th_start_h0",
			th_start_h0,
			[&](int& value)
			{
				DEBUG(th_start_h0);
			}
		);

		for(int i = 0; i < 3; i++)
		{
			visualizer::slider::update();
			DEBUG(th_start_h0);
			sleep(1);
		}
	}

	return 0;
}