#ifndef GRAPH_H
#define GRAPH_H

#include <QFilter/util.hpp>
#include <QFilter/set_operation.hpp>
#include <QFilter/bitpack_maximal_clique.hpp>
#include "utils.h"

class Graph{
    public:
        std::vector<std::vector<uint>> m_uedges; // edges of each node in U
		std::vector<std::vector<uint>> m_vedges; // edges of each node in V
        std::vector<uint> m_udeg;     // degree of each node in U
		std::vector<uint> m_vdeg;     // degree of each node in V
        std::vector<uint64> m_uDeg;
        std::vector<uint64> m_vDeg;
        std::string m_graph;
        std::vector<uint> mapIndex;
        std::vector<uint> oriIndex;
        std::vector<ankerl::unordered_dense::set<uint>> uedgeSet;
        std::vector<ankerl::unordered_dense::map<uint,uint>> uedgeMap;
        std::vector<std::vector<int>> uSorted; // edges of each node in U
        std::vector<std::vector<int>> vSorted; // edges of each node in v
        uint hotTHR;
        bool Flag_Reo=false;
    private:
        std::string m_folder;
        uint m_nu; // the number of nodes in U
        uint m_nv; // the number of nodes in V
        uint64 m_m; // the number of edges

        void readNM();
        void readGraph();
        void addEdge(uint u, uint v);
    public:
        uint getUDeg(uint u) const;
		uint getVDeg(uint v) const;
        uint getOriVDeg(uint v) const;
        uint64 getM() const;
        uint64 getUNDeg(uint v) const;
        uint64 getVNDeg(uint v) const;
        uint getNu() const;
        uint getNv() const;
        std::string getGraphFolder() const;
        bool getReo() const;
        // const std::vector<std::pair<uint, double>>& operator [] (uint u) const;

        Graph(const std::string& t_folder, const std::string& t_graph);
        void preprocess();
        void setBuild();
        void mapBuild(const BPMaximalClique& fast_set)
        {
            this->uedgeMap.resize(m_nu);
            int* pool_base = fast_set.pool_base;
            int* pool_state = fast_set.pool_state;
            for (int u=0; u<m_nu;u++)
            {
                const UVertex& x = fast_set.graph[u];
                const auto& bases =  pool_base + x.start;
                const auto& states =  pool_state + x.start;
                const auto& d = x.deg;
                for (int i=0; i < d; i++)
                {
                    this->uedgeMap[u][bases[i]]=states[i];
                }
            }
        }
        // void
        void OriOrder();
        void DegOrder();
        void SampleOrder();
};
#endif
