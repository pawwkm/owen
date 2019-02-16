#!/bin/bash

set -e

gcc compiler/*.c -o owen -std=c99 -g
./owen
