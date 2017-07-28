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


        normal_distribution<double> bgPixelMeanDistribution = normal_distribution<double>( 1.19936, 0.06062);
        normal_distribution<double> fluxPixelMeanDistribution = normal_distribution<double>( 1.67467, 0.368148);
        normal_distribution<double> bgAreaDistribution = normal_distribution<double>( 131.769, 36.872);
        normal_distribution<double> fluxAreaDistribution = normal_distribution<double>( 303.736, 111.971);
        normal_distribution<double> bgPhotonsInBlobDistribution = normal_distribution<double>( 1.00452, 0.322571);
        normal_distribution<double> fluxPhotonsInBlobDistribution = normal_distribution<double>( 2.96629, 1.37758);
        normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 0.555733, 1.32262);
        normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 3.83731, 2.48066);

        /// ADD DISTRIBUTION IF NEEDED

        float countBgBlobs = 221;
        float countFluxBlobs = 178;
        float bgFrequency = 0.553885;
        float fluxFrequency = 0.446115;







};

#endif // BAYESIANCLASSIFIERFORBLOBS_H
