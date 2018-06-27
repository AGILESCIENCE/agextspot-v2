#!/bin/sh
module load agile-B25-r5

make clean
make bayesian

./bin/AG_extspot-v2-bayesian-model-evaluator TRAINING/TrainingSetShortTerm 0.5 0.5 7.5
