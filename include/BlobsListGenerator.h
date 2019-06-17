#ifndef BLOBSLISTGENERATOR_H
#define BLOBSLISTGENERATOR_H

#include <string>

#include "AgileCountMapsBlobsFinder.h"
#include "HealPixCountMapsBlobsFinder.h"
#include "FolderManager.h"

using std::string;


class BlobsListGenerator {

  public:

    BlobsListGenerator(string counts_map_folder_path, bool healpix, float cdelt1, float cdelt2, float psf);

    void generate(bool shuffle_dataset);

  private:
    string counts_map_folder_path;
    bool healpix;
    float cdelt1;
    float cdelt2;
    float psf;

    BlobsFinder * blobs_finder;
};



#endif
