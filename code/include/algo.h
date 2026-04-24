#ifndef ALGO_H
#define ALGO_H
#include "graph.h"
#include<unordered_set>
/* assistant function */
void intersection(const std::unordered_set<uint>& A, const std::unordered_set<uint>& B, vector<uint>& result);
int fast_item_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& Cij, uint ui, uint uj);

/* assistant class */
struct PairHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        // return (static_cast<std::size_t>(p.first) << 32) | p.second;
        // 使用现有的 std::hash 计算 p.first 和 p.second 的哈希值
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // 仿照 boost::hash_combine 的方式组合两个哈希值
        // 这是一个经过验证的、可以有效减少哈希冲突的算法
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
/* swing algorithm */
void ExactSwing(int src, double alpha, double eps, std::vector<double>& swing, const Graph& graph);
void FastExactSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph, const BPMaximalClique& fast_set);
void PRSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);
void FastASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastKASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);

#endif