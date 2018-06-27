#!/bin/sh
module load agile-B25-r5
export PFILES=$PWD/conf

make clean
make agextspot


./bin/AG_extspot-v2 logs/LOG 7.5 /home/baroncelli/AG_extspot-v2-TESTING/TestSetRealImages/F100724B.cts 50 yes None
