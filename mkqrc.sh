#!/bin/bash

QRCFILE=zandemo.qrc
FILES=$(echo ./icons/*.* LICENSE)

printf "" > $QRCFILE

printf "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n<qresource>\n" >> $QRCFILE

for f in $FILES; do
	printf "\t<file>$f</file>\n" >> $QRCFILE
done

printf "</qresource>\n</RCC>\n" >> $QRCFILE
