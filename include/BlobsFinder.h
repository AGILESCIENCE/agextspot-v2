#ifndef BLOBSFINDER_H
#define BLOBSFINDER_H

#include <map>

#include "Blob.h"
#include "Thresholder.h"
#include "GaussianFilterer.h"
#include "HistogramStretching.h"

using namespace cv;
using namespace std;


class BlobsFinder
{
    public:

        /**
            Return a list of Blobs. In order to find one or more Blob it uses:
                - non linear stretching
                - gaussian filtering
                - find contours procedure
         */
        static vector<Blob*> findBlobs(Mat image);

    private:
        BlobsFinder();

};

#endif // BLOBSFINDER_H
