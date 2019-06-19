////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       AG_ap5
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it, Giancarlo Zollino
//
// NOTICE
//       Any information contained in this software
//       is property of the AGILE TEAM and is strictly
//       private and confidential.
//       Copyright (C) 2005-2019 AGILE Team. All rights reserved.
/*

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
////////////////////////////////////////////////////////////////////////////////////

#include "BayesianClassifierForBlobs.h"





vector<pair<string,double> > BayesianClassifierForBlobs::classify(Blob* _b){

    AgileBlob * b = static_cast<AgileBlob *>(_b);


    vector<pair<string,double> > prediction;

    //double pixelMean = b->getPixelsMean();
    double area = b->get_blob_area_deg();
    double photons = b->get_number_of_photons();
    double photonsCloseness = b->get_photons_closeness();
    /// ADD ATTRIBUTE

    //double bgPM = computeProbabilityFromDistribution(pixelMean,bgPixelMeanDistribution);
    double bgA = computeProbabilityFromDistribution(area, bgAreaDistribution);
    double bgP = computeProbabilityFromDistribution(photons, bgPhotonsInBlob);
    double bgPC = computeProbabilityFromDistribution(photonsCloseness, bgPhotonsCloseness);
    /// ADD DISTR VALUE


    //double fluxPM = computeProbabilityFromDistribution(pixelMean,fluxPixelMeanDistribution);
    double fluxA = computeProbabilityFromDistribution(area, fluxAreaDistribution);
    double fluxP = computeProbabilityFromDistribution(photons, fluxPhotonsInBlob);
    double fluxPC = computeProbabilityFromDistribution(photonsCloseness, fluxPhotonsCloseness);
    /// ADD DISTR VALUE

    /// THE LIKELYHOOD CALCULUS .. MODIFY THIS TO CHANGE ATTRIBUTES OF ANALYSIS
    double likelyHoodOfBackground =bgA*bgPC*bgP*bgFrequency;
    double likelyHoodOfFlux = fluxA*fluxPC*fluxP*fluxFrequency;


    /// NORMALIZATION STEP TO GET A PROBABILITY
    double sum = likelyHoodOfBackground+likelyHoodOfFlux;

    double probabilityOfBg = likelyHoodOfBackground/sum;
    double probabilityOfFlux = likelyHoodOfFlux/sum;






    prediction.push_back(make_pair("Background",probabilityOfBg));
    prediction.push_back(make_pair("Flux",probabilityOfFlux));


   // cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is background " << " with probability: " << probabilityOfBg*100<<"%"<<endl;
  //  cout << "[Reverend Bayes] "<< "Point "<<b->getFloatingCentroid()<<" is flux " << " with probability: " << probabilityOfFlux*100<<"%"<<endl;


    return prediction;
}


double BayesianClassifierForBlobs::computeProbabilityFromDistribution(double x,NormalDistribution distribution){
    double mean = distribution.getMean();
    double stddev = distribution.getStdDev();


    double probability = 0;

    double multiplier = 1 / ( sqrt(2*M_PI*pow(stddev,2))   );

    double exponent = -1 *( (pow(x-mean,2)) / (2*pow(stddev,2)) );

    double exponential = exp(exponent);

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
