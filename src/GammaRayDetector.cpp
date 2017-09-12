#include "GammaRayDetector.h"

GammaRayDetector::GammaRayDetector(string _imagePath, string _outputLogName,float _classificationThreshold){
    imagePath = _imagePath;

    fileName = extractFileNameFromImagePath(imagePath);

    /// FIND .txt in outputLogName.
    size_t foundTxt = _outputLogName.find(".txt");
    if(foundTxt != string::npos)
        outputLogName = _outputLogName.substr(0,foundTxt);
    else
        outputLogName = _outputLogName;



    outputLogName +="_"+fileName+".txt";

    cout << "\nCreated Log File: " << outputLogName << endl;

    classificationThreshold = _classificationThreshold/100;

	reverendBayes = new BayesianClassifierForBlobs();
    agileMapUtils = new AgileMap(imagePath.c_str());

 }






void GammaRayDetector::detect()
{


    string observationDateUTC = FitsToCvMatConverter::getObservationDateFromFitsFile(imagePath);
    string observationDateTT = FitsToCvMatConverter::getObservationTimeFromFits(imagePath);

    /// converte un file fits in un'immagine Mat di opencv
	Mat photonsImage = FitsToCvMatConverter::convertFitsToCvMat(imagePath);

    /// tira fuori una lista con tutti i BLOBS
    vector<Blob*> blobs = BlobsFinder::findBlobs(photonsImage);



    string information2PrintForSources = "";

    int index = 1;
    if(blobs.size() > 0)
    {
        for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++)
        {
            information2PrintForSources += to_string(index);
            index++;
            Blob* b = *i;

            /// CLASSIFICATION
            double fluxProbability = classifyBlob(b);

            double gaLong = agileMapUtils->l(b->getCentroid().x,b->getCentroid().y);
            double gaLat  = agileMapUtils->b(b->getCentroid().x,b->getCentroid().y);
            string tempString = to_string(gaLong)+", "+to_string(gaLat)+", "+to_string(fluxProbability*100)+", "+observationDateUTC+", "+observationDateTT+", "+to_string(classificationThreshold*100)+"\n";
 //           string tempString = to_string(fluxProbability*100)+", "+observationDateUTC+", "+observationDateTT+", "+to_string(classificationThreshold*100)+"\n";


            /// LABELING
            if(fluxProbability >= classificationThreshold){
                 information2PrintForSources += ", SOURCE, "+tempString+", "+fileName;
            }else{
                 information2PrintForSources += ", BG, "+tempString+", "+fileName;

            }
        }
    }else{

        information2PrintForSources += "No blobs has been found!";
    }


     FileWriter::write2File(outputLogName,information2PrintForSources);

}




double GammaRayDetector::classifyBlob(Blob* b)
{
    vector<pair<string,double> > predicted = reverendBayes->classify(b);
    //double bgProbability   = predicted[0].second;
    double fluxProbability = predicted[1].second;

    return fluxProbability;
}


///   /ANALYSIS3/NGC4993_SHORT/output/20s_428630400.0_431308800.0/SCAN20_428630420.0_428630440.0_1.cts.gz
string GammaRayDetector::extractFileNameFromImagePath(string imagePath){


    int firstIndex=0;
    int secondIndex=imagePath.size();

    /// FINDING .cts.gz  OR .cts in imagePath
    size_t foundCtsGz = imagePath.find(".cts.gz");

    if (foundCtsGz!=string::npos){
        //cout << ".cts.gz foundCtsGz at: " << foundCtsGz <<endl;
        secondIndex = foundCtsGz;
        imagePath = imagePath.substr(firstIndex, secondIndex);
    } else {

        size_t foundCts = imagePath.find(".cts");
        if (foundCts!=string::npos){
            secondIndex = foundCts;
            imagePath = imagePath.substr(firstIndex, secondIndex);
        }
    }

    /// FINDING SLASH in imagePath
    size_t foundSlash;


    char toDelete2 = '/';
    do{
        foundSlash = imagePath.find(toDelete2);
        //cout << "\nSlash foundSlash at: " << foundSlash << " NPOS: "<<string::npos << endl;
        firstIndex = foundSlash;
        imagePath = imagePath.substr(firstIndex+1, secondIndex);
      //  cout << "\nIMAGE PATH SUBSTRING: " << imagePath << endl;
    }
    while (foundSlash!=string::npos);



    return imagePath;
}
