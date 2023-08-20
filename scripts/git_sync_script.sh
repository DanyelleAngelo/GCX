#!/bin/bash

source measure_compression.sh

report_execution=$REPORT_DIR/log-$formatted_date.txt

check_and_create_folder | tee -a "$report_execution"
download_files | tee -a "$report_execution"
compress_decompress_and_generate_report | tee -a "$report_execution"

git add ../report/*
git commit -m "reports and logs"
git push