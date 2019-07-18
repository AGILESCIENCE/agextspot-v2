////////////////////////////////////////////////////////////////////////////////
// DESCRIPTION
//       AGILE Science Tools
//       agextspot-v2
//       First release: 2017
//       Authors: Leonardo Baroncelli leonardo.baroncelli@inaf.it,
//                Giancarlo Zollino giancarlo.zollino@gmail.com
//
// NOTICE
//       Any information contained in this software
//       is property of the AGILE TEAM and is strictly
//       private and confidential.
//       Copyright (C) 2005-2019 AGILE Team. All rights reserved.
/*
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

    BlobsListGenerator(string map_format, float cdelt1, float cdelt2, float psf, double classification_threshold);

    void generate(string counts_map_folder_path, string output_filename, string output_folder, bool shuffle_dataset, bool save_cv_steps);

  private:

    BlobsFinder * blobs_finder;
};



#endif
