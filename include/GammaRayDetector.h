#pragma once

#include "BayesianClassifierForBlobs.h"
#include "FitsToCvMatConverter.h"
#include "BlobsFinder.h"
#include "FileWriter.h"

#include <AgileMap.h>


using namespace std;

class GammaRayDetector
{
public:

    /**
        User has to specify the path to the FITS file, the name of the output file, the classification threshold as an interger or a floating point number
    */
	GammaRayDetector(string imagePath, string outputLogName, float classificationThreshold);

	/**
        Open the fits file, convert it to Mat image, extract blobs, classify them with bayesian classifier.
    */
    void detect();


private:


    /**
        Given a blob list, for each blob, call Reverend Bayes to predict the probabilities and write them to an output file with the
        galactic coordinates of the blob.
    */
    void classifyBlobs(vector<Blob*> blobs, string observationDate);

	string imagePath;
	string outputLogName;
	float classificationThreshold;

 	BayesianClassifierForBlobs* reverendBayes;
    AgileMap* agileMapUtils;
 };
