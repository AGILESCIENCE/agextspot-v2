#pragma once

#include <iostream>
#include <stdio.h>

#include "FitsToCvMatConverter.h"
#include "FolderManager.h"
#include "BlobsFinder.h"
#include "ErrorEstimator.h"
#include "BayesianClassifierForBlobs.h"
#include "FileWriter.h"
//#include <AgileMap.h>
#include <math.h>
#define _USE_MATH_DEFINES

using namespace std;

class GammaRayDetector
{
public:

    /**

    */
	GammaRayDetector(string imagePath, string outputLogName);

	/**
        Open the fits file, convert it to Mat image, finds blobs, classify them with bayesian classifier, compute the
        centroid of the blob in galactic coordinate. Return the blob if found, eturn null_ptr instead.
    */
 	void detect();

    /**
        Computes the distributions for the classifier from a training set
    */
    void trainBayesianClassifier(string trainingSetPath);



private:



    /**
        Given a blob list, for each blob, call Reverend Bayes to predict the probabilities and write them to an output file.
    */
    void classifyBlobs(vector<Blob*> blobs);

	string imagePath;
	string outputLogName;
 	BayesianClassifierForBlobs* reverendBayes;
	//AgileMap* agileMapUtils;
 };
