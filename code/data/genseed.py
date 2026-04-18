import random
import sys,os
import numpy as np
import argparse
import matplotlib.pyplot as plt
from utils import read_stat_file

def data_load(dataset, num_user, num_item):
    graph_file = f"data/{dataset}/graph.txt.new"
    degree_user = np.zeros(num_user, dtype=np.int32)
    degree_item = np.zeros(num_item, dtype=np.int32)

    with open(graph_file, 'r') as f:
        for line in f:
            toks = line.strip().split()
            # 如果格式正确
            if len(toks) == 2:
                # 这里直接转 int
                u = int(toks[0])
                v = int(toks[1])
                if 0 <= u < num_user:
                    degree_user[u] += 1
                else :
                    raise ValueError(f"User ID {u} out of range [0, {num_user})")
                if 0 <= v < num_item:
                    degree_item[v] += 1
                else :
                    raise ValueError(f"Item ID {v} out of range [0, {num_item})")
            else:
                print(line)
                raise ValueError(f"Invalid line format: {line.strip()}")

    return degree_user, degree_item

def plot_degree_distribution(pic_folder, dataset, degree_item, query_list, extra=''):
    pic_folder = os.path.join(pic_folder, f"{dataset}/plots")
    os.makedirs(pic_folder, exist_ok=True)
    # 把 query_list 里的item和其degree配对，排序
    items_with_degree = [(item, degree_item[item]) for item in query_list]
    sorted_items = sorted(items_with_degree, key=lambda x: x[1], reverse=True)
    sorted_query_list = [item for item, deg in sorted_items]
    sorted_query_degrees = [deg for item, deg in sorted_items]
    n = len(sorted_query_degrees)
    # 创建一个画布，两行一列的子图
    plt.figure(figsize=(6, 4))
    plt.plot(sorted_query_degrees, lw=0.8)

    # 计算 20%、40%、60%、80% 位置（索引）
    percents = [0.2, 0.4, 0.6, 0.8, 1.0]
    colors = ['r', 'g', 'b', 'purple', 'orange']
    # 画垂直线
    for p, c in zip(percents, colors):
        idx = int(p * n)
        plt.axvline(idx, color=c, linestyle='--', linewidth=1)
        # 可以选择是否加text
        # plt.text(idx, plt.ylim()[0], f'{int(p*100)}%', color=c, ha='center', va='bottom', fontsize=10)

    plt.xlabel('Query Item (sorted by degree)')
    # plt.ylabel('Degree')
    plt.text(0,1.05,'Degree',transform=plt.gca().transAxes,
             fontsize=10, verticalalignment='top', fontstyle='italic')
    plt.title(f'{dataset} Query-list Degree Distribution')
    plt.yscale('log')
    plt.grid(True)

    # 布局更紧凑，保存
    plt.tight_layout()
    joint_path = os.path.join(pic_folder, f'query_list_distribution{extra}.png')
    plt.savefig(joint_path, dpi=300, bbox_inches='tight')
    plt.close()
    print('Save query_list degree distribution (sorted) figure to', joint_path)
    return sorted_query_list
def top_hot_items_ratio(folder, dataset, num_item, ratio):
    graph_file = f"{folder}/{dataset}/graph.txt.new"
    degree = np.zeros(num_item, dtype=np.int32)
    with open(graph_file, 'r') as f:
        for line in f:
            toks = line.strip().split()
            # 如果格式正确
            if len(toks) == 2:
                # 这里直接转 int
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
def top_hot_items_LB(folder, dataset, num_item, bound):
    graph_file = f"{folder}/{dataset}/graph.txt.new"
    degree = np.zeros(num_item, dtype=np.int32)
    with open(graph_file, 'r') as f:
        for line in f:
            toks = line.strip().split()
            # 如果格式正确
            if len(toks) == 2:
                # 这里直接转 int
                y = int(toks[1])
                if 0 <= y < num_item:
                    degree[y] += 1
                else :
                    raise ValueError(f"Item ID {y} out of range [0, {num_item})")
            else:
                raise ValueError(f"Invalid line format: {line.strip()}")

    # 只拿前 degree >= bound
    hot_items_indices = np.where(degree >= bound)[0]
    return hot_items_indices.tolist()
def top_hot_items_UB(folder, dataset, num_item, bound):
    graph_file = f"{folder}/{dataset}/graph.txt.new"
    degree = np.zeros(num_item, dtype=np.int32)
    with open(graph_file, 'r') as f:
        for line in f:
            toks = line.strip().split()
            # 如果格式正确
            if len(toks) == 2:
                # 这里直接转 int
                y = int(toks[1])
                if 0 <= y < num_item:
                    degree[y] += 1
                else :
                    raise ValueError(f"Item ID {y} out of range [0, {num_item})")
            else:
                raise ValueError(f"Invalid line format: {line.strip()}")

    # 只拿前 degree >= bound
    hot_items_indices = np.where(degree < bound)[0]
    return hot_items_indices.tolist()
