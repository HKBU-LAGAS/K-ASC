#ifndef ALGO_H
#define ALGO_H
#include "graph.h"
#include<unordered_set>
/* assistant function */
void intersection(const std::unordered_set<uint>& A, const std::unordered_set<uint>& B, vector<uint>& result);
int fast_item_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& Cij, uint ui, uint uj);

/* assistant class */
class DotHash {
public:
    // 构造函数
    DotHash(int dim=256)
        : dimensions(dim) {
        // 初始化随机数生成器
        std::random_device rd;
        rng = std::mt19937(rd());
        Rnd = std::uniform_int_distribution<>(0, 1);
    }

    // 初始化签名 (对应 Python 的 init_signatures)
    void init_signatures(const Graph& graph) {
        int num_nodes = graph.getNu() + graph.getNv();
        // 1. 生成随机节点向量
        std::vector<std::vector<float>> node_vectors(num_nodes, std::vector<float>(dimensions));
        const float scale = std::sqrt(1.0f / static_cast<float>(dimensions));

        for (int i = 0; i < num_nodes; ++i) {
            for (int j = 0; j < dimensions; ++j) {
                // 生成 -1.0 或 1.0
                float random_val = (Rnd(rng) == 0) ? -1.0f : 1.0f;
                node_vectors[i][j] = random_val * scale;
            }
        }
        // 2. 计算 Adamic-Adar 节点缩放因子
        std::vector<float> node_scaling(num_nodes, 0.0f);

        for (int i = 0; i < num_nodes; ++i) {
            int num_neighbors = 0;
            if (i < graph.getNu())
            {
                num_neighbors = graph.getUDeg(i);
            }else
            {
                num_neighbors = graph.getVDeg(i - graph.getNu());
            }
            if (num_neighbors >= 2) {
                node_scaling[i] = std::sqrt(1.0f / std::log(static_cast<float>(num_neighbors)));
            }
        }

        // 3. 应用缩放因子
        for (int i = 0; i < num_nodes; ++i) {
            for (int j = 0; j < dimensions; ++j) {
                node_vectors[i][j] *= node_scaling[i];
            }
        }

        // 4. 计算节点签名
        signatures = std::vector<std::vector<float>> (num_nodes, std::vector<float>(dimensions, 0.0f));

        // 模拟 index_add_ 操作
        for (int u = 0; u < graph.getNu(); u++)
        {
            for (auto v:graph.m_uedges[u])
            {
                for (int d=0; d < dimensions; d++)
                {
                    signatures[u][d] += node_vectors[v + graph.getNu()][d];
                }
            }
        }
        for (int v = 0; v < graph.getNv(); v++)
        {
            for (auto u:graph.m_vedges[v])
            {
                for (int d=0; d < dimensions; d++)
                {
                    signatures[v + graph.getNu()][d] += node_vectors[u][d];
                }
            }
        }
    }

    // 计算分数 (user pairs)
    float user_pairs_scores(const int& u1, const int& u2) const{
        return dot(signatures[u1], signatures[u2]);
    }

private:
    int dimensions;
    std::vector<std::vector<float>> signatures;

    // 随机数生成器
    std::mt19937 rng;
    std::uniform_int_distribution<> Rnd;

    // 计算两个向量的点积
    float dot(const std::vector<float>& vec1, const std::vector<float>& vec2) const{
        float sum = 0.0f;
        for (size_t i = 0; i < vec1.size(); ++i) {
            sum += vec1[i] * vec2[i];
        }
        return sum;
    }
};
struct DouBuck
{
    BPMaximalClique fast_set;
    vector<bitset<hotTHR>> hot_set;
    // DouBuck(const Graph& graph)
    // {
    //     fast_set.build(graph, hotTHR);
    //     int num_user = graph.getNu();
    //     hot_set.resize(num_user);
    //     for (uint u = 0; u < num_user; u++)
    //     {
    //         std::vector<uint> Uedge;
    //         for (int i=0; i < Uedge.size(); i++)
    //         {
    //             int v = graph.mapIndex[graph.m_uedges[u][i]];
    //             if (v < hotTHR) hot_set[u][v] = 1;
    //         }
    //     }
    // }
    void build(const Graph& graph)
    {
        fast_set.build(graph, hotTHR);
        int num_user = graph.getNu();
        hot_set.resize(num_user);
        for (uint u = 0; u < num_user; u++)
        {
            std::vector<uint> Uedge;
            for (auto ordv: graph.m_uedges[u])
            {
                int v = graph.mapIndex[ordv];
                if (v < hotTHR) hot_set[u][v] = 1;
            }
        }
    }
};
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
void TruncSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);
void FastTruncSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void MonteCarloSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);
void FastMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void GroupMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);
void FastGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastKASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastGWS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);

void FastGSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUSS2(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUSS3(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUSS4(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void FastUSS5(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void ConstUSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);

void ASCGWS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void ASCGWS2(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void ASCGWS4(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void WedgeSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng);

long long AdaptGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void CutMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, std::mt19937& rng);
void TopKSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng, int K);
void FastAdapSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng);
void TestSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, std::mt19937& rng);
void WeightWedgeSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng);
void CorMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, std::mt19937& rng);
void WedgeMixSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng);
void FastReorderSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph, const BPMaximalClique& fast_set);
void DouBuckSwing(int vq, const Config& config, std::vector<double>& swing, const Graph& graph, const DouBuck& db);
void DBMCSwing(int vq, const Config& config, std::vector<double>& swing, const Graph& graph, const DouBuck& db, std::mt19937& rng);
void MixSetSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph, const BPMaximalClique& fast_set);
void PRSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config);
void TestGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void CompGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);
void CompExt(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set);

#endif