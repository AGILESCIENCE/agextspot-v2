/*//////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       AG_extspot_v2
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it, Giancarlo Zollino
//
// NOTICE
//       Any information contained in this software
//       is property of the AGILE TEAM and is strictly
//       private and confidential.
//       Copyright (C) 2005-2019 AGILE Team. All rights reserved.

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

#include "Blob.h"

Blob::Blob(string _filePath, float _cdelt1, float _cdelt2, float _psf, int _map_rows, int _map_cols, vector<CustomPoint>& _contourPixels, vector<pair<CustomPoint,int> >& _blobPixels, vector<CustomPoint>& _photonsInBlob) : agileMapTool(_filePath.c_str())
{

	filePath = _filePath;
	cdelt1 = _cdelt1;
	cdelt2 = _cdelt2;
	psf = _psf;
	pixelArea = fabs(cdelt1*cdelt2);
	map_rows = _map_rows;
	map_cols = _map_cols;
	//cout << "DEBUG: " << cdelt1 << " " << cdelt2 << " " << psf << " " << map_rows << " " << map_cols << " " << endl;

	contour = _contourPixels;

	contour_size = _contourPixels.size();

	photonsInBlob = _photonsInBlob;

	number_of_photons = photonsInBlob.size();

	blobPixels = _blobPixels;

	numberOfPixels = blobPixels.size();

	blobArea = numberOfPixels*pixelArea;


	centroid = computeCentroid();

	galacticCentroid = computeGalacticCentroid();

	pixelMean = computePixelMean();

	photonsCloseness = computePhotonsCloseness();

	circulary_ratio = computeCircularyRatio();

	rectangularity = computeRectangularity();

	eccentricity = computeEccentricity();

}


CustomPoint Blob::computeCentroid()
{
	int sumX=0;
	int sumY=0;
	for(vector<CustomPoint>::iterator l = contour.begin(); l < contour.end(); l++){
		CustomPoint p = *l;
		sumX+=p.x;
		sumY+=p.y;
	}
	CustomPoint cp(sumY/contour.size(),sumX/contour.size());

	return cp;
}

CustomPoint Blob::computeGalacticCentroid()
{
	// Changing the reference system
	int y_euclidean_ref_sys = agileMapTool.Rows() - getCentroid().y;
	double l  = agileMapTool.l(centroid.x, y_euclidean_ref_sys );
	double b  = agileMapTool.b(centroid.x, y_euclidean_ref_sys );

	CustomPoint cp(l,b);
	return cp;
}


float Blob::computeCircularyRatio()
{
	// Area (pixel^2) / perimetro alla seconda (pixel)
	return blobArea/(contour_size*contour_size);
}

float Blob::computeRectangularity()
{
	return 0;
}

float Blob::computeEccentricity()
{
	return 0;
}

float Blob::computePixelMean()
{
	float numberOfBlobPixels = (float) blobPixels.size();
	float greyLevelCount = 0;

	for (vector<pair<CustomPoint,int> >::iterator it = blobPixels.begin(); it != blobPixels.end(); ++it){
		pair<CustomPoint,int> p= *it;
		greyLevelCount+=p.second;
	}

	//cout << "["<<centroid.y<<","<<centroid.x<<"]" << greyLevelCount << " / " << numberOfBlobPixels << " = "<< greyLevelCount/numberOfBlobPixels << endl;
	return greyLevelCount/numberOfBlobPixels;
}

float Blob::computePhotonsCloseness()
{
	float photonsCloseness = 0;
	float countDistances = 0;
	float countPhotons = photonsInBlob.size();

	for(vector<CustomPoint>::iterator i = photonsInBlob.begin(); i != photonsInBlob.end(); i++){
		CustomPoint photon = *i;
	//countDistances += getDistanceFromCentroid(photon);
		countDistances += getSphericalDistanceFromCentroid(photon);
		countPhotons++;
	}
	// cout << "countDistances: " << countDistances << endl;
	// cout << "countPhotons: " << countPhotons << endl;

	photonsCloseness = countDistances/countPhotons;
	return photonsCloseness;
}

double Blob::getDistanceFromCentroid(CustomPoint photon)
{
	double distance =  0;
	CustomPoint centroid = getCentroid();
	double arg =  pow(photon.x - centroid.x,2) +pow (photon.y - centroid.y,2) ;
	distance = pow(arg , 0.5);
	return distance;
}

double Blob::getSphericalDistanceFromCentroid(CustomPoint photon)
{
	double distance =  0;
	CustomPoint centroid = getCentroid();
	/*
	cout <<"\nfile: "<<agileMapTool.GetFileName()<<endl;
	cout <<"Photon: "<<100-photon.y<<","<< photon.x<<endl;
	cout <<"Photon Gal: "<<agileMapTool.l(photon.x,100-photon.y)<<","<<agileMapTool.b(photon.x,100-photon.y)<<endl;
	cout <<"Centroid: "<<100-centroid.y <<","<<centroid.x <<endl;
	cout <<"Centroid Gal: "<<getGalacticCentroidL()<<","<<getGalacticCentroidB()<<endl;
	*/
	distance = agileMapTool.SrcDist(photon.x,agileMapTool.Rows() - photon.y,getGalacticCentroidL(),getGalacticCentroidB());
	//cout << "distance: " << distance << ""<<endl;
	return distance;
}

bool Blob::isCentered()
{
  int centerY = agileMapTool.Rows()/2;
	int centerX = agileMapTool.Cols()/2;
	int offset = 15;

	if( centroid.x <= centerX+offset && centroid.x >= centerX-offset && centroid.y <= centerY+offset && centroid.y >= centerY-offset ){
	 	return true;
	}else{
		return false;
	}

}



