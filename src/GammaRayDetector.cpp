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

	FitsData fitsData = FitsToCvMatConverter::extractImageAndObsDateFromFitsFile(fitsFilePath);

    vector<Blob*> blobs = BlobsFinder::findBlobs(fitsData.image);

    classifyBlobs(blobs,tempImage,fitsData.observationDates);


}




string GammaRayDetector::classifyBlobs(vector<Blob*> blobs,Mat tempImage, DateObsEnd observationDates)
{

    FileWriter::write2FileHeader(imagePath, outputLogName, classificationThreshold);


    vector<pair<string, Blob* > > labelledBlobs;


    if(blobs.size() > 0)
    {

        for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++)
        {

            Blob* b = *i;

            vector<pair<string,double> > predicted = reverendBayes->classify(b);

            //double bgProbability   = predicted[0].second;
            double fluxProbability = predicted[1].second;

            double gaLong = agileMapUtils->l(b->getCentroid().x,b->getCentroid().y);
            double gaLat  = agileMapUtils->b(b->getCentroid().x,b->getCentroid().y);

            string information2Print = "["+to_string(gaLong)+","+to_string(gaLat)+"], "+to_string(fluxProbability*100)+"%, "+observationDates.dateObs;

            //string information2Print = "["+to_string(b->getCentroid().x)+","+to_string(b->getCentroid().y)+"],"+to_string(fluxProbability*100)+"%";


            if(fluxProbability >= classificationThreshold)
            {
                 information2Print = "SOURCE, "+information2Print;
                 FileWriter::write2FileBody(information2Print,outputLogName);
                 FileWriter::write2SourcesFile(imagePath,information2Print,outputLogName);

            }
            else
            {
                 information2Print = "BG, "+information2Print;
                 FileWriter::write2FileBody(information2Print,outputLogName);

            }


        } // end for loop
    }
    else
    {
        FileWriter::write2FileBody("No blob was found",outputLogName);

    }


}







