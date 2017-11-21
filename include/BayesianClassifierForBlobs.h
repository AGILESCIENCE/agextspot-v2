/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#ifndef BAYESIANCLASSIFIERFORBLOBS_H
#define BAYESIANCLASSIFIERFORBLOBS_H

#include <Blob.h>
#include <random>

using namespace std;

class BayesianClassifierForBlobs
{
    public:

        /**
            Uses already computed distribution values, hard-coded as class attributes.
        */
        BayesianClassifierForBlobs();


        /**
            Compute the probability for a Blob to be background and flux. Returns [ ("background", x%) , ("flux",1-x%) ]
        */
        vector<pair<string,double> > classify(Blob* b);



    private:


        /**
            Compute the probability of an attribute value, given a distribution, from the Gaussian Equation.
            This probability will be inserted in the Bayesian Theorem in the classify() method.
        */
        double computeProbabilityFromDistribution(double x,normal_distribution<double> distribution);


         /// Big Dataset


	/* STATIC KERNEL [21x21 sigma=4] */
	//normal_distribution<double> bgPhotonsInBlobDistribution = normal_distribution<double>( 1.22337, 0.560462 );
        //normal_distribution<double> fluxPhotonsInBlobDistribution = normal_distribution<double>( 4.14473, 1.96385 );

  	//normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 15.1198, 7.05518 ); //degrees
	//normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 1.92534, 1.00237 ); // degrees

	
	/* PARAMETRIZED KERNEL WITH PSF [31x31 sigma=7.5] , DEGREES DISTANCES */

	// background
	normal_distribution<double> bgPhotonsInBlobDistribution = normal_distribution<double>( 1.77717, 1.2559 );
  	normal_distribution<double> bgPhotonsClosenessDistribution = normal_distribution<double>( 2.03695, 2.16098 );  

	// flux
        normal_distribution<double> fluxPhotonsInBlobDistribution = normal_distribution<double>( 4.33456, 2.03592 );
	normal_distribution<double> fluxPhotonsClosenessDistribution = normal_distribution<double>( 1.96242, 0.999395 );  










	// DEPRECATED
        normal_distribution<double> bgPixelMeanDistribution = normal_distribution<double>( 1.220504, 0.119515);
        normal_distribution<double> fluxPixelMeanDistribution = normal_distribution<double>( 1.81847, 0.458405);
	// DEPRECATED
        normal_distribution<double> bgAreaDistribution = normal_distribution<double>( 143.977, 66.2592 );
        normal_distribution<double> fluxAreaDistribution = normal_distribution<double>( 401.355, 143.053 );

        


        /// ADD DISTRIBUTION IF NEEDED

        int countBgBlobs;
        int countFluxBlobs;
        double bgFrequency;
        double fluxFrequency;

};

#endif // BAYESIANCLASSIFIERFORBLOBS_H
