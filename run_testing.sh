#!/bin/sh
module load agile-B24-r5

make clean
make performance


#./bin/AG_extspot-v2-performance-evaluator TESTING/testSet 30 0.5 0.5 0.5 7.5
#./bin/AG_extspot-v2-performance-evaluator TESTING/MAPPE_CON_ERRORE 80 0.5 0.5 0.5 7.5
./bin/AG_extspot-v2-performance-evaluator TESTING/TestSetShortTerm 0 100 10 0.5 0.5 7.5
