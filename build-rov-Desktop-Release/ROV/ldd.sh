#! /bin/bash
EXE='ROVComm' 
PWD=`pwd`
files=`ldd $EXE | awk '{ if(match($3,"^/"))printf("%s "),$3 }'`
cp $files $PWD
