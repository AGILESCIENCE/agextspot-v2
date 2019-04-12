/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 *
*/
#include <iostream>
#include <stdlib.h>
#include <PilParams.h>

#include "BlobsListGenerator.h"

using std::cout;
using std::endl;

const char* startString = {
"################################################################\n"
"###  Task AG_extspot_v2_generate_blobs_list..................###"
};

const char* endString = {
"################################################################\n"
"###  Task AG_extspot_v2_generate_blobs_list..................###"
};

int main(int argc, char*argv[]){

	cout << startString << endl;

	const PilDescription paramsDescr[] = {
		{ PilString, "counts_map_folder_path","The path to the folder that contains the dataset."},
		{ PilBool,   "healpix","If 'yes' (or 'y') it is asserted that the map is in HealPix format. Insert 'no' (or 'n') if the maps are in AGILE counts map format"},
		{ PilReal,   "cdelt1", "Degree pixel size (width)"},
		{ PilReal,   "cdelt2", "Degree pixel size (height)"},
		{ PilReal, 	 "psf", "The telescope's spread point function (degree)"},
		{ PilBool,   "shuffle_dataset","If 'yes' (or 'y') the filenames will be shuffled. Insert 'no' (or 'n') instead"},
		{ PilNone, 	 "", "" }
	};

	PilParams params(paramsDescr);

  if (!params.Load(argc, argv))
      return EXIT_FAILURE;

	const char * counts_map_folder_path	= params["counts_map_folder_path"];
	bool healpix  = params["healpix"];
	double cdelt1 = params["cdelt1"];
	double cdelt2 = params["cdelt2"];
	double psf 		= params["psf"];
  bool shuffle_dataset = params["shuffle_dataset"];

	params.Print();

	int kernelSize = (2 * psf/cdelt2) + 1;
	cout << "Smoothing with:\n * kernel size: ["<<kernelSize<<"x"<<kernelSize<<"] (formula: 2 * psf/cdelt2 + 1)\n * sigma (psf): "<<psf<<endl;

  BlobsListGenerator blg(counts_map_folder_path, healpix, cdelt1, cdelt2, psf);

  blg.generate(shuffle_dataset);

	cout << endString << endl;

	return 0;
}
