#!/bin/bash

for i in `find ./ -type f`
do 
	for j in "$@" 
	do
		cat "$i" | grep "$j" > /dev/null
		if [ $? -eq 0 ] ; then
			echo "mot trouve " $j " dans fichier " $i
		fi
	done
done

