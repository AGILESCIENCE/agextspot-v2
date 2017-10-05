/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


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
