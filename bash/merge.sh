#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <input_folder> <output_file>"
    exit 1
fi

input_folder="$1"
output_file="$2"

cat $input_folder* > "$output_file"
