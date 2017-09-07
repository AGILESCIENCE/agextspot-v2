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

    FileWriter::write2FileHeader(imagePath,observationDate, outputLogName, classificationThreshold);

    if(blobs.size() > 0)
    {

        for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++)
        {

            Blob* b = *i;

            double fluxProbability = classifyBlob(b);

            double gaLong = agileMapUtils->l(b->getCentroid().x,b->getCentroid().y);
            double gaLat  = agileMapUtils->b(b->getCentroid().x,b->getCentroid().y);

            string information2Print = "["+to_string(gaLong)+","+to_string(gaLat)+"], "+to_string(fluxProbability*100)+"%";
            //string information2Print = "["+to_string(b->getCentroid().x)+","+to_string(b->getCentroid().y)+"], "+to_string(fluxProbability*100)+"%";


            if(fluxProbability >= classificationThreshold){
                FileWriter::write2FileBody("SOURCE, "+information2Print,outputLogName);
                FileWriter::write2SourceFile(imagePath,"SOURCE, "+information2Print+", "+observationDate);

            }else
            {
                FileWriter::write2FileBody("BG,"+information2Print,outputLogName);
            }

        }
    }else{

        FileWriter::write2FileBody("No blobs has been found!",outputLogName);
    }
}




double GammaRayDetector::classifyBlob(Blob* b)
{
    vector<pair<string,double> > predicted = reverendBayes->classify(b);
    //double bgProbability   = predicted[0].second;
    double fluxProbability = predicted[1].second;

    return fluxProbability;
}




