#include "BayesianClassifierForBlobs.h"



BayesianClassifierForBlobs::BayesianClassifierForBlobs(bool _debugMode){
    debugMode = _debugMode;

    /**
        Prints everythings
    */
   // showDistributionsValues();
}



void BayesianClassifierForBlobs::learnFromTrainingSet(string _trainingSetPath){

    simulatedBackgroundFitsFilesPath = _trainingSetPath+"/bg";

    simulatedFluxFitsFilesPath = _trainingSetPath+"/flux";


    cout << "\n** Learning from training set **  Debug = "<<debugMode<<"\n" << endl;

    /// Frequency of classes
    countBgBlobs = BlobsDistributionEvaluator::getFrequencyOfClass(simulatedBackgroundFitsFilesPath, debugMode, "BG");
    countFluxBlobs = BlobsDistributionEvaluator::getFrequencyOfClass(simulatedFluxFitsFilesPath, debugMode, "FLUX");

    float total = countBgBlobs + countFluxBlobs;

    bgFrequency = countBgBlobs/total;
    fluxFrequency = countFluxBlobs/total;






    /**
        ADD DISTRIBUTION IF NEEDED
    */

    bgPixelMeanDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedBackgroundFitsFilesPath, PIXELMEAN ,debugMode, "BG");

    fluxPixelMeanDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedFluxFitsFilesPath, PIXELMEAN, debugMode, "FLUX");

    bgAreaDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedBackgroundFitsFilesPath, AREA ,debugMode, "BG");

    fluxAreaDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedFluxFitsFilesPath, AREA, debugMode, "FLUX");

    bgPhotonsInBlobDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedBackgroundFitsFilesPath, PHOTONS, debugMode, "BG");

    fluxPhotonsInBlobDistribution= BlobsDistributionEvaluator::getMeanAndDeviation(simulatedFluxFitsFilesPath, PHOTONS, debugMode, "FLUX");

    bgPhotonsClosenessDistribution = BlobsDistributionEvaluator::getMeanAndDeviation(simulatedBackgroundFitsFilesPath, PHOTONSCLOSENESS, debugMode, "BG");

    fluxPhotonsClosenessDistribution= BlobsDistributionEvaluator::getMeanAndDeviation(simulatedFluxFitsFilesPath, PHOTONSCLOSENESS, debugMode, "FLUX");


     /**
        Prints everythings
    */
    showDistributionsValues();
}

vector<pair<string,float> > BayesianClassifierForBlobs::classify(Blob* b){

    vector<pair<string,float> > prediction;

    float pixelMean = b->getPixelsMean();
    float area = b->getNumberOfPixels();
    float photons = b->getPhotonsInBlob();
    float photonsCloseness = b->getPhotonsCloseness();
    /// ADD ATTRIBUTE

    float bgPM = computeProbabilityFromDistribution(pixelMean,bgPixelMeanDistribution);
    float bgA = computeProbabilityFromDistribution(area, bgAreaDistribution);
    float bgP = computeProbabilityFromDistribution(photons, bgPhotonsInBlobDistribution);
    float bgPC = computeProbabilityFromDistribution(photonsCloseness, bgPhotonsClosenessDistribution);
    /// ADD DISTR VALUE


    float fluxPM = computeProbabilityFromDistribution(pixelMean,fluxPixelMeanDistribution);
    float fluxA = computeProbabilityFromDistribution(area, fluxAreaDistribution);
    float fluxP = computeProbabilityFromDistribution(photons, fluxPhotonsInBlobDistribution);
    float fluxPC = computeProbabilityFromDistribution(photonsCloseness, fluxPhotonsClosenessDistribution);
    /// ADD DISTR VALUE

   // cout << bgA << " " << bgP << " " << bgFrequency << " "  << fluxA << " " << fluxP << " " << fluxFrequency << endl;


    /// THE LIKELYHOOD CALCULUS .. MODIFY THIS TO CHANGE ATTRIBUTES OF ANALYSIS
    float likelyHoodOfBackground = bgP*bgPC*bgFrequency;  //bgA*bgP*bgFrequency;
    float likelyHoodOfFlux = fluxP*fluxPC*fluxFrequency;  //fluxA*fluxP*fluxFrequency;


    float sum = likelyHoodOfBackground+likelyHoodOfFlux;

    float probabilityOfBg = likelyHoodOfBackground/sum;
    float probabilityOfFlux = likelyHoodOfFlux/sum;

    prediction.push_back(make_pair("Background",probabilityOfBg));
    prediction.push_back(make_pair("Flux",probabilityOfFlux));


    cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is background " << " with probability: " << probabilityOfBg*100<<"%"<<endl;
    cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is flux " << " with probability: " << probabilityOfFlux*100<<"%"<<endl;


    return prediction;
}


