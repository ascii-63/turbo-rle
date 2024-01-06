#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 folder_path"
    exit 1
fi

folder_path="$1"


rm -f $folder_path/*.part?
