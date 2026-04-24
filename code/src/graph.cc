#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define ASSERT(v) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; exit(1);}}

using namespace std;

void handle_error(const char* msg) {
	perror(msg);
	exit(255);
}

Graph::Graph(const string& t_folder, const string& t_graph): 
            m_folder(t_folder), m_graph(t_graph) {
    readNM();
    cout << "graph info: nu=" << this->m_nu << " nv=" << this->m_nv << " m=" << this->m_m << endl;

    this->m_uedges = std::vector<std::vector<uint>>(this->m_nu, std::vector<uint>());
    this->m_udeg = std::vector<uint>(m_nu, 0);
    this->m_vedges = std::vector<std::vector<uint>>(this->m_nv, std::vector<uint>());
    this->m_vdeg = std::vector<uint>(m_nv, 0);

    cout << "loading graph..." << endl;
    readGraph();
    preprocess();
}
void Graph::preprocess()
{

    this->uSorted.reserve(m_nu);
    this->vSorted.reserve(m_nv);

#pragma omp parallel for schedule(dynamic, 64)
    for (int u=0; u<m_nu; u++)
    {
        uSorted[u].assign(m_uedges[u].begin(), m_uedges[u].end());
        sort(uSorted[u].begin(), uSorted[u].end());
    }

#pragma omp parallel for schedule(dynamic, 64)
    for (int v=0; v<m_nv; v++)
    {
        vSorted[v].reserve(m_vedges[v].size());
        for (auto u:m_vedges[v])
        {
            if (m_udeg[u] < 2)continue;
            vSorted[v].push_back(u);
        }
        sort(vSorted[v].begin(), vSorted[v].end(), [this](auto a, auto b){return m_udeg[a] < m_udeg[b];});
    }

    this->m_uDeg = std::vector<uint64>(m_nu, 0);
    this->m_vDeg = std::vector<uint64>(m_nv, 0);
#pragma omp parallel for
    for (int u = 0; u < m_nu; ++u) {
        uint64 sum = 0;
        for (const auto v : m_uedges[u]) {
            sum += m_vdeg[v];
        }
        m_uDeg[u] = sum;
    }
#pragma omp parallel for
    for (int v = 0; v < m_nv; ++v) {
        uint64 sum = 0;
        for (const auto u : vSorted[v]) {
            sum += m_udeg[u];
        }
        m_vDeg[v] = sum;
    }
}

void Graph::readNM(){
    ifstream fin((m_folder + "/" + m_graph + "/stat.txt").c_str());
    string s;
    if (fin.is_open()){
        while (fin >> s){
            if (s.substr(0, 2) == "u="){
                this->m_nu = atoi(s.substr(2).c_str());
                continue;
            }
            if (s.substr(0, 2) == "v="){
                this->m_nv = atoi(s.substr(2).c_str());
                continue;
            }
            if (s.substr(0, 2) == "m="){
                this->m_m = atoi(s.substr(2).c_str());
                continue;
            }
        }
        fin.close();
    }
    else handle_error("Fail to open attribute file!");
}

void Graph::readGraph(){
    FILE *fin = fopen((m_folder + "/" + m_graph + "/graph.txt.new").c_str(), "r");
    uint64 readCnt = 0;
    uint u, v;
	double w;
    while (fscanf(fin, "%d%d", &u, &v) != EOF) {
        readCnt++;
        ASSERT( u < this->m_nu );
        ASSERT( v < this->m_nv );
        addEdge(u, v);
        // cout << u << " " << v << endl;
    }
    fclose(fin);

    // ASSERT(readCnt == this->m_m);
    cout << "read Graph Done!" << endl;
}

void Graph::addEdge(uint u, uint v){
    this->m_uedges[u].push_back(v);
    this->m_vedges[v].push_back(u);
    this->m_udeg[u]+=1;
    this->m_vdeg[v]+=1;
}

uint Graph::getUDeg(uint u) const{
    return this->m_udeg[u];
}

uint Graph::getVDeg(uint v) const{
    return this->m_vdeg[v];
}

uint Graph::getOriVDeg(uint v) const{
    if (getReo()) v = this->oriIndex[v];
    return this->m_vdeg[v];
}

uint64 Graph::getUNDeg(uint v) const{
    return this->m_uDeg[v];
}
uint64 Graph::getVNDeg(uint v) const{
    return this->m_vDeg[v];
    // uint64 res = 0;
    // for (auto u: this->m_vedges[v])
    // {
    //     res += this->getUDeg(u);
    // }
    // return res;
}

uint Graph::getNu() const{
    return this->m_nu;
}

uint Graph::getNv() const{
    return this->m_nv;
}

uint64 Graph::getM() const{
    return this->m_m;
}

std::string Graph::getGraphFolder() const{
    std::string folder(m_folder + "/" + m_graph + "/");
    return folder;
}

