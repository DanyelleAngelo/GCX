#!/bin/bash

YELLOW='\033[33m'
GREEN='\033[0;32m'
BLUE='\033[34m'
RED='\033[31m'
RESET='\033[0m'


build_gc_star() {
  echo "$(YELLOW)Compiling GC*...$(RESET)"
  make compile -C gc_/ OUTPUT="$(pwd)/gc_star"
  make clean -C gc_/ OUTPUT="$(pwd)/gc_star"
  if [ $? -ne 0 ]; then
    echo "$(RED)Error compiling GC* $(RESET)"
    exit 1
  fi
  echo "$(GREEN)GC* compiled successfully as gc_star.$(RESET)"
}

build_gcx() {
  echo "$(YELLOW)Compiling GCX...$(RESET)"
  make compile -C gcx/ OUTPUT="$(pwd)/gcx"
  make clean -C gcx/ OUTPUT="$(pwd)/gcx"
  if [ $? -ne 0 ]; then
    echo "$(RED)Error compiling GCX $(RESET)"
    exit 1
  fi
  echo "$(GREEN) GCX compiled successfully as gcx. $(RESET)"
}

if [ "$1" == "gc_star" ]; then
  build_gc_star
elif [ "$1" == "gcx" ]; then
  build_gcx
elif [ "$1" == "all" ]; then
  build_gc_star
  build_gcx
else
  echo "Usage: $0 {gc_star|gcx|all}"
  exit 1
fi
