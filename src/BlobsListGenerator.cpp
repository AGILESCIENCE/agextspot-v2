#include "BlobsListGenerator.h"



BlobsListGenerator::BlobsListGenerator(string _counts_map_folder_path, bool _healpix, float _cdelt1, float _cdelt2, float _psf){
  counts_map_folder_path = _counts_map_folder_path;
  healpix = _healpix;
  cdelt1 = _cdelt1;
  cdelt2 = _cdelt2;
  psf = _psf;

  if( healpix == true ){

    blobs_finder = new HealPixCountMapsBlobsFinder(cdelt1, cdelt2, psf);

  }else{

    blobs_finder = new AgileCountMapsBlobsFinder(cdelt1, cdelt2, psf);

  }

}

void BlobsListGenerator::generate(bool shuffle_dataset) {

	// Vector that contains all the file names of the training set background images.
	vector<string> filenames = FolderManager::getFileNamesFromFolder(counts_map_folder_path);

  for(vector<string>::iterator filename_it = filenames.begin() ; filename_it < filenames.end(); filename_it++){

		string fits_file_path = counts_map_folder_path + "/" + (*filename_it);

    cout << "Extracting blobs from: "<< fits_file_path << endl;

    cout << "Format: " << blobs_finder->get_format() << endl;
    vector<Blob*> blobs = blobs_finder->findBlobs(fits_file_path, false);
    exit(0);

    for(vector<Blob *>::iterator blob_it = blobs.begin() ; blob_it < blobs.end(); blob_it++){

      string json_string = (*blob_it)->toJsonString();

      cout << json_string << endl;
      // fw->write2file(json_blob);
    }

  }

}
