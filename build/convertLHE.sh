#!/bin/bash

if [ ! -d lheFiles ]; then
    mkdir -p lheFiles
fi

if [ "`ls -A lheFiles`" != "" ]; then
    rm -rf lheFiles/*
fi

cmsEnergyDiv2=2510

for inputFile in `ls output/slight*.tx`
do
    echo $inputFile

    baseFileName=`basename $inputFile`

    outputFile=${baseFileName%.tx}  # remove `.tx`

    echo $outputFile

    root -l -b << EOF
    .x convert_SL2LHE.C+("$inputFile","lheFiles/$outputFile",$cmsEnergyDiv2,$cmsEnergyDiv2)
    .q
EOF

done
