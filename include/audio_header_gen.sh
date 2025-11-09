#!/bin/bash

for f in $(find $(dirname "$0") -name "*.wav"); do
    echo "$f"
    python $(dirname "$0")/../wav_to_h/converter.py $f
done