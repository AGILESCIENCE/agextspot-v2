#ifndef FILTERER_H
#define FILTERER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;


class Filterer
{
    public:
        Filterer(Size kernelSize, double sigma);
        virtual ~Filterer();
        Mat filter(Mat inputImg);
    protected:
    private:
        Size kernelSize;
        double sigma;
};

#endif // FILTERER_H
