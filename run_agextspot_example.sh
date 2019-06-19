#!/bin/sh
module load agile-B25-r5
module load opencv-2.4.13.6

export PFILES=$PWD/conf

#make clean
#make agextspot

./bin/AG_extspot-v2 logs/log 7.5 /home/rt/baroncelli.leonardo/agextspot-v2/real_images/F090510x.cts 50 no None
