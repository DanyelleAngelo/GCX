#!/bin/bash

source measure_compression.sh

check_and_create_folder
download_files
compress_and_decompress_with_dcx
run_extract

git add ../report/$CURR_DATE
git commit -m "reports and logs"
git push
