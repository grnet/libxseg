#!/bin/sh 
CUR_SOURCE_DIR=$1
CUR_BINARY_DIR=$2

echo "AAAAA"
cd $CUR_SOURCE_DIR
mkdir -p $CUR_BINARY_DIR/xseg
if [ -f xseg/version.py ] ; then
	cp xseg/version.py $CUR_BINARY_DIR/xseg/version.py ;
else
	echo '__version__ = "'`devflow-version python`'"' > $CUR_BINARY_DIR/xseg/version.py ;
fi
cd $CUR_BINARY_DIR
