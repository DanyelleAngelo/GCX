#!/bin/bash

source measure_compression.sh

report_execution=$REPORT_DIR/log-$CURR_DATE.txt

check_and_create_folder | tee -a "$report_execution"
download_files | tee -a "$report_execution"
compress_and_decompress_with_dcx | tee -a "$report_execution"
valid_dcx_extract | tee -a "$report_execution"
run_extract | tee -a "$report_execution"

git add ../report/*
git commit -m "reports and logs"
git push
