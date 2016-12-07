#!/usr/bin/env sh
set -e

#24 cal
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libstdc++.so.6 /home/xileli/Documents/library/caffe/build/tools/caffe train --solver=/home/xileli/Documents/program/CascadeCNN/Models/24cal/solver.prototxt 


