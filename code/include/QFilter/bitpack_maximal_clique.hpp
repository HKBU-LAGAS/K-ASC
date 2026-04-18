#ifndef _BITPACK_MAXIMAL_CLIQUE_H
#define _BITPACK_MAXIMAL_CLIQUE_H

#include "util.hpp"
#include "set_operation.hpp"
#include <random>

template<typename T>
std::vector<T> random_list(const std::vector<T>& source, size_t count) {
    if (count > source.size()) {
        // 根据需要处理错误，可以抛出异常或返回一个空/完整的 vector
        // 这里我们选择返回一个完整的副本
        return source;
    }

    std::vector<T> sampled_elements;

    // 静态的生成器可以避免每次调用都重新播种，提高性能
    // 但在多线程环境下需要使用 thread_local
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::sample(
        source.begin(),
        source.end(),
        std::back_inserter(sampled_elements),
        count,
        gen
    );

    return sampled_elements;
}
class BPMaximalClique
{
public:
    int v_num, p_num;
    long long e_num;
    std::vector<UVertex> graph;
    int *pool_base = NULL;
    PackState *pool_state = NULL;
    int *sets_base = NULL;
    PackState *sets_state = NULL;

    BPMaximalClique();
    ~BPMaximalClique();

    template<typename GraphType>
    double build(const GraphType& g)
    {
        v_num = g.getNu();
        e_num = g.getM();
        p_num = v_num / PACK_WIDTH;
        if (v_num % PACK_WIDTH != 0) p_num++;

        graph.resize(v_num);

        int cur_packnode_idx = -1;
        for (uint u = 0; u < v_num; u++)
        {
            std::vector<uint> Uedge(g.getUDeg(u));
            if (g.getReo() == true)
            {
                for (int i=0; i < Uedge.size(); i++)
                {
                    Uedge[i] = g.mapIndex[g.m_uedges[u][i]];
                }
                sort(Uedge.begin(), Uedge.end());
            }else
            {
                for (int i=0; i < Uedge.size(); i++)
                {
                    Uedge[i] = g.m_uedges[u][i];
                }
            }
            graph[u].deg = 0;
            for (auto it = Uedge.begin(); it != Uedge.end(); it++)
            {
                int vid = (*it);
                int v_base = (vid >> PACK_SHIFT);
                PackState v_bit = ((PackState)1 << (vid & PACK_MASK));
                if (it == Uedge.begin())
                {
                    graph[u].deg++;
                    graph[u].start = ++cur_packnode_idx;
                    pool_base[cur_packnode_idx] = v_base;
                    pool_state[cur_packnode_idx] = v_bit;
                }else
                {
                    if (pool_base[cur_packnode_idx] == v_base) {
                        pool_state[cur_packnode_idx] |= v_bit;
                    } else {
                        graph[u].deg++;
                        pool_base[++cur_packnode_idx] = v_base;
                        pool_state[cur_packnode_idx] = v_bit;
                    }
                }
            }
        }
        cur_packnode_idx++;

        double comp_ratio = (double)cur_packnode_idx / e_num;
        printf("comp_ratio=%d/%lld, %.4f\n", cur_packnode_idx, e_num, comp_ratio);
        return comp_ratio;
    }
    template<typename GraphType>
    double Twobuild(const GraphType& g)
    {
        v_num = g.getNu();
        e_num = g.getM();
        p_num = v_num / PACK_WIDTH;
        if (v_num % PACK_WIDTH != 0) p_num++;

        graph.resize(v_num);

        int cur_packnode_idx = -1;
        for (uint u = 0; u < v_num; u++)
        {
            std::vector<uint> Uedge(g.getUDeg(u));
            for (int i=0; i < Uedge.size(); i++)
            {
                Uedge[i] = g.mapIndex[g.m_uedges[u][i]];
            }
            sort(Uedge.begin(), Uedge.end());
            graph[u].deg = 0;
            for (auto it = Uedge.begin(); it != Uedge.end(); it++)
            {
                int vid = (*it);
                int v_base = (vid >> PACK_SHIFT);
                PackState v_bit = ((PackState)1 << (vid & PACK_MASK));
                if (it == Uedge.begin())
                {
                    graph[u].deg++;
                    graph[u].start = ++cur_packnode_idx;
                    pool_base[cur_packnode_idx] = v_base;
                    pool_state[cur_packnode_idx] = v_bit;
                }else
                {
                    if (pool_base[cur_packnode_idx] == v_base) {
                        pool_state[cur_packnode_idx] |= v_bit;
                    } else {
                        graph[u].deg++;
                        pool_base[++cur_packnode_idx] = v_base;
                        pool_state[cur_packnode_idx] = v_bit;
                    }
                }
            }
        }
        cur_packnode_idx++;

        double comp_ratio = (double)cur_packnode_idx / e_num;
        printf("comp_ratio=%d/%lld, %.4f\n", cur_packnode_idx, e_num, comp_ratio);
        return comp_ratio;
    }
    template<typename GraphType>
    double ReoBuild(const GraphType& g)
    {
        v_num = g.getNu();
        e_num = g.getM();
        p_num = v_num / PACK_WIDTH;
        if (v_num % PACK_WIDTH != 0) p_num++;

        graph.resize(v_num);
        std::vector<std::vector<uint>>mapUedge(v_num);
        for (int i = 0; i < v_num; i++) mapUedge[i].reserve(g.m_udeg[i]);
        for (int i = 0 ; i < g.oriIndex.size(); i++)
        {
            int v = g.oriIndex[i];
            for (auto u: g.m_vedges[v])
            {
                mapUedge[u].push_back(i);
            }
        }
        int cur_packnode_idx = -1;
        for (uint u = 0; u < v_num; u++)
        {
            std::vector<uint>& Uedge = mapUedge[u];

            graph[u].deg = 0;
            for (auto it = Uedge.begin(); it != Uedge.end(); it++)
            {
                int vid = (*it);
                int v_base = (vid >> PACK_SHIFT);
                PackState v_bit = ((PackState)1 << (vid & PACK_MASK));
                if (it == Uedge.begin())
                {
                    graph[u].deg++;
                    graph[u].start = ++cur_packnode_idx;
                    pool_base[cur_packnode_idx] = v_base;
                    pool_state[cur_packnode_idx] = v_bit;
                }else
                {
                    if (pool_base[cur_packnode_idx] == v_base) {
                        pool_state[cur_packnode_idx] |= v_bit;
                    } else {
                        graph[u].deg++;
                        pool_base[++cur_packnode_idx] = v_base;
                        pool_state[cur_packnode_idx] = v_bit;
                    }
                }
            }
        }
        cur_packnode_idx++;

        double comp_ratio = (double)cur_packnode_idx / e_num;
        printf("comp_ratio=%d/%lld, %.4f\n", cur_packnode_idx, e_num, comp_ratio);
        return comp_ratio;
    }
    template<typename GraphType>
    double SimpleBuild(const GraphType& g)
    {
        v_num = g.getNu();
        e_num = g.getM();
        p_num = v_num;
        graph.resize(v_num);

        int cur_packnode_idx = -1;
        for (uint u = 0; u < v_num; u++)
        {
            std::vector<uint> Uedge(g.getUDeg(u));
            for (int i=0; i < Uedge.size(); i++)
            {
                Uedge[i] = g.m_uedges[u][i];
            }
            sort(Uedge.begin(), Uedge.end());
            graph[u].deg = 0;
            for (auto it = Uedge.begin(); it != Uedge.end(); it++)
            {
                int vid = (*it);
                if (it == Uedge.begin())
                {
                    graph[u].deg++;
                    graph[u].start = ++cur_packnode_idx;
                    pool_base[cur_packnode_idx] = vid;
                }else
                {
                    graph[u].deg++;
                    pool_base[++cur_packnode_idx] = vid;
                }
            }
        }
        cur_packnode_idx++;

        double comp_ratio = (double)cur_packnode_idx / e_num;
        printf("comp_ratio=%d/%lld, %.4f\n", cur_packnode_idx, e_num, comp_ratio);
        return comp_ratio;
    }
    template<typename GraphType>
    double build(const GraphType& g, const int& TruncNum)
    {
        v_num = g.getNu();
        e_num = 0;
        p_num = v_num / PACK_WIDTH;
        if (v_num % PACK_WIDTH != 0) p_num++;

        graph.resize(v_num);

        int cur_packnode_idx = -1;
        for (uint u = 0; u < v_num; u++)
        {
            // std::vector<uint> Uedge(g.getUDeg(u));
            std::vector<uint> Uedge = random_list(g.m_uedges[u], TruncNum);
            e_num += Uedge.size();
            if (g.getReo() == true)
            {
                for (int i=0; i < Uedge.size(); i++)
                {
                    Uedge[i] = g.mapIndex[Uedge[i]];
                }
                sort(Uedge.begin(), Uedge.end());
            }
            graph[u].deg = 0;
            for (auto it = Uedge.begin(); it != Uedge.end(); it++)
            {
                int vid = (*it);
                int v_base = (vid >> PACK_SHIFT);
                PackState v_bit = ((PackState)1 << (vid & PACK_MASK));
                if (it == Uedge.begin())
                {
                    graph[u].deg++;
                    graph[u].start = ++cur_packnode_idx;
                    pool_base[cur_packnode_idx] = v_base;
                    pool_state[cur_packnode_idx] = v_bit;
                }else
                {
                    if (pool_base[cur_packnode_idx] == v_base) {
                        pool_state[cur_packnode_idx] |= v_bit;
                    } else {
                        graph[u].deg++;
                        pool_base[++cur_packnode_idx] = v_base;
                        pool_state[cur_packnode_idx] = v_bit;
                    }
                }
            }
        }
        cur_packnode_idx++;

        double comp_ratio = (double)cur_packnode_idx / e_num;
        printf("comp_ratio=%d/%lld, %.4f\n", cur_packnode_idx, e_num, comp_ratio);
        return comp_ratio;
    }

    double build(const EdgeVector& _e_v);

    std::vector<int> degeneracy_order();

    int maximal_clique_degen();

    void save_answers(const char* file_path);

private:
    EdgeVector edge_vec;
    std::vector<int> org_deg;

    int *pool_mc = NULL, pool_mc_idx = 0, mc_num = 0;

    int max_pool_sets_idx = 0, maximum_clique_size = 0;
    int intersect_call_time = 0, big_intersect_call_time = 0;

    void Tomita(std::vector<int>& R, UVertex P, UVertex X);
};

#endif