float BayesianClassifierForBlobs::computeProbabilityFromDistribution(float x,normal_distribution<double> distribution){
    float mean = distribution.mean();
    float stddev = distribution.stddev();


    /* Se la stddev fosse 0 */
    if(stddev == 0)
        stddev = 0.00001;

    float probability = 0;

    float multiplier = 1 / ( sqrt(2*M_PI*pow(stddev,2))   );

    float exponent = -1 *( (pow(x-mean,2)) / (2*pow(stddev,2)) );

    float exponential = exp(exponent);

    probability = multiplier * exponential;

    /*
    cout << "mean: " << mean << endl;
    cout << "stddev: " << stddev << endl;
    cout << "multiplier: " << multiplier << endl;
    cout << "exponent: " << exponent << endl;
    cout << "exponential: " << exponential << endl;
    cout << "probability: " << probability << endl;
    cout << "\n";
    */
    return probability;
}


void BayesianClassifierForBlobs::showDistributionsValues(){

    cout << "\n *** Number and Frequency of classes:" << endl;
    cout << "Background Blobs: " << countBgBlobs <<" - " <<bgFrequency  <<endl;
    cout << "Flux Blobs: " << countFluxBlobs <<" - " << fluxFrequency  <<endl;

    cout << "\n *** Analysis of PIXEL MEAN Background Blobs Distribution Complete:" << endl;
    cout << "Mean: " << bgPixelMeanDistribution.mean() <<endl;
    cout << "Variance: " << bgPixelMeanDistribution.stddev() <<endl;


	cout << "\n *** Analysis of PIXEL MEAN Flux Blobs Distribution Complete:" << endl;
    cout << "Mean: " << fluxPixelMeanDistribution.mean() <<endl;
    cout << "Variance: " << fluxPixelMeanDistribution.stddev() << endl;


    cout << "\n *** Analysis of AREA Background Blobs Distribution Complete:" << endl;
    cout << "Mean: " << bgAreaDistribution.mean() <<endl;
    cout << "Variance: " << bgAreaDistribution.stddev() << endl;


	cout << "\n *** Analysis of AREA Flux Blobs Distribution Complete:" << endl;
    cout << "Mean: " << fluxAreaDistribution.mean() <<endl;
    cout << "Variance: " << fluxAreaDistribution.stddev() << endl;


	cout << "\n *** Analysis of PHOTONS Background Blobs Distribution Complete:" << endl;
    cout << "Mean: " << bgPhotonsInBlobDistribution.mean() <<endl;
    cout << "Variance: " << bgPhotonsInBlobDistribution.stddev() << endl;


	cout << "\n *** Analysis of PHOTONS Flux Blobs Distribution Complete:" << endl;
    cout << "Mean: " << fluxPhotonsInBlobDistribution.mean() <<endl;
    cout << "Variance: " << fluxPhotonsInBlobDistribution.stddev() << endl;


    cout << "\n *** Analysis of PHOTONS CLOSENESS Background Blobs Distribution Complete:" << endl;
    cout << "Mean: " << bgPhotonsClosenessDistribution .mean() <<endl;
    cout << "Variance: " << bgPhotonsClosenessDistribution.stddev() << endl;


	cout << "\n *** Analysis of PHOTONS CLOSENESS Flux Blobs Distribution Complete:" << endl;
    cout << "Mean: " << fluxPhotonsClosenessDistribution.mean() <<endl;
    cout << "Variance: " << fluxPhotonsClosenessDistribution.stddev() << endl;

    getchar();
}
