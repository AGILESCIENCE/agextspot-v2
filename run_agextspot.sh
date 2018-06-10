#!/bin/sh
module load agile-B24-r5
export PFILES=$PWD/conf

make clean
make agextspot

#./bin/AG_extspot log.txt ../cntcubeDifficile02112017CToolToAgile.fits 95 SCAN1000_430651900.0_430652900.0_308.377257_39.293747.exp.gz false
#./bin/AG_extspot-v2 log.txt mapsParmi/SCAN1000_188711134.0_188712134.0_308.377257_39.293747.cts.gz 95 mapsParmi/SCAN1000_188711134.0_188712134.0_308.377257_39.293747.exp.gz
#./bin/AG_extspot-v2 7.5 log.txt maps/SCAN1000_430651900.0_430652900.0_308.377257_39.293747.cts.gz 95 maps/SCAN1000_430651900.0_430652900.0_308.377257_39.293747.exp.gz
#./bin/AG_extspot-v2 log.txt RealImages/090401B.cts
#./bin/AG_extspot-v2 logParmiggianiMap ../SCAN20_411587354.152_411587374.152_292.5_0.cts.gz

## Test on Real Images 080514B.cts  090401B.cts  090510.cts  090510x.cts  100724B.cts
#./bin/AG_extspot-v2 log080514B.txt 7.5 RealImages/080514B.cts 50 no None
#./bin/AG_extspot-v2 log09041B.txt 7.5 RealImages/090401B.cts 50 no None
#./bin/AG_extspot-v2 log090510.txt 7.5 RealImages/090510.cts 50 no None
#./bin/AG_extspot-v2 log090510x.txt 7.5 RealImages/090510x.cts 50 no None

./bin/AG_extspot-v2 LOGS/extspotlogs/log100724B.txt 7.5 MAPS/RealImages/100724B.cts 50 yes None