bool Graph::getReo() const{
    return this->Flag_Reo;
}
void Graph::OriOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);
    for (int i = 0; i < num_item; ++i)
    {
        this->mapIndex[i] = i;
        this->oriIndex[i] = i;
    }
}
void Graph::DegOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    const int DegLim=1000;
    // count sort
    std::vector<int> deg(num_item);
    for (int i = 0; i < num_item; ++i) {
        deg[i] = getVDeg(i);
        this->oriIndex[i] = i;
    }

    /*if (num_item * log(num_item) > 2 * max_deg + num_item)
    {
        cout << "bucket sort" << endl;
        std::vector<int> bin(max_deg + 1,0);
        for (int i = 0; i < num_item; ++i) bin[deg[i]]++;
        for (int i = 1; i <= max_deg; ++i) bin[i] += bin[i-1];
        for (int i = 0; i < num_item; ++i)
        {
            this->mapIndex[i] = --bin[deg[i]];
            this->oriIndex[this->mapIndex[i]] = i;
        }
    }else*/
    {
        cout << "Fast sort" << endl;
        sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
        for (int i = 0; i < num_item; ++i)
        {
            this->mapIndex[this->oriIndex[i]] = i;
        }
    }
}
void Graph::SampleOrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    std::vector<uint> vertices(num_item);
    std::vector<int> BigD,SmaD;
    auto deg=this->m_vdeg;
    int pos = num_item - (int)std::ceil(num_item / std::log((double)num_item));
    std::nth_element(deg.begin(), deg.begin() + pos, deg.end());
    int DegLim = deg[pos];
    cerr << DegLim << endl;
    // count sort
    uint max_deg = 0;
    for (int i = 0; i < num_item; ++i) {
        deg[i] = this->m_vdeg[i];
        if (deg[i]>DegLim) BigD.push_back(i);
        else
        {
            SmaD.push_back(i);
            max_deg = std::max(max_deg, deg[i]);
        }
    }
    sort(BigD.begin(), BigD.end(), [&](int a,int b){return deg[a]>deg[b];});
    int Bigind = 0;
    for (auto v: BigD)
    {
        vertices[Bigind++] = v;
    }
    // cout << "BigIndex: " << Bigind << " SmaIndex: " << SmaD.size() << endl;
    std::vector<int> bin(max_deg + 1,0);
    for (auto i: SmaD) bin[deg[i]]++;
    for (int i = max_deg-1; i >=0; --i) bin[i] += bin[i+1];
    for (auto i: SmaD)
    {
        vertices[(--bin[deg[i]]) + Bigind] = i;
    }

    vector<bool> visited(num_item, false);
    priority_queue<pair<double,int>>Q;

    vector<int> vis_user(num_user, -1);
    vector<int> vis_item(num_item, -1);
    vector<double> temp_weight(num_item, 0);
    vector<int> temp_cnt(num_item, 0);
    vector<int> update_list(num_item, -1);
    vector<bool> update_mask(num_item, false);

    // const int huge_vertex = sqrt((double)v_num);
    const int window_size = 32;
    int cur_v_idx = 0;
    int cur_p_idx = -1;
    int cur_dll = 0;
    const int lim = 50;
    while (cur_v_idx < num_item) {
        int v;
        if (cur_v_idx % window_size == 0 ||Q.empty()) {
            v = vertices[cur_dll++];
            while (visited[v]) v = vertices[cur_dll++];
            visited[v] = true;
            Q = {};
            cur_p_idx++;
        } else {
            v = Q.top().second;
            Q.pop();
            while (visited[v] && !Q.empty())
            {
                v = Q.top().second;
                Q.pop();
            }
            while (visited[v]) v = vertices[cur_dll++];
            visited[v] = true;
        }

        this->mapIndex[v] = cur_v_idx++;
        // if (cur_v_idx == num_item) break;

        // if (cur_v_idx % PACK_WIDTH == 0) continue;
        int update_index=0;



        vector<uint> uList = random_sample(this->m_vedges[v], lim);
        // vector<uint> uList;
        // for (auto u: this->m_vedges[v])
        // {
        //     if (this->m_udeg[u] >= lim)
        //     {
        //         uList.push_back(u);
        //     }
        // }
        // double pb = this->m_vdeg[v] / (double)uList.size();
        for (auto u: uList) {
            if (vis_user[u] == cur_p_idx) continue;
            vis_user[u] = cur_p_idx;
            for (auto vt: this->m_uedges[u]) {
                if (visited[vt]) continue;

                if (vis_item[vt] != cur_p_idx)
                {
                    vis_item[vt] = cur_p_idx;
                    temp_weight[vt] = 0;
                }
                temp_cnt[vt]++;
                // temp_weight[vt] += 1;
                if (!update_mask[vt])
                {
                    update_mask[vt] = true;
                    update_list[update_index++] = vt;
                }
            }
        }

        // update
        for (int i=0; i < update_index;i++)
        {
            int vt = update_list[i];
            // temp_weight[vt] += temp_cnt[vt] * (temp_cnt[vt] - 1) / 2;
            temp_weight[vt] += temp_cnt[vt];
            temp_cnt[vt] = 0;
            Q.push({temp_weight[vt], vt});
            update_mask[vt] = false;
        }
    }

    // update orgId.
    for (int i = 0; i < num_item; ++i)
    {
        if (!visited[i])
        {
            this->mapIndex[i] = i;
        }
        this->oriIndex[this->mapIndex[i]] = i;
    }
}
void Graph::setBuild()
{
    for (int u=0; u<m_nu;u++)
    {
        this->uedgeSet.emplace_back(this->m_uedges[u].begin(),this->m_uedges[u].end());
    }
}