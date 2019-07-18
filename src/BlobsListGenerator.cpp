#include "BlobsListGenerator.h"

BlobsListGenerator::BlobsListGenerator(string map_format, float cdelt1, float cdelt2, float psf, double classification_threshold)
{

  if ( map_format == "agile" )
  {
    cout << "[BlobsListGenerator] "<< map_format <<" map format selected." <<endl;
    bool interactive_extraction = false;
    blobs_finder = new AgileCountMapsBlobsFinder(cdelt1, cdelt2, psf, classification_threshold, interactive_extraction);
  }
  else if ( map_format == "healpix")
  {
    cout << "[BlobsListGenerator] "<< map_format <<" map format selected." <<endl;
    blobs_finder = new HealPixCountMapsBlobsFinder(cdelt1, cdelt2, psf, classification_threshold);
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

  if(blobs_finder == NULL)
  {
    cout << "[BlobsListGenerator] No blobs-finder exists. Quitting." <<endl;
    exit(EXIT_FAILURE);
  }

  cout << "[BlobsListGenerator] Format: " << blobs_finder->get_format() << endl;


  string json_output_filepath = output_folder+"/"+output_filename+".json";
  cout << "[BlobsListGenerator] Output JSON file: " << json_output_filepath << endl;

  // if( FileWriter::file_exists(json_output_filepath) )
  FileWriter::write2File(json_output_filepath, "[\n", false);




	vector<string> filenames = FolderManager::getFileNamesFromFolder(counts_map_folder_path);

  // opening json list

  for(vector<string>::iterator filename_it = filenames.begin() ; filename_it < filenames.end(); filename_it++){

    string filename = *filename_it;
		string fits_file_path = counts_map_folder_path + "/" + filename;

    cout << "[BlobsListGenerator] Extracting blobs from: "<< fits_file_path << endl;

    vector<Blob*> blobs = blobs_finder->find_blobs(filename, counts_map_folder_path, save_cv_steps, output_folder);



    for(vector<Blob *>::iterator blob_it = blobs.begin() ; blob_it < blobs.end(); blob_it++){

      string json_string = (*blob_it)->to_json_str(fits_file_path);

      // cout << json_string << endl;

      FileWriter::write2File(json_output_filepath, json_string+",\n", true);

    }
  }

  // closing json list
  FileWriter::write2File(json_output_filepath, "]", true);


}
