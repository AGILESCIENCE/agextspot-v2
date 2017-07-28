#include "GammaRayDetector.h"

GammaRayDetector::GammaRayDetector(string _imagePath, string _outputLogName){
    imagePath = _imagePath;
    outputLogName = _outputLogName;
	reverendBayes = new BayesianClassifierForBlobs(false);
	//agileMapUtils = new AgileMap(imagePath.c_str());

 }



void GammaRayDetector::trainBayesianClassifier(string trainingSetPath){
    reverendBayes->learnFromTrainingSet(trainingSetPath);
}






void GammaRayDetector::detect()
{
    FileWriter::write2FileHeader(imagePath, outputLogName);
    cout<<"\n\nAnalysis of: " << imagePath << endl;

    /// 1 - CONVERTING FITS TO MAT
	Mat tempImage = FitsToCvMatConverter::convertFitsToCvMat(imagePath);


    /// 2 - FINDING BLOBS -> stretching,gaussian filtering
    vector<Blob*> blobs = BlobsFinder::findBlobs(tempImage,false);


    /// 3 - SELECT MOST PROBABLE BLOB TO BE A FLUX
    classifyBlobs(blobs);


}



void GammaRayDetector::classifyBlobs(vector<Blob*> blobs){

    if(blobs.size() > 0){


        for(vector<Blob*>::iterator i = blobs.begin(); i != blobs.end(); i++){

            Blob* b = *i;

            vector<pair<string,float> > predicted = reverendBayes->classify(b);

            float bgProbability = predicted[0].second;
            float fluxProbability = predicted[1].second;

            //double gaLong=agileMapUtils->l(b->getCentroid().x,b->getCentroid().y);
            //double gaLat=agileMapUtils->b(b->getCentroid().x,b->getCentroid().y);


            string information2Print = "Blob in: [" + to_string(b->getCentroid().x) +"," +to_string(b->getCentroid().y)   +"]"; //+"["+to_string(gaLong) +","+to_string(gaLat)+"] P[Blob=bg]: " + to_string(bgProbability*100) +"%" + " P[Blob=flux]: " + to_string(fluxProbability*100) +"%";
            cout << information2Print << endl;
            FileWriter::write2FileBody(information2Print, outputLogName);


        } // end for loop



    }else{
        cout << "No flux has been found." << endl;
        FileWriter::write2FileBody("No flux has been found!", outputLogName);

    }
 }