def five_fold_divide(args, qfoldder, num_user, num_item):
    folder = 'data'
    dataset = args.dataset
    num_sample = args.sample
    file_name = f"{qfoldder}/seeds-all.txt"
    hot_items = list(range(num_item))

    query_list = []
    with open(file_name, "w") as f:
        for i in range(num_sample):
            x = random.choice(hot_items)
            query_list.append(x)
            f.write(str(x)+"\n")

    degree_user, degree_item = data_load(dataset, num_user, num_item)
    # plot distribution
    sorted_query_list = plot_degree_distribution(folder,dataset, degree_item, query_list)

    if args.divide > 0:
        total_queries = len(sorted_query_list)
        queries_per_division = total_queries // args.divide
        for div in range(args.divide):
            start_idx = div * queries_per_division
            end_idx = (div + 1) * queries_per_division if div < args.divide - 1 else total_queries
            sub_file_name = f"{qfoldder}/seeds-part{div+1}.txt"
            with open(sub_file_name, "w") as f:
                for q in sorted_query_list[start_idx:end_idx]:
                    f.write(str(q)+"\n")
            print(f"Saved division {div+1} to {sub_file_name}")

def LB_divide(args, qfoldder, num_user, num_item):
    folder = 'data'
    dataset = args.dataset
    num_sample = args.sample
    file_name = f"{qfoldder}/seeds-{args.bound}.txt"
    hot_items = top_hot_items_LB(folder, dataset, num_item, args.bound)

    query_list = []
    with open(file_name, "w") as f:
        for i in range(num_sample):
            x = random.choice(hot_items)
            query_list.append(x)
            f.write(str(x)+"\n")

    degree_user, degree_item = data_load(dataset, num_user, num_item)
    # plot distribution
    sorted_query_list = plot_degree_distribution(folder,dataset, degree_item, query_list, f"-{args.bound}")
def UB_divide(args, qfoldder, num_user, num_item):
    folder = 'data'
    dataset = args.dataset
    num_sample = args.sample
    file_name = f"{qfoldder}/seeds-{args.bound}.txt"
    hot_items = top_hot_items_UB(folder, dataset, num_item, args.bound)

    query_list = []
    with open(file_name, "w") as f:
        for i in range(num_sample):
            x = random.choice(hot_items)
            query_list.append(x)
            f.write(str(x)+"\n")

    degree_user, degree_item = data_load(dataset, num_user, num_item)
    # plot distribution
    sorted_query_list = plot_degree_distribution(folder,dataset, degree_item, query_list, f"-{args.bound}")
if __name__=='__main__':
    random.seed(202510)
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--dataset', required=True, help='dataset name')
    parser.add_argument('--sample', type=int, default=1000, help='number of samples')
    parser.add_argument('--ratio', type=float, default=0, help='topk ratio value')
    parser.add_argument('--bound', type=int, default=0, help='topk bound value')
    parser.add_argument('--divide', type=int, default=0, help='divide query set')
    parser.add_argument('--DivideOpt', type=str, default='None', help='sample type')
    args = parser.parse_args()

    # parameters
    folder = 'data'
    dataset = args.dataset
    num_sample = args.sample
    ratio = args.ratio
    num_user, num_item, num_edge = read_stat_file(folder, dataset)

    # make directory
    qfoldder = f"{folder}/{dataset}/queries"
    os.makedirs(qfoldder, exist_ok=True)

    # if ratio == 0:
    #     file_name = f"{qfoldder}/seeds-all.txt"
    #     hot_items = list(range(num_item))
    # else :
    #     hot_items = top_hot_items(folder, dataset, num_item, ratio)
    #     file_name = f"{qfoldder}/seeds-{ratio:.1f}.txt"

    # file_name = f"{folder}/{dataset}/seeds-{ratio:.1f}.txt"
    if args.DivideOpt=='FiveDiv':
        five_fold_divide(args, qfoldder, num_user, num_item)
    elif args.DivideOpt=='LowerBound':
        LB_divide(args, qfoldder, num_user, num_item)
    elif args.DivideOpt=='UpperBound':
        UB_divide(args, qfoldder, num_user, num_item)
    else :
        raise ValueError(f"Invalid DivideOpt {args.DivideOpt}")
    # query_list = []
    # with open(file_name, "w") as f:
    #     for i in range(num_sample):
    #         x = random.choice(hot_items)
    #         query_list.append(x)
    #         f.write(str(x)+"\n")
    #
    # degree_user, degree_item = data_load(dataset, num_user, num_item)
    # # plot distribution
    # sorted_query_list = plot_degree_distribution('data',dataset, degree_item, query_list)
    #
    # if args.divide > 0:
    #     total_queries = len(sorted_query_list)
    #     queries_per_division = total_queries // args.divide
    #     for div in range(args.divide):
    #         start_idx = div * queries_per_division
    #         end_idx = (div + 1) * queries_per_division if div < args.divide - 1 else total_queries
    #         sub_file_name = f"{qfoldder}/seeds-part{div+1}.txt"
    #         with open(sub_file_name, "w") as f:
    #             for q in sorted_query_list[start_idx:end_idx]:
    #                 f.write(str(q)+"\n")
    #         print(f"Saved division {div+1} to {sub_file_name}")
