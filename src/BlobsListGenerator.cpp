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
void BlobsListGenerator::generate(string counts_map_folder_path, string output_filename, string output_folder, bool shuffle_dataset, bool save_cv_steps)
{



  string output_filepath = output_folder+"/"+output_filename;
  cout << "[BlobsListGenerator] Output file: " << output_filepath << endl;

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

    string filename = *filename_it;
		string fits_file_path = counts_map_folder_path + "/" + filename;

    cout << "[BlobsListGenerator] Extracting blobs from: "<< fits_file_path << endl;

    cout << "[BlobsListGenerator] Format: " << blobs_finder->get_format() << endl;

    vector<Blob*> blobs = blobs_finder->find_blobs(filename, counts_map_folder_path, false, save_cv_steps, output_folder);

    cout << "Format: " << blobs_finder->get_format() << endl;


    for(vector<Blob *>::iterator blob_it = blobs.begin() ; blob_it < blobs.end(); blob_it++){

      string json_string = (*blob_it)->to_json_str(fits_file_path);

      // cout << json_string << endl;

      FileWriter::write2File(output_filepath, json_string+",\n", true);

    }



  }
  FileWriter::write2File(output_filepath, "]", true);
  cout << "GENERATE is TERMINATED! " << endl;
}
