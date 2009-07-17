#!/bin/bash

rm -f files

for i in $COLLECTION/*/*/*.{mp3,ogg,flac}; do echo $i | sed s#$COLLECTION/## >> files; done;

./create-db.pl > default.db

rm files
