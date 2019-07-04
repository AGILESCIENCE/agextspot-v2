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

#ifndef BLOBS_FINDER_H
#define BLOBS_FINDER_H

#include <string>

#include "Blob.h"

using std::string;

class BlobsFinder {

  public:

    BlobsFinder(float _cdelt1, float _cdelt2, float _psf): cdelt1(_cdelt1), cdelt2(_cdelt2), psf(_psf) {}

    virtual vector<Blob *> find_blobs(string fitsfilename, string fitsfile_folder, bool save_cv_steps, string output_folder) = 0;

    virtual string get_format() = 0;

  protected:

    float cdelt1;
    float cdelt2;
    float psf;
    string file_format;

};


#endif
