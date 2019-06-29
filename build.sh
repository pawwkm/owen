#!/bin/bash

set -e

clear

gcc compiler/*.c -o owen -std=c99 -g

sloc=$(cloc compiler/ | awk '/[0-9]+/ {print $NF}' | tail -n 1)
characters=$(lynx --dump -nolist -notitle site/specification.html | sed '1d' | tr '\n' ' ' | sed "s/ //g"| wc -c)
pages=$(echo "scale=2; $characters / 2200" | bc -l)

echo "The compiler sloc is $sloc out of 6000."
echo "The specification is $pages out of 20 pages."
