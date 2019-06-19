#ifndef BLOBSLISTGENERATOR_H
#define BLOBSLISTGENERATOR_H

#include <string>

#include "AgileCountMapsBlobsFinder.h"
#include "HealPixCountMapsBlobsFinder.h"
#include "FolderManager.h"
#include "FileWriter.h"

using std::string;


class BlobsListGenerator
{

  public:

    BlobsListGenerator( string map_format, float cdelt1, float cdelt2, float psf);

    void generate(string counts_map_folder_path, string output_filepath, bool shuffle_dataset);

  private:

    BlobsFinder * blobs_finder;
};



#endif
