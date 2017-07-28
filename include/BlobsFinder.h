#ifndef BLOBSFINDER_H
#define BLOBSFINDER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>
#include <map>
#include "Blob.h"
#include "Thresholder.h"
#include "GaussianFilterer.h"
#include "ImagePrinter.h"
#include "HistogramStretching.h"

using namespace cv;
using namespace std;


class BlobsFinder
{
    public:

        /**
            Return a list of Blobs. In order to find a Blob it does:
                - non linear stretching
                - gaussian filtering
                - thresholding
        */
        static vector<Blob*> findBlobs(Mat image, bool debugMode);

    private:
        BlobsFinder();

};

#endif // BLOBSFINDER_H
