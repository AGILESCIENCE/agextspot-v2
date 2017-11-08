/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "Blob.h"

Blob::Blob(vector<CustomPoint>& _contourPixels, vector<pair<CustomPoint,int>>& _blobPixels, double ** image, int ** photonImage, double CDELT1, double CDELT2)
{
    
	pixelArea = CDELT1*CDELT2;

	contour = _contourPixels;

    centroid = computeCentroid();

    blobPixels = _blobPixels;

    numberOfPixels = blobPixels.size();

	blobArea = numberOfPixels*pixelArea;

    pixelMean = computePixelMean();

    photonsInBlob = computePhotonsBlob(photonImage);

    photonsCloseness = computePhotonsCloseness();
	


}



/// GETTERS
vector<CustomPoint> Blob::getContour(){
    return contour;
}
CustomPoint Blob::getCentroid(){
    return centroid;
}

int Blob:: getNumberOfPixels() {
    return numberOfPixels;
}
int Blob::getNumberOfPhotonsInBlob() {
    return photonsInBlob.size();
}
double Blob::getPixelsMean(){
    return pixelMean;
}
double Blob::getPhotonsCloseness(){
    return photonsCloseness;
}
double Blob::getArea(){
	return blobArea;
}

CustomPoint Blob::computeCentroid(){
    int sumX=0;
    int sumY=0;
    for(vector<CustomPoint>::iterator l = contour.begin(); l < contour.end(); l++){
        CustomPoint p = *l;
        sumX+=p.x;
        sumY+=p.y;
    }
    CustomPoint c;
	c.x = sumX/contour.size();
	c.y = sumY/contour.size();

    return c;
}


vector<pair<CustomPoint,int>> Blob::computePhotonsBlob(int ** photonImage){
    vector<pair<CustomPoint,int>> photonPixels;
    for(vector<pair<CustomPoint,int>>::iterator i = blobPixels.begin(); i != blobPixels.end(); i++){
        pair<CustomPoint,int> p = *i;

        int greyLevel = photonImage[p.first.y][p.first.x];
	//cout << photonImage[p.first.y][p.first.x] << " ";
	for(int j = 0; j < greyLevel; j++){
		//cout << p.first.y << " " << p.first.x <<endl;
		photonPixels.push_back(p);
	}        
 
    }
    return photonPixels;
}



double Blob::computePixelMean(){
    double numberOfBlobPixels = (double)blobPixels.size();
    double greyLevelCount = 0;

    for (vector<pair<CustomPoint,int>>::iterator it = blobPixels.begin(); it != blobPixels.end(); ++it){
        pair<CustomPoint,int> p= *it;
        greyLevelCount+=p.second;
    }
    return greyLevelCount/numberOfBlobPixels;
}

double Blob::computePhotonsCloseness(){
    double photonsCloseness = 0;
    double countDistances = 0;
    double countPhotons = 0;

	
    for(vector<pair<CustomPoint,int>>::iterator i = photonsInBlob.begin(); i != photonsInBlob.end(); i++){
        pair<CustomPoint,int> photon = *i;
        countDistances += getDistanceFromCentroid(photon.first);
        countPhotons++;
    }
    // cout << "countDistances: " << countDistances << endl;
    // cout << "countPhotons: " << countPhotons << endl;
    if(countPhotons==0)
        return 0;
    photonsCloseness = countDistances/countPhotons;
    return photonsCloseness;
}

double Blob::getDistanceFromCentroid(CustomPoint photon) {
    double distance =  0;
    CustomPoint centroid = getCentroid();
    double arg =  pow(photon.x - centroid.x,2) +pow (photon.y - centroid.y,2) ;
    distance = pow(arg , 0.5);
    return distance;
}

bool Blob::isCentered(){
    if( centroid.x <= 55 && centroid.x >= 45 && centroid.y <= 55 && centroid.y >= 45 ){
          return true;
    }else{
          return false;
    }

}
