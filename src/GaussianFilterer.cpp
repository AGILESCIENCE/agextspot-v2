#include "GaussianFilterer.h"



GaussianFilterer::GaussianFilterer(Size _kernelSize, double _sigma)
{
	kernelSize = _kernelSize;
	sigma = _sigma;
}


Mat GaussianFilterer::filter(Mat inputImg) {
	Mat outputImg;
	GaussianBlur(inputImg, outputImg, kernelSize, sigma, 0, 0);
	return outputImg;
}
