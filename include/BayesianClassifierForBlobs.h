////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       agextspot-v2
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it,
//                Giancarlo Zollino giancarlo.zollino@gmail.com
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
////////////////////////////////////////////////////////////////////////////////////*/

#ifndef BAYESIANCLASSIFIERFORBLOBS_H
#define BAYESIANCLASSIFIERFORBLOBS_H

#include "AgileBlob.h"
#include <utility> // std::pair
#include "NormalDistribution.h"

using std::make_pair;

class BayesianClassifierForBlobs
{
    public:

        // Uses already computed distribution values, hard-coded as class attributes.
        BayesianClassifierForBlobs() :
              bgFrequency(0.5),
              fluxFrequency(0.5),
              bgAreaDistribution( 259.313, 114.337 ),
              bgPhotonsInBlob( 2.70782, 1.06004 ),
              bgPhotonsCloseness( 2.8795, 1.41748 ),
              fluxAreaDistribution( 240.756, 66.9674 ),
              fluxPhotonsInBlob( 4.26807, 1.97511 ),
              fluxPhotonsCloseness( 1.7331, 0.873429 ) { }


        // Compute the probability for a Blob to be background and flux. Returns [ ("background", x%) , ("flux",1-x%) ]
        vector<pair<string,double> > classify(Blob* b);



    private:

        double bgFrequency; // Assuming 0.5
        double fluxFrequency; // Assuming 0.5

        // background
        NormalDistribution bgAreaDistribution;
        NormalDistribution bgPhotonsInBlob;
        NormalDistribution bgPhotonsCloseness;

        // flux
        NormalDistribution fluxAreaDistribution;
        NormalDistribution fluxPhotonsInBlob;
        NormalDistribution fluxPhotonsCloseness;


        // Compute the probability of an attribute value, given a distribution, from the Gaussian Equation.
        // This probability will be inserted in the Bayesian Theorem in the classify() method.
        double computeProbabilityFromDistribution(double x, NormalDistribution distribution);


     //////////////////////////////////////////////////////////////////////////////////////////////////////
     //
     // Distribution 1
     //
     // Observables:
     //     - PhotonsInBlob : number of photons inside the blobs
     //     - PhotonsCloseness : closeness index among photons inside the blob
     //     - Area : the blob's area (degree)
     //
     // Generated with https://github.com/Leofaber/AG_extspot-v2-bayesian-model-evaluator
     //
     // Dataset:
     //
     //    - TODO: NEED DATASET GENERATION PARAMETERs !!!
     //
     //    - 972 background images -> 2152 backgroud blobs
     //    - 4441 flux images      -> 4357 flux blobs
     //    - 5413 total images     -> 6509 total blobs
     //
     // Blobs Finder parameters:
     //     - Smoothing [31x31 sigma=7.5]
     //     - Thresholding (on the grey level's average)
     //     - Post filtering => extract a blob iff contains a number of photons >= 2
     //
     // Bayesian Model Evaluator Post Filtering on FLUX images:
     //     - extract the i-blob in the j-image iff:
     //         - IS CENTERED (15 pixels offset)
     //         - CONTAINS A NUMBER OF PHOTONs > 1
     //         - IS THE FIRST FLUX FOUND IN j
     //
     ////////////////////////////////////////////////////////////////////////////////////////////




     /*
     // background
        NormalDistribution bgPhotonsInBlob( 2.92556, 1.30257 );
        NormalDistribution bgPhotonsCloseness( 3.50439, 1.63845 );
     //
     // flux
        NormalDistribution fluxPhotonsInBlob( 4.33456, 2.03592 );
        NormalDistribution fluxPhotonsCloseness( 1.96242, 0.999395 );
     //
     */
};

#endif // BAYESIANCLASSIFIERFORBLOBS_H
