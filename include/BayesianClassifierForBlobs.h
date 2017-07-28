#ifndef BAYESIANCLASSIFIERFORBLOBS_H
#define BAYESIANCLASSIFIERFORBLOBS_H

#include "BlobsDistributionEvaluator.h"

class BayesianClassifierForBlobs
{
    public:

        /**
            Uses already computed distribution values, hardcoded into this constructor.
        */
        BayesianClassifierForBlobs(bool debugMode);


        /**
            Use the BlobsDistributionEvaluator class to compute the frequencies of the classes background/flux,
            and the distributions of the attributes.
        */
        void learnFromTrainingSet(string _trainingSetPath);



        /**
            Compute the probability for a Blob to be background and flux. Returns [ ("background", x%) , ("flux",1-x%) ]
        */
        vector<pair<string,float> > classify(Blob* b);

        /**
            Prints the mean and standard deviation of all the distributions
        */
        void showDistributionsValues();

    private:

        string simulatedBackgroundFitsFilesPath;
        string simulatedFluxFitsFilesPath;
        bool debugMode;




        /**
            Compute the probability of an attribute value, given a distribution, from the Gaussian Equation.
            This probability will be inserted in the Bayesian Theorem in the classify() method.
        */
        float computeProbabilityFromDistribution(float x,normal_distribution<double> distribution);


        normal_distribution<double> bgPixelMeanDistribution = normal_distribution<double>( 5.25778, 0.336926);
        normal_distribution<double> fluxPixelMeanDistribution = normal_distribution<double>( 11.8688, 5.23051);
        normal_distribution<double> bgAreaDistribution = normal_distribution<double>( 16.9377, 10.1723);
        normal_distribution<double> fluxAreaDistribution = normal_distribution<double>( 97.3049, 37.9206);
        normal_distribution<double> bgPhotonsInBlobDistribution = normal_distribution<double>( 1.53307, 0.954141);
        normal_distribution<double> fluxPhotonsInBlobDistribution = normal_distribution<double>( 10.3087, 6.39049);
        normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 1.28323, 0.908676);
        normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 2.7578, 0.972047);

        /// ADD DISTRIBUTION IF NEEDED

        float countBgBlobs = 514;
        float countFluxBlobs = 528;
        float bgFrequency = 0.493282;
        float fluxFrequency = 0.506718;







};

#endif // BAYESIANCLASSIFIERFORBLOBS_H
