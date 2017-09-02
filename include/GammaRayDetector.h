#pragma once

#include "BayesianClassifierForBlobs.h"
#include "FitsToCvMatConverter.h"
#include "BlobsFinder.h"
#include "FileWriter.h"

//#include <AgileMap.h>


using namespace std;

class GammaRayDetector
{
public:

    /**

    */
	GammaRayDetector(string imagePath, string outputLogName, float classificationThreshold);

	/**
        Open the fits file, convert it to Mat image, finds blobs, classify them with bayesian classifier, compute the
        centroid of the blob in galactic coordinate. Return the blob if found, eturn null_ptr instead.
    */

    void detect();





private:


    vector<Blob*> extractBlobs(string imagePath,Mat tempImage);

    /**
        Given a blob list, for each blob, call Reverend Bayes to predict the probabilities and write them to an output file.
    */
    void classifyBlobs(vector<Blob*> blobs,Mat tempImage);

	string imagePath;
	string outputLogName;
	float classificationThreshold;

 	BayesianClassifierForBlobs* reverendBayes;
	//AgileMap* agileMapUtils;
 };
