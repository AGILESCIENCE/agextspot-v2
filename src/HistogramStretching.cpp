#include "HistogramStretching.h"

HistogramStretching::HistogramStretching()
{
	//ctor
}

Mat HistogramStretching::nonLinearStretch(Mat inputImage, float r)
{
    Mat image = inputImage.clone();
	int rows = image.rows;
	int cols = image.cols;
	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < cols; ++j){
			int pixelValue = (int)image.at<uchar>(i, j);
			int newPixelValue = pow(255, 1 - r)* pow(pixelValue,r);
			image.at<uchar>(i, j) = (uchar)newPixelValue;
		}
	}
    return image;

}

Mat HistogramStretching::linearStretch(Mat inputImage){
	Mat image = inputImage.clone();
	int rows = image.rows;
	int cols = image.cols;
	int max = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
		{
			int pixelValue = (int)image.at<uchar>(i, j);
			if (pixelValue > max)
				max = pixelValue;
		}
	}
	//cout << "max: " << max;


	if (max == 0){
		cout << "[Stretching] Error: max = 0!" << endl;
		return inputImage;
	}
	// slope for linear stretching
	int slope = int(255 / max);


	// linear stretching
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			int pixelValue = (int)image.at<uchar>(i, j);
			int newPixelValue = pixelValue*slope;
			image.at<uchar>(i, j) = (uchar)newPixelValue;

		}
	}
	return image;

}
