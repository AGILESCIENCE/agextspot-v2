#include "Filterer.h"

Filterer::Filterer(Size _kernelSize, double _sigma)
{
    kernelSize=_kernelSize;
    sigma=_sigma;
}

Filterer::~Filterer()
{
    //dtor
}


Mat Filterer::filter(Mat inputImg) {
    Mat outputImg;
    GaussianBlur(inputImg,outputImg,kernelSize,sigma,0,0);
    return outputImg;
}
