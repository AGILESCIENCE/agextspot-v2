#ifndef THRESHOLDBACKGROUNDEVALUATOR_H
#define THRESHOLDBACKGROUNDEVALUATOR_H

#include "FolderManager.h"
#include "FitsToCvMatConverter.h"
#include "BlobsFinder.h"
#include <random>

enum AttributeType { PIXELMEAN = 1, AREA = 2, PHOTONS = 3, PHOTONSCLOSENESS = 4}; /// ADD ATTRIBUTE IF NEEDED

class BlobsDistributionEvaluator
{
    public:
        /**
            Called by BayesianClassifier to get the needed distribution.
        */
        static normal_distribution<double> getMeanAndDeviation(string pathFitsFiles, AttributeType attribute, bool debugMode, string dataType);


        /**
            Called by BayesianClassifier to get the needed class frequencies.
        */
        static float getFrequencyOfClass(string pathFitsFile, bool debugMode, string dataType);



    private:
         BlobsDistributionEvaluator();


         /**
            Search in every blob of every images, the values of the desired attribute. E.g. number of photons...
         */
         static vector<float> getAttributesInFitsFile(string pathFitsFiles, string pathToFile, AttributeType attribute, bool debugMode, string dataType);
};

#endif // THRESHOLDBACKGROUNDEVALUATOR_H
