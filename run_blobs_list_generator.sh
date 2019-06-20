#!/bin/sh
module load agile-B25-r5
module load opencv-2.4.13.6
module load healpix-3.31

export PFILES=$PWD/conf

#make clean
#make agextspot


./bin/AG_extspot_v2_blobs_list_generator /home/rt/baroncelli.leonardo/maps_for_agextspot agile 0.5 0.5 7.5 yes agile_0.5_0.5_7.5.json
