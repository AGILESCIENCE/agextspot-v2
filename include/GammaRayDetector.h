#pragma once

#include "BayesianClassifierForBlobs.h"
#include "FitsToCvMatConverter.h"
#include "BlobsFinder.h"
#include "FileWriter.h"
#include "ExpRatioEvaluator.h"

#include <AgileMap.h>


using namespace std;

class GammaRayDetector
{
public:

    /**
        User has to specify the path to the FITS file, the name of the output file, the classification threshold as an interger or a floating point number
    */
	GammaRayDetector(string imageCtsPath, string outputLogName, float classificationThreshold, const char *imageExpPath, bool doExpEvaluationOnNormalizedMap, double minTreshold, double maxTreshold); 

	/**
        Open the fits file, convert it to Mat image, extract blobs, classify them with bayesian classifier.
    */
    void detect();


private:


    /**
        Given a blob call Reverend Bayes to predict the probabilities.
    */
    double classifyBlob(Blob* Blob);
    string extractFileNameFromImagePath(string imagePath);

	string imagePath;
	string outputLogName;
	string fileName;
	float classificationThreshold;

	const char *imageExpPath;
	bool doExpEvaluationOnNormalizedMap;
	double minTreshold;
	double maxTreshold;
	
	
 	BayesianClassifierForBlobs* reverendBayes;
    AgileMap* agileMapUtils;
	ExpRatioEvaluator* exp;
 };