string Blob::toJsonString()
{

	// document is the root of a json message
	rapidjson::Document jsonBlob;

	rapidjson::Document::AllocatorType& allocator = jsonBlob.GetAllocator();

	jsonBlob.SetObject(); // define the document as an object rather than an array


	/*
	"meta" : {
			"file_path" : "/home/rt/baroncelli.leonardo/maps_for_agextspot/FL_200s.cts.gz",
			"map_width_pixels" : 50,
			"map_height_pixels" : 50,
			"cdelt1" : 0.5,
			"cdelt2" : 0.5,
			"psf" : 7.5,
			"blob_contour" : [ "23,12","23,13", "24,14", ..],
			"blob_centroid" :  {"x":46, "y":34},
			"blob_galactic_centroid" : {"l":46, "b":34}
	}
	*/
	rapidjson::Value meta(rapidjson::kObjectType);

	rapidjson::Value f_p(rapidjson::StringRef(filePath.c_str()));
	meta.AddMember("file_path", f_p, allocator);

	rapidjson::Value m_r(map_rows);
	meta.AddMember("map_width_pixels", m_r, allocator);

	rapidjson::Value m_c(map_cols);
	meta.AddMember("map_height_pixels", m_c, allocator);

	rapidjson::Value cd_1(cdelt1);
	meta.AddMember("cdelt1", cd_1, allocator);

	rapidjson::Value cd_2(cdelt2);
	meta.AddMember("cdelt2", cd_2, allocator);

	rapidjson::Value p(psf);
	meta.AddMember("psf", psf, allocator);

	rapidjson::Value pix_centr(rapidjson::kObjectType);
	rapidjson::Value pix_centr_x(centroid.x);
	rapidjson::Value pix_centr_y(centroid.y);
	pix_centr.AddMember("x", pix_centr_x, allocator);
	pix_centr.AddMember("y", pix_centr_y, allocator);
	meta.AddMember("pixel_centroid", pix_centr, allocator);

	rapidjson::Value gal_centr(rapidjson::kObjectType);
	rapidjson::Value gal_centr_l(getGalacticCentroidL());
	rapidjson::Value gal_centr_b(getGalacticCentroidB());
	gal_centr.AddMember("l", gal_centr_l, allocator);
	gal_centr.AddMember("b", gal_centr_b, allocator);
	meta.AddMember("galactic_centroid", gal_centr, allocator);

	/* Contour !!
	rapidjson::Value cont(rapidjson::kArrayType);
	for(vector<CustomPoint>::iterator l = contour.begin(); l < contour.end(); l++){
		CustomPoint p = *l;
		rapidjson::Value cont_point(rapidjson::kObjectType);
		rapidjson::Value cont_point_x(p.x);
		rapidjson::Value cont_point_y(p.y);
		cont_point.AddMember("x", cont_point_x, allocator);
		cont_point.AddMember("y", cont_point_y, allocator);

		cont.PushBack(cont_point, allocator);
	}
	meta.AddMember("contour", cont, allocator);
	*/

	// Adding meta to document
	jsonBlob.AddMember("meta", meta, allocator);



	/*
	"features" : {
			"area_degrees" :  ,
			"pixel_grey_levels_mean" : ,
			"number_of_pixels" : ,
			"number_of_photons" : ,
			"contour_size" : ,
			"photons_closeness" : ,
			"circulary_ratio" : Area (pixel^2) / perimetro alla seconda (pixel)
			"rectangularity" : Area / area rettangolo che contiene la shape
			"eccentricity" : asse maggiore / asse minore (or of the bounding rectangle)
	}
	*/
	rapidjson::Value features(rapidjson::kObjectType);

	rapidjson::Value a_d(blobArea);
	features.AddMember("area_degrees", a_d, allocator);

	rapidjson::Value p_g_l_m(pixelMean);
	features.AddMember("pixel_grey_levels_mean", p_g_l_m, allocator);

	rapidjson::Value n_o_p(numberOfPixels);
	features.AddMember("number_of_pixels", n_o_p, allocator);

	rapidjson::Value n_o_ph(number_of_photons);
	features.AddMember("number_of_photons", n_o_ph, allocator);

	rapidjson::Value c_size(contour_size);
	features.AddMember("contour_size", c_size, allocator);

	rapidjson::Value p_clos(photonsCloseness);
	features.AddMember("photonsCloseness", p_clos, allocator);

	rapidjson::Value circ_r(circulary_ratio);
	features.AddMember("circulary_ratio", circ_r, allocator);


	rapidjson::Value rect(rectangularity);
	features.AddMember("rectangularity", rect, allocator);


	rapidjson::Value ecc(eccentricity);
	features.AddMember("eccentricity", ecc, allocator);



	jsonBlob.AddMember("features", features, allocator);


	// Convert JSON document to string
	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	jsonBlob.Accept(writer);
	return strbuf.GetString();
}


/// GETTERS
string Blob::getFilePath()
{
	return filePath;
}
vector<CustomPoint> Blob::getContour()
{
	return contour;
}
CustomPoint Blob::getCentroid()
{
	return centroid;
}
CustomPoint Blob::getGalacticCentroid()
{
	return galacticCentroid;
}
double Blob::getGalacticCentroidL()
{
	return galacticCentroid.y;
}
double Blob::getGalacticCentroidB()
{
	return galacticCentroid.x;
}
int Blob:: getNumberOfPixels()
{
	return numberOfPixels;
}
int Blob::getNumberOfPhotonsInBlob()
{
	return photonsInBlob.size();
}
vector<CustomPoint> Blob::getPhotonsInBlob()
{
	return photonsInBlob;
}
float Blob::getPixelsMean()
{
	return pixelMean;
}
float Blob::getPhotonsCloseness()
{
	return photonsCloseness;
}
float Blob::getArea()
{
	return blobArea;
}
