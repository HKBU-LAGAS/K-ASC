import random
import sys,os
import numpy as np
import argparse
import matplotlib.pyplot as plt

def read_stat_file(folder, graph):
    stat_file = f"{folder}/{graph}/stat.txt"
    m_nu = m_nv = m_m = None

    try:
        with open(stat_file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line.startswith("u="):
                    m_nu = int(line[2:])
                elif line.startswith("v="):
                    m_nv = int(line[2:])
                elif line.startswith("m="):
                    m_m = int(line[2:])
    except FileNotFoundError:
        raise RuntimeError("Fail to open attribute file!")

    return m_nu, m_nv, m_m
def top_hot_items_ratio(folder, dataset, num_item, ratio):
    graph_file = f"{folder}/{dataset}/graph.txt.new"
    degree = np.zeros(num_item, dtype=np.int32)
    with open(graph_file, 'r') as f:
        for line in f:
            toks = line.strip().split()
            if len(toks) == 2:
                y = int(toks[1])
                if 0 <= y < num_item:
                    degree[y] += 1
                else :
                    raise ValueError(f"Item ID {y} out of range [0, {num_item})")
            else:
                raise ValueError(f"Invalid line format: {line.strip()}")

    # degree降序排列，得到所有item列表
    items = list(enumerate(degree))
    sorted_items = sorted(items, key=lambda x: x[1], reverse=True)
    # 只拿前 topK
    topK = int(num_item * ratio)
    topK = max(1, min(topK, len(sorted_items)))
    hot_items = [item for item, deg in sorted_items[:topK]]
    return hot_items
def query_gen(args, qfoldder, num_item, popItem):
    dataset = args.dataset
    num_sample = args.sample
    pop_sample = args.sample * args.ratio
    file_name = f"{qfoldder}/seeds-all.txt"
    popfile_name = f"{qfoldder}/seeds-pop.txt"
    all_items = list(range(num_item))

    with open(file_name, "w") as f:
        for i in range(num_sample):
            x = random.choice(all_items)
            f.write(str(x)+"\n")

    with open(popfile_name, "w") as f:
        for i in range(num_sample):
            x = random.choice(popItem)
            f.write(str(x)+"\n")

if __name__=='__main__':
    random.seed(202510)
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--dataset', required=True, help='dataset name')
    parser.add_argument('--sample', type=int, default=1000, help='number of samples')
    parser.add_argument('--ratio', type=float, default=0.2, help='topk ratio value')
    args = parser.parse_args()

    # parameters
    folder = 'data'
    dataset = args.dataset

    # make directory
    qfoldder = f"{folder}/{dataset}/queries"
    os.makedirs(qfoldder, exist_ok=True)

    num_user, num_item, num_edge = read_stat_file(folder, dataset)
    popItem = top_hot_items_ratio(folder, dataset, num_item, args.ratio)
    query_gen(args, qfoldder, num_item, popItem)