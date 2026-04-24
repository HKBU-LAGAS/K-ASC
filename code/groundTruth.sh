#!/usr/bin/env bash
datasets=(Gowalla)
seeds=(all pop)
algo=Truth
savefile="results/Truth"
for seed in "${seeds[@]}"; do
  for dataset in "${datasets[@]}"; do
        echo "Running: ./main -f data -g ${dataset} -a ${algo} -r ${seed} -s ${savefile}"
        ./main -f data -g "${dataset}" -a ${algo} -r ${seed} -s ${savefile}
    done
done
