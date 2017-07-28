#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include "Blob.h"
using namespace cv;
using namespace std;

class Thresholder
{
public:
    static Mat computeImageHistogram(Mat inputImage);
	static Mat makeThresholding(Mat inputImage, double t);
	static int getThresholdFromPeaksMethod(Mat histogram);
	static int getThresholdFromPercentileMethod(Mat image, float percentile);
    static vector<Blob> makeThresholdingOnBlobPixelMean(vector<Blob> blobs,float backgroundThresholdValue);

private:
	Thresholder();

};
