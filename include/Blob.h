#ifndef BLOB_H
#define BLOB_H

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct Pixel{
    int greyLevel;
    Point p;
};

class Blob
{
    public:

        /**
            Create a new Blob starting from the contour pixels.
            Computes the centroid of the blob.
            Finds the pixels that are inside the blob.
            Finds the number of photons inside the blob.
            Compute the gray level pixel mean of the blob.
        */
        Blob(vector<Point>& c, Mat image, Mat photonImage);


        /**
            Return the centroid of the blob
        */
        Point getCentroid();
        Point getFloatingCentroid();
        /**
            Return the sum of all the grey levels of the pixels of the blob divided by the number of those pixels.
        */
        float getPixelsMean();

        /**
            Return all the contour points
        */
        vector<Point> getContour();

        /**
            Return the number of pixels inside the blob
        */
        int getNumberOfPixels();

        /**
            Return the number of photons inside the blob (before any stretching or smoothing)
        */
        float getPhotonsInBlob();

        /**
            Return all the Pixels (a Point with a grey level) that are inside the blob
        */
        vector<Pixel> getBlobPixels();


        /**
            Return the sum of the distances of each photons from the centroid divided by the number of photons
        */
        float getPhotonsCloseness();

        bool isCentered();


     private:

        vector<Point> contour;


        vector<Pixel> blobPixels;
        int numberOfPixels;
        vector<Pixel> computePixelsOfBlob(vector<Point>& c, Mat image);

        float photonsInBlob;
        float computePhotonsBlob(Mat photonImage);

        Point centroid;
        Point computeCentroid();

        Point floatingCentroid;
        Point computeFloatingCentroid();

        double l;
        double b;

        float pixelMean;
        float computePixelMean();

        float photonsCloseness;
        float computePhotonsCloseness(Mat photonImage);

        float getDistanceFromCentroid(Point p);

        void setGalacticCoords(double l, double b);
//        string getGalCoords();
};

#endif // BLOB_H
