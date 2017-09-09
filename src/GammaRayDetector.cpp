#include "GammaRayDetector.h"

GammaRayDetector::GammaRayDetector(string _imagePath, string _outputLogName,float _classificationThreshold){
    imagePath = _imagePath;
    outputLogName = _outputLogName;
    classificationThreshold = _classificationThreshold/100;

	reverendBayes = new BayesianClassifierForBlobs();
	agileMapUtils = new AgileMap(imagePath.c_str());

 }






void GammaRayDetector::detect()
{


    string observationDate = FitsToCvMatConverter::getObservationDateFromFitsFile(imagePath);

    /// converte un file fits in un'immagine Mat di opencv
	Mat photonsImage = FitsToCvMatConverter::convertFitsToCvMat(imagePath);

    /// tira fuori una lista con tutti i BLOBS
    vector<Blob*> blobs = BlobsFinder::findBlobs(photonsImage);


    string information2Print="DETECTION OF: "+imagePath+" -OBSD: "+observationDate+" -T: "+classificationThreshold*100+"\n";
    string information2PrintForSources="";

    if(blobs.size() > 0)
    {
        for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++)
        {
            Blob* b = *i;

            /// CLASSIFICATION
            double fluxProbability = classifyBlob(b);

            double gaLong = agileMapUtils->l(b->getCentroid().x,b->getCentroid().y);
            double gaLat  = agileMapUtils->b(b->getCentroid().x,b->getCentroid().y);
            string tempString = to_string(gaLong)+","+to_string(gaLat)+"], "+to_string(fluxProbability*100)+"%";

            /// LABELING
            if(fluxProbability >= classificationThreshold){
                information2Print += "\nSOURCE, ["+tempString;
                information2PrintForSources += "\n"+tempString;
            }else{
                information2Print += "\nBG, ["+tempString;
            }
        }
    }else{

        information2Print += "\nNo blobs has been found!";
    }


        FileWriter::write2File(outputLogName,information2Print);
        FileWriter::write2File(outputLogName+"_sources",information2PrintForSources);

}




double GammaRayDetector::classifyBlob(Blob* b)
{
    vector<pair<string,double> > predicted = reverendBayes->classify(b);
    //double bgProbability   = predicted[0].second;
    double fluxProbability = predicted[1].second;

    return fluxProbability;
}




