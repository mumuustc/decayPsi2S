#!/bin/bash

workDir=$PWD
echo $workDir

mkdir -p log output
rm -rf log/* output/*

echo "Universe     = vanilla">submitJob.con
echo "GetEnv       = True">>submitJob.con
echo "Initialdir   = $workDir">>submitJob.con
echo "Requirements = (OpSysAndVer =?= \"CentOS7\")">>submitJob.con
echo "+JobFlavour  = \"workday\"">>submitJob.con
echo "should_transfer_files = YES">>submitJob.con
echo "transfer_input_files = fdgen">>submitJob.con
echo "">>submitJob.con
echo "">>submitJob.con

ifile=0
for FILE in `cat datalist_all`
do
     echo $FILE

     baseFileName=`basename $FILE`
	 outputFile=${baseFileName%.out}  # remove `.out`

#     echo $outputFile

     echo "Executable       = runDecay.sh">>submitJob.con
#     echo "Arguments        = $workDir $FILE output/$outputFile">>submitJob.con
     echo "Arguments        = $FILE $workDir/output/$outputFile">>submitJob.con
     echo "Output           = log/all_$ifile.out">>submitJob.con
     echo "Error            = log/all_$ifile.err">>submitJob.con
     echo "Log              = log/all_$ifile.olog">>submitJob.con
     echo  "Queue" >>submitJob.con
     echo  "     " >>submitJob.con
      
     let "ifile+=1";
done

condor_submit submitJob.con
