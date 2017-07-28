#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "stdio.h"
#include "Blob.h"
#include "HistogramStretching.h"
using namespace cv;
using namespace std;

class ImagePrinter
{
public:
	static void printImageInConsole(Mat inputImage);
	static void printImageInWindowWithStretching(Mat inputImage, string windowName);
	static void printImageInWindow(Mat inputImage, string windowName);
	static void printImageBlob(Mat inputImage, Blob* b, string windowName);
	static void printImageBlobs(int rows,int cols,vector<Blob*> blobs, string windowName);

private:
	ImagePrinter();
};
