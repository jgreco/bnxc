#!/bin/bash

rm -f tmp

for i in $COLLECTION/*/*/*.{mp3,ogg,flac}; do echo $i | sed s#$COLLECTION/## >> tmp; done;

./create-db.pl > default.db

rm tmp
