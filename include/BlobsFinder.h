#include <string>

#include "Blob.h"

using std::string;

class BlobsFinder {

  public:

    BlobsFinder(float _cdelt1, float _cdelt2, float _psf): cdelt1(_cdelt1), cdelt2(_cdelt2), psf(_psf) {}

    virtual vector<Blob *> findBlobs(string fitsfilePath, bool debug) = 0;

    virtual string get_format() = 0;

  protected:

    float cdelt1;
    float cdelt2;
    float psf;
    string file_format;

};
