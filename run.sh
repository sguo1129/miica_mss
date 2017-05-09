#!/bin/bash

# The script needs 3 input cariables, the first one
# is first processing scene_name name, the senod one is second processing name, 
# the third one is output name. The input and final output files are in the 
# current data directory, 
# e.g. ./run.sh LM50450301985132AAA04 LM50450301986135AAA03 LM5045030_1985_1986

./parseMTL $1"_MTL.txt"
./parseMTL $2"_MTL.txt"
./calcTOA -sc $1"_B1_info.txt"
./calcTOA -sc $1"_B2_info.txt"
./calcTOA -sc $1"_B3_info.txt"
./calcTOA -sc $1"_B4_info.txt"
./calcTOA -sc $2"_B1_info.txt"
./calcTOA -sc $2"_B2_info.txt"
./calcTOA -sc $2"_B3_info.txt"
./calcTOA -sc $2"_B4_info.txt"
# rm *_info.txt
./calcMIICA_noMask_MSS $1 $2 $3 
