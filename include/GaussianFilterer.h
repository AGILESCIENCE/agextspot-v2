#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;


class GaussianFilterer
{
public:

	GaussianFilterer(Size kernelSize, double sigma);
    /**
        Filters the image and returns a smoothed image
    */
	Mat filter(Mat inputImg);
private:
	Size kernelSize;
	double sigma;
};
