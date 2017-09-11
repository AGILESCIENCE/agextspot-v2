#include "MathUtils.h"

MathUtils::MathUtils()
{
    //ctor
}

double MathUtils::sphericalDistanceDeg(double long1, double lat1, double long2, double lat2){

    double l1 = long1*DEG2RAD;
    double l2 = long2*DEG2RAD;
    double b1 = lat1*DEG2RAD;
    double b2 = lat2*DEG2RAD;
    double val = sin(b1)*sin(b2)+cos(b1)*cos(b2)*cos(l1-l2);
    if(val>1.0) return 0.0;
    if(val < -1.0) return 180.0;
    double dist = acos(val);
    return dist*RAD2DEG;
}
