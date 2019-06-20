#include "BlobsListGenerator.h"

BlobsListGenerator::BlobsListGenerator(string map_format, float cdelt1, float cdelt2, float psf)
{

  if ( map_format == "agile" )
  {
    cout << "[BlobsListGenerator] "<< map_format <<" map format selected." <<endl;
    blobs_finder = new AgileCountMapsBlobsFinder(cdelt1, cdelt2, psf);
  }
  else if ( map_format == "healpix")
  {
    cout << "[BlobsListGenerator] "<< map_format <<" map format selected." <<endl;
    blobs_finder = new HealPixCountMapsBlobsFinder(cdelt1, cdelt2, psf);
  }
  else
  {
    cout << "[BlobsListGenerator] Map format "<< map_format <<" is not yet supported." <<endl;
    blobs_finder = NULL;
  }
}

/*
  It writes a Json list [{}, {}, ..]
*/
void BlobsListGenerator::generate(string counts_map_folder_path, string output_filepath, bool shuffle_dataset)
{

  if(blobs_finder == NULL)
  {
    cout << "[BlobsListGenerator] No blobs-finder exists. Quitting." <<endl;
    exit(EXIT_FAILURE);
  }

  if( FileWriter::is_empty(output_filepath) )
  {
    FileWriter::write2File(output_filepath, "[\n", false);
  }

	vector<string> filenames = FolderManager::getFileNamesFromFolder(counts_map_folder_path);

  for(vector<string>::iterator filename_it = filenames.begin() ; filename_it < filenames.end(); filename_it++){

		string fits_file_path = counts_map_folder_path + "/" + (*filename_it);

    cout << "[BlobsListGenerator] Extracting blobs from: "<< fits_file_path << endl;

    cout << "[BlobsListGenerator] Format: " << blobs_finder->get_format() << endl;

    vector<Blob*> blobs = blobs_finder->find_blobs(fits_file_path, false);

    cout << "Format: " << blobs_finder->get_format() << endl;

    cout << "\n\n ==> QUITTING FOR DEBUGGING PURPOSES" << endl;
    exit(0);

    for(vector<Blob *>::iterator blob_it = blobs.begin() ; blob_it < blobs.end(); blob_it++){

      (*blob_it)->build_json_encoding(fits_file_path);

      string json_string = (*blob_it)->to_json_str();

      cout << json_string << endl;

      FileWriter::write2File(output_filepath, json_string+",\n", true);

    }

  }

  FileWriter::write2File(output_filepath, "]", true);
}
