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
#include <chrono>

#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>

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
		{ PilString, "output_filename", "The name of the output file."},
		{ PilString, "output_folder", "The name of the output folder."},
		{ PilString, "counts_map_folder_path", "The path to the folder that contains the dataset."},
		{ PilString, "map_format", "'agile','healpix'"},
		{ PilReal, "cdelt1", "Degree pixel size (width)"},
		{ PilReal, "cdelt2", "Degree pixel size (height)"},
		{ PilReal, "psf", "The telescope's spread point function (degree)"},
		{ PilBool, "shuffle_dataset","If 'yes' (or 'y') the filenames will be shuffled. Insert 'no' (or 'n') instead"},
		{ PilBool, "save_cv_steps","If 'yes' (or 'y') the convolved, thresholded, labelled and contoured maps are saved on disk. Insert 'no' (or 'n') instead"},
		{ PilNone, "", "" }
	};

	PilParams params(paramsDescr);

  if (!params.Load(argc, argv))
      return EXIT_FAILURE;

	string output_filename  = string(params["output_filename"]);
	string output_folder  = string(params["output_folder"]);
	string counts_map_folder_path	= string(params["counts_map_folder_path"]);
	string map_format  = string(params["map_format"]);
	double cdelt1 = params["cdelt1"];
	double cdelt2 = params["cdelt2"];
	double psf 		= params["psf"];
  bool shuffle_dataset = params["shuffle_dataset"];
	bool save_cv_steps = params["save_cv_steps"];

	params.Print();

	// Creating the INDEX directory
	if (mkdir(output_folder.c_str(), 0777) != -1)
		cout << "* Output folder "<<output_folder<<" created" << endl;


	int kernelSize = (2 * psf/cdelt2) + 1;
	cout << "* Smoothing with:\n * kernel size: ["<<kernelSize<<"x"<<kernelSize<<"] (formula: 2 * psf/cdelt2 + 1)\n * sigma (psf): "<<psf<<endl;

	auto start = std::chrono::system_clock::now();

  BlobsListGenerator blg(map_format, cdelt1, cdelt2, psf);

  blg.generate(counts_map_folder_path, output_filename, output_folder, shuffle_dataset, save_cv_steps);

	auto stop = std::chrono::system_clock::now();

  std::chrono::duration<double> diff;

  diff = stop-start;
	cout << "Execution time: " << diff.count() << " s" << endl;

	cout << endString << endl;

	return 0;
}
