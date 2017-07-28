#include "BlobsFinder.h"

BlobsFinder::BlobsFinder()
{
    //ctor
}

vector<Blob*> BlobsFinder::findBlobs(Mat tempImage, bool debugMode) {

    Mat photonImage = tempImage.clone();


    /// STRETCHING
 	float r = 0.05;
	tempImage = HistogramStretching::nonLinearStretch(tempImage,r);


	/// PRINTING IMAGE
	if(debugMode){
        cout << "Stretching complete" <<endl;
      //  ImagePrinter::printImageInConsole(tempImage);
        ImagePrinter::printImageInWindow(tempImage,"Non linear stretching");

	}


    /// GAUSSIAN FILTERING
	GaussianFilterer gaussianFilter(Size(21, 21), 4); // 17x17   2.5    23x23   3
	tempImage = gaussianFilter.filter(tempImage);


	/// PRINTING IMAGE
	if(debugMode){
        cout << "Gaussian Filtering complete" << endl;
       // ImagePrinter::printImageInConsole(tempImage);
        ImagePrinter::printImageInWindowWithStretching(tempImage, "Gaussian smoothing");
	}





	/// COMPUTING THRESHOLD
	//float threshold = Thresholder::getThresholdFromPeaksMethod(tempImage);
	//int threshold = Thresholder::getThresholdFromPercentileMethod(tempImage,50); // 98.5


 	/// DO THRESHOLDING
	//tempImage = Thresholder::makeThresholding(tempImage, threshold);

	/// PRINTING IMAGE
	/*if(debugMode){
        cout << "Thresholding complete." << endl;
        //ImagePrinter::printImageInConsole(tempImage);
        ImagePrinter::printImageInWindowWithStretching(tempImage, "Thresholding");
	}*/


	/// FIND THE CONTOUR OF EACH BLOB

    vector<Blob*> blobs;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    Mat copyOfImage = tempImage.clone();
    findContours( copyOfImage, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);


    for(vector<vector<Point> >::iterator i = contours.begin(); i < contours.end(); i++){

        vector<Point> currentBlob = *i;
        /// CREATING A BLOB
        blobs.push_back(new Blob(currentBlob,tempImage,photonImage,debugMode));

    }
    if(debugMode)
        waitKey(0);

    return blobs;
}

