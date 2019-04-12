#!/bin/sh
module load agile-B25-r5
module load opencv-2.4.13.6

export PFILES=$PWD/conf

#make clean
#make agextspot


./bin/AG_extspot_v2_blobs_list_generator /home/rt/baroncelli.leonardo/maps_for_agextspot no 0.5 0.5 7.5 yes
