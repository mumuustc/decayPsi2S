#!/bin/bash

source /afs/cern.ch/user/s/shuaiy/setup_root6.sh

echo $PWD

#workDir=$1
#inFile=$2
#outFile=$3

#cd $workDir

inFile=$1
outFile=$2

./fdgen $inFile $outFile
