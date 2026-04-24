#!/usr/bin/env bash
datasets=(Gowalla)
sample=1000
ratio=0.2
for dataset in "${datasets[@]}"; do
      echo "Generating seeds for dataset: ${dataset} with sample number: ${sample} for all Items and ${ratio} for popular Items"
      python data/genseed.py --dataset=${dataset} --sample=${sample} --ratio=${ratio}
done