#!/bin/bash

module load agile-B25-r5
module load opencv-2.4.13.6
#module load healpix-3.31
module load healpix-3.50

if [ "$#" -ne 1 ]; then
  echo "> Please provide the map format: 'agile' or 'healpix'"
  return
fi

echo "Map format: $1"


export PFILES=$PWD/conf

if [ "$1" = "healpix" ]; then
  ./bin/AG_extspot_v2_blobs_list_generator blobs json_blobs_data /home/zollino/AGILE_REAL_IMAGE healpix 0.5 0.5 7.5 no yes
  # ./bin/AG_extspot_v2_blobs_list_generator blobs json_blobs_data /home/zollino/AGILE_REAL_IMAGE healpix 0.25 0.25 7.5 no yes
fi
if [ "$1" = "agile" ]; then
  ./bin/AG_extspot_v2_blobs_list_generator agile_real_grb agextspot_output test_images/agile_real agile 0.5 0.5 7.5 no yes
fi
