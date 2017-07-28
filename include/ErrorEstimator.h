#ifndef ERRORESTIMATOR_H
#define ERRORESTIMATOR_H

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Blob.h"
#include <math.h>
//#include <AgileMap.h>


class ErrorEstimator
{
    public:
        ErrorEstimator(int rows, int cols);

        /**
            Add to the errorListElement a new distance error.
            It is called from GammaRayDetector when a blob is found.
        */
        void updateErrorList(Blob* b, string fileName, string imagePath);


        /**
            Print on console the results.
        */
        void showResults();


    private:

        float computeFMeasure();

        float computeAccuracy();

        float computeSimpleAccuracy();

        float computeFalseNegativeRate();

        float computeFalsePositiveRate();

        /**
            Compute the precision
        */
        float computePrecision();

        /**
            Compute the recall
        */
        float computeRecall();

        /**
            Compute the specificity
        */
        float computeSpecificity();


        /**
            Returns the euclidean distance of the blob centroid from the center of the image
        */
        float getDistanceFromCenter(Blob* b);

        float getSphericalDistanceFromCenter(Blob *b);

        /**
            Update the TruePositiveCount attribute adding a true positive blob.
        */
        void addTruePositiveCount();
        /**
            Update the FalsePositiveCount attribute adding a true negative blob.
        */
        void addFalsePositiveCount();

        /**
            Update the FalseNegativeCount attribute adding a true negative blob.
        */
        void addFalseNegativeCount();

        /**
            Update the TrueNegativeCount attribute adding a true negative blob.
        */
        void addTrueNegativeCount();

        /**
            Returns the mean of the values of errorListElement.
        */
        float getDistanceErrorMean(vector<float> errorList);

        /**
            Returns the standard deviation of the values of errorListElement.
        */
        float getDistanceErrorStdDev(vector<float> errorList);


        float getChoenKappa();


        int getTotalInstances();

        vector<float> errorListElement;
        vector<float> sphericalDistanceErrorsList;

//        AgileMap* agileMapUtils;

        Point center;
        double centerL;
        double centerB;

        float tpCount;
        float fpCount;
        float tnCount;
        float fnCount;
        float fClass;
        float bClass;
        float precision;
        float recall;
        float specificity;

};

#endif // ERRORESTIMATOR_H
