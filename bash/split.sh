#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

input_file="$1"


split -a 1 -d -n 3 "$input_file" "$input_file.part"