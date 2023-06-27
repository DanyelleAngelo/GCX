#!/bin/bash
declare -a files
declare -a rules
declare -a codec
files=("cere" "dna.001.1" "influenza" "proteins.001.1")
rules=(3 8 9 12) 
codecs=("elias" "int") 

for file in "${files[@]}"
do
    for rule in "${rules[@]}"
    do
        make compressor-2 MODE=e RULES_SIZE=$rule FILE=$file
    done
done

for file in "${files[@]}"
do
    for codec in "${codecs[@]}"
    do
        make compressor CODEC=$codec MODE=e RULES_SIZE=8 FILE=$file
    done
done