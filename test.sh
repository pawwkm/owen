#!/bin/bash

set -e

gcc compiler/*.c -o owen -std=c99 -D Test
./owen
