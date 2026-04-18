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
    cout << "processing graph..." << endl;

    // bool flag = 0;
    // for (int u=0; u<m_nu; u++)
    // {
        // sort(m_uedges[u].begin(),m_uedges[u].end());
        // m_uedges[u].erase(unique(m_uedges[u].begin(),m_uedges[u].end()),m_uedges[u].end());
        // if (m_uedges[u].size() != m_udeg[u])
        // {
        //     // cout<< "u=" << u << " redundant=" << m_udeg[u] - m_uedges[u].size() << '\n';
        //     m_udeg[u] = m_uedges[u].size();
        // }
    // }
    // for (int v=0;v<m_nv;v++)
    // {
    //     sort(m_vedges[v].begin(),m_vedges[v].end());
    //     m_vedges[v].erase(unique(m_vedges[v].begin(),m_vedges[v].end()),m_vedges[v].end());
    //     if (m_vedges[v].size() != m_vdeg[v])
    //     {
    //         // cout<< "v=" << v << " redundant=" << m_vdeg[v] - m_vedges[v].size() << '\n';
    //         flag=1;
    //         m_vdeg[v] = m_vedges[v].size();
    //     }
    // }
    // if (flag)
    // {
    //     cout << t_graph + " is not clear !" << endl;
    //     this->m_m = 0;
    //     for (int u=0; u<m_nu; u++)
    //     {
    //         this->m_m += m_udeg[u];
    //     }
    //     cout << this->m_m << endl;
    //     // assert(0);
    // }
    // this->m_muwsum = *std::max_element(this->m_uwsum.begin(),this->m_uwsum.end());
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
void Graph::AdapDegOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    const int DegLim=1000;
    // count sort
    std::vector<int> BigD,SmaD;
    std::vector<int> deg(num_item);
    int max_deg = 0;
    for (int i = 0; i < num_item; ++i) {
        deg[i] = getVDeg(i);
        if (deg[i]>DegLim) BigD.push_back(i);
        else
        {
            SmaD.push_back(i);
            max_deg = std::max(max_deg, deg[i]);
        }
        // this->oriIndex[i] = i;
    }

    sort(BigD.begin(), BigD.end(), [&](int a,int b){return deg[a]>deg[b];});
    int Bigind = 0;
    for (auto v: BigD)
    {
        this->mapIndex[v] = Bigind++;
        this->oriIndex[this->mapIndex[v]] = v;
    }
    cout << "BigIndex: " << Bigind << " SmaIndex: " << SmaD.size() << endl;
    std::vector<int> bin(max_deg + 1,0);
    for (auto i: SmaD) bin[deg[i]]++;
    for (int i = max_deg-1; i >=0; --i) bin[i] += bin[i+1];
    for (auto i: SmaD)
    {
        this->mapIndex[i] = (--bin[deg[i]]) + Bigind;
        this->oriIndex[this->mapIndex[i]] = i;
    }

   /* {
        cout << "check" << endl;
        vector<int> ori(num_item,0);
        vector<int> mp(num_item,0);
        for (int i=0;i<num_item;i++) ori[i] = i;
        sort(ori.begin(), ori.end(), [&](uint a, uint b){return deg[a] > deg[b];});
        for (int i = 0; i < num_item; ++i)
        {
            mp[ori[i]] = i;
        }
        for (int i = 0; i < num_item; ++i)
        {
            cout <<"ori[" << ori[i] << "]: " << deg[ori[i]] << ' ' << "ExOri[" << this->oriIndex[i] << "]:" << deg[this->oriIndex[i]] << endl;
            if (deg[ori[i]] != deg[this->oriIndex[i]])
            {
                cout <<deg[ori[i]] << ' ' << deg[this->oriIndex[i]] << endl;
                // assert(0);
                sleep(5);
            }
            // cout <<"ori[" << ori[i] << "]: " << deg[ori[i]] << ' ' << "ExOri[" << this->oriIndex[i] << "]:" << deg[this->oriIndex[i]] << endl;
        }
        cout << "check passed" << endl;
    }*/
}
void Graph::WeightOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<long long> Uweigt(this->m_nu);
    for (int u = 0; u < num_user; ++u)
    {
        for (auto v: this->m_uedges[u])
        {
            Uweigt[u] += this->m_vdeg[v];
            if (Uweigt[u] < 0) assert(0);
        }
    }
    std::vector<long long> deg(num_item);
    for (int i = 0; i < num_item; ++i) {
        deg[i] = 0;
        for (auto u: this->m_vedges[i])
        {
            // deg[i] += sqrt(getUDeg(u));
            deg[i] += Uweigt[u];
            if (deg[i] < 0) assert(0);
        }
        this->oriIndex[i] = i;
    }
    sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
    for (int i = 0; i < num_item; ++i)
    {
        this->mapIndex[this->oriIndex[i]] = i;
    }
}
void Graph::SumOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<long long> deg(num_item);
    for (int i = 0; i < num_item; ++i) {
        deg[i] = 0;
        for (auto u: this->m_vedges[i])
        {
            // deg[i] += sqrt(getUDeg(u));
            deg[i] += this->m_udeg[u];
            if (deg[i] < 0) assert(0);
        }
        this->oriIndex[i] = i;
    }
    sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
    for (int i = 0; i < num_item; ++i)
    {
        this->mapIndex[this->oriIndex[i]] = i;
    }
}
void Graph::SqrtOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<double> deg(num_item);
    for (int i = 0; i < num_item; ++i) {
        deg[i] = 0;
        for (auto u: this->m_vedges[i])
        {
            // deg[i] += sqrt(getUDeg(u));
            deg[i] += sqrt(this->m_udeg[u]);
            if (deg[i] < 0) assert(0);
        }
        this->oriIndex[i] = i;
    }
    sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
    for (int i = 0; i < num_item; ++i)
    {
        this->mapIndex[this->oriIndex[i]] = i;
    }
}
void Graph::LogOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<double> deg(num_item);
    for (int i = 0; i < num_item; ++i) {
        deg[i] = 0;
        for (auto u: this->m_vedges[i])
        {
            // deg[i] += sqrt(getUDeg(u));
            deg[i] += log(this->m_udeg[u]);
            if (deg[i] < 0) assert(0);
        }
        this->oriIndex[i] = i;
    }
    sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
    for (int i = 0; i < num_item; ++i)
    {
        this->mapIndex[this->oriIndex[i]] = i;
    }
}
void Graph::LinearOrder(){
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<long long> Uweigt(this->m_nu);
    for (int u = 0; u < num_user; ++u)
    {
        for (auto v: this->m_uedges[u])
        {
            Uweigt[u] += this->m_vdeg[v];
            if (Uweigt[u] < 0) assert(0);
        }
    }
    vector<int> deg(num_item);
    for (int v = 0 ; v < num_item; ++v)
    {
        deg[v] = this->m_vdeg[v];
    }
    vector<uint> uvec(this->m_nu);
    for (int u = 0; u < num_user; ++u) uvec[u] = u;
    sort(uvec.begin(), uvec.end(), [&](uint u, uint v){return Uweigt[u] > Uweigt[v];});

    vector<bool> visited(num_item, false);
    int cur = 0;
    for (int u: uvec)
    {
        vector<int> tmp;
        for (auto v: this->m_uedges[u])
        {
            if (visited[v]) continue;
            tmp.push_back(v);
            // this->mapIndex[v] = cur++;
            visited[v] = true;
        }
        sort(tmp.begin(), tmp.end(), [&](int x, int y){return deg[x] > deg[y];});
        for (auto v: tmp)
        {
            this->mapIndex[v] = cur++;
        }
    }

    for (int i = 0; i < num_item; ++i)
    {
        this->oriIndex[this->mapIndex[i]] = i;
    }
}
void Graph::HeapOrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    std::vector<double> deg(num_item);
    std::vector<uint> vertices(num_item);
    for (int i = 0; i < num_item; ++i)
    {
        deg[i] = this->m_vdeg[i];
        vertices[i] = i;
    }
    std::sort(vertices.begin(), vertices.end(),
        [&](const int& a, const int& b) -> bool {
            if (deg[a] == deg[b]) return a < b;
            return deg[a] > deg[b];
        });

    vector<bool> visited(num_item, false);
    priority_queue<pair<long long,int>>Q;

    vector<int> vis_user(num_user, -1);
    vector<int> vis_item(num_item, -1);
    vector<long long> temp_weight(num_item, 0);
    vector<int> update_list(num_item, -1);
    vector<bool> update_mask(num_item, false);

    // const int huge_vertex = sqrt((double)v_num);
    const int window_size = 32;
    int cur_v_idx = 0;
    int cur_p_idx = -1;
    int cur_dll = 0;
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
        for (auto u: this->m_vedges[v]) {
            if (vis_user[u] == cur_p_idx) continue;
            vis_user[u] = cur_p_idx;
            for (auto vt: this->m_uedges[u]) {
                if (visited[vt]) continue;

                if (vis_item[vt] != cur_p_idx)
                {
                    vis_item[vt] = cur_p_idx;
                    temp_weight[vt] = 0;
                }
                temp_weight[vt] += 1;
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
            Q.push({temp_weight[vt], vt});
            update_mask[vt] = false;
        }
    }

    // update orgId.
    for (int i = 0; i < num_item; ++i)
    {
        this->oriIndex[this->mapIndex[i]] = i;
    }
}
void Graph::ROrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    std::vector<double> deg(num_item);
    std::vector<uint> vertices(num_item);
    for (int i = 0; i < num_item; ++i)
    {
        deg[i] = this->m_vdeg[i];
        vertices[i] = i;
    }
    std::sort(vertices.begin(), vertices.end(),
        [&](const int& a, const int& b) -> bool {
            if (deg[a] == deg[b]) return a < b;
            return deg[a] > deg[b];
        });

    vector<bool> visited(num_item, false);
    priority_queue<pair<long long,int>>Q;

    vector<int> vis_user(num_user, -1);
    vector<int> vis_item(num_item, -1);
    vector<long long> temp_weight(num_item, 0);
    vector<int> update_list(num_item, -1);
    vector<bool> update_mask(num_item, false);

    // const int huge_vertex = sqrt((double)v_num);
    const int window_size = 32;
    int cur_v_idx = 0;
    int cur_p_idx = -1;
    int cur_dll = 0;
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
        for (auto u: this->m_vedges[v]) {
            if (vis_user[u] == cur_p_idx) continue;
            vis_user[u] = cur_p_idx;
            for (auto vt: this->m_uedges[u]) {
                if (visited[vt]) continue;

                if (vis_item[vt] != cur_p_idx)
                {
                    vis_item[vt] = cur_p_idx;
                    temp_weight[vt] = 0;
                }
                temp_weight[vt] += 1;
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
            Q.push({temp_weight[vt], vt});
            update_mask[vt] = false;
        }
    }

    // update orgId.
    for (int i = 0; i < num_item; ++i)
    {
        this->oriIndex[this->mapIndex[i]] = i;
    }
}
void Graph::ButterflyOrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    std::vector<uint> deg(num_item);
    std::vector<uint> vertices(num_item);
    for (int i = 0; i < num_item; ++i)
    {
        deg[i] = this->m_vdeg[i];
        vertices[i] = i;
    }
    std::sort(vertices.begin(), vertices.end(),
        [&](const int& a, const int& b) -> bool {
            if (deg[a] == deg[b]) return a < b;
            return deg[a] > deg[b];
        });

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
    const int lim = 100;
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



        // vector<uint> uList;
        vector<uint> uList = random_sample(this->m_vedges[v], 50);
        // for (auto u: this->m_vedges[v])
        // {
        //     if (this->m_udeg[u] >= lim)
        //     {
        //         uList.push_back(u);
        //     }
        // }
        double pb = this->m_vdeg[v] / (double)uList.size();
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
                temp_weight[vt] += 1;
                if (!update_mask[vt])
                {
                    update_mask[vt] = true;
                    update_list[update_index++] = vt;
                }
            }
        }

        // int T = min((int)this->m_vdeg[v], lim);   // wedge sample count
        // int T = 100;
        // int T = (int)this->m_vdeg[v];
        // if (T > 0) {
        //     for (int s = 0; s < T; ++s) {
        //         // 1) uniformly sample u from N(v)
        //         const auto &vu = this->m_vedges[v];
        //         uint u = vu[rand() % vu.size()];
        //
        //         // 2) uniformly sample vt from N(u)
        //         const auto &uv = this->m_uedges[u];
        //         if (uv.empty()) continue;
        //
        //         uint vt = uv[rand() % uv.size()];
        //         if (visited[vt]) continue;
        //
        //         // inverse-probability compensation:
        //         // p((u,vt)) = 1/deg(v) * 1/deg(u)
        //         // T = deg(v)
        //         // contribution = 1 / (T * p) = deg(u)
        //         double pb = (double)uv.size() * vu.size() / T;
        //         // double pb = (double)uv.size();
        //
        //         if (vis_item[vt] != cur_p_idx) {
        //             vis_item[vt] = cur_p_idx;
        //             temp_weight[vt] = 0;
        //         }
        //         temp_weight[vt] += 1;
        //
        //         if (!update_mask[vt]) {
        //             update_mask[vt] = true;
        //             update_list[update_index++] = vt;
        //         }
        //     }
        // }

        // update
        for (int i=0; i < update_index;i++)
        {
            int vt = update_list[i];
            // temp_weight[vt] = temp_cnt[vt] * (temp_cnt[vt] - 1);
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
void Graph::LvOrder(){
    this->Flag_Reo = true;
    uint num_user = this->m_nu;
    uint num_item = this->m_nv;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    std::random_device rd;
    std::mt19937 g(rd());
    float resolution = 1;
    float tol_optimization = 1e-6;
    uint n = num_user + num_item;

    // --- 1. 初始化 ---
    // 初始标签：每个节点一个社区
    auto labels = vector<uint>(n, 0);
    std::iota(labels.begin(), labels.end(), 0);
    // 缓存所有节点的度数
    float sqrt_degree_sum = 1.0 / sqrt(2 * (this->getM()));
    std::vector<float> degrees(n);
    for (uint i = 0; i < num_user; ++i) degrees[i] = sqrt_degree_sum * this->getUDeg(i);
    for (uint i = 0; i < num_item; ++i) degrees[num_user + i] = sqrt_degree_sum * this->getVDeg(i);
    // 大小设为 n 足够，因为社区 ID 不会超过 n
    std::vector<float> cluster_weights(n);
    // 社区权重初始化
    vector<float>user_cluster_weights(n);
    vector<float>item_cluster_weights(n);  // 同上
    for (int i=0; i<num_user; ++i){
        user_cluster_weights[i] = degrees[i];
    }
    for (int i=0; i<num_item; ++i){
        item_cluster_weights[num_user + i] = degrees[num_user + i];
    }

    // 用于在一次迭代中临时存储到邻居社区的连接权重

    bool stop = false;
    while (!stop) {
        float increase_pass = 0.0f;
        vector<int>candidate(n);
        std::iota(candidate.begin(),candidate.end(),0);
        std::shuffle(candidate.begin(),candidate.end(),g);
        // --- 2. 遍历所有节点 ---
        for (auto x:candidate) {
            float k_i = degrees[x];
            int current_label = labels[x];
            std::set<int> neighbor_labels;
            // --- 2.1 收集邻居社区信息 ---
            std::unordered_map<int, float> neighbor_comm_weights;
            if (x < num_user)
            {
                int i = x;
                for (auto neighbor_node: this->m_uedges[i]) {
                    int target_label = labels[num_user + neighbor_node];
                    neighbor_labels.insert(target_label);
                    cluster_weights[target_label] += 1; // 累加到目标社区的权重
                }
            }else
            {
                int i = x - num_user;
                for (auto neighbor_node: this->m_vedges[i]) {
                    int target_label = labels[neighbor_node];
                    neighbor_labels.insert(target_label);
                    cluster_weights[target_label] += 1 ; // 累加到目标社区的权重
                }
            }

            // 从候选中移除节点当前的社区
            neighbor_labels.erase(current_label);

            if (!neighbor_labels.empty()) {
                float user_weight_i=0, item_weight_i=0;
                if (x < num_user)
                {
                    user_weight_i = degrees[x];
                }else
                {
                    item_weight_i = degrees[x];
                }

                // --- 2.2 计算移出当前社区的模块度变化 (delta) ---
                float delta = 2.0f * cluster_weights[current_label];
                delta -= resolution * item_weight_i * (user_cluster_weights[current_label] - user_weight_i);
                delta -= resolution * user_weight_i * (item_cluster_weights[current_label] - item_weight_i);

                int best_label = current_label;
                float max_delta_gain = 0.0f;

                // --- 2.3 遍历所有邻居社区，寻找最佳移动目标 ---
                for (int target_label : neighbor_labels) {
                    // 对应 Cython: delta_local = 2 * cluster_weights[label_target] ...
                    float delta_gain = 2.0f * cluster_weights[target_label];
                    delta_gain -= resolution * item_weight_i * user_cluster_weights[target_label];
                    delta_gain -= resolution * user_weight_i * item_cluster_weights[target_label];
                    delta_gain -= delta; // 这是原始代码中的优化技巧

                    if (delta_gain > max_delta_gain) {
                        max_delta_gain = delta_gain;
                        best_label = target_label;
                    }
                }

                // --- 2.4 如果找到了更好的社区，则移动节点 ---
                if (best_label != current_label) {
                    increase_pass += max_delta_gain;

                    // 更新社区权重
                    user_cluster_weights[current_label] -= user_weight_i;
                    item_cluster_weights[current_label] -= item_weight_i;
                    user_cluster_weights[best_label] += user_weight_i;
                    item_cluster_weights[best_label] += item_weight_i;

                    // 更新节点的标签
                    labels[x] = best_label;
                }
            }

            // --- 2.5 清理临时权重 ---
            // 无论节点是否移动，都需要重置本次计算使用的 cluster_weights
            cluster_weights[current_label] = 0.0f;
            for (int target_label : neighbor_labels) {
                cluster_weights[target_label] = 0.0f;
            }
        }

        // --- 3. 检查是否停止迭代 ---
        // cout << increase_pass << endl;
        if (increase_pass <= tol_optimization) {
            stop = true;
        }
    }
    unordered_map<uint, vector<uint>> Sv;
    for (int i=num_user;i<n;i++)
    {
        Sv[labels[i]].push_back(i - num_user);
    }
    cout << "label count:" << Sv.size() << endl;
    int reindex = 0;
    for (auto [l,S]:Sv)
    {
        // naive
        for (auto v:S)
        {
            this->mapIndex[v] = reindex++;
            this->oriIndex[this->mapIndex[v]] = v;
        }
        // degree descend
        // vector<pair<uint,uint>> deg(S.size());
        // for (int i = 0; i < S.size(); ++i)
        // {
        //     deg[i] = {this->getVDeg(S[i]),S[i]};
        // }
        // sort(deg.begin(), deg.end());
        // for (auto [d, v]:deg)
        // {
        //     this->mapIndex[v] = reindex++;
        //     this->oriIndex[this->mapIndex[v]] = v;
        // }
    }
}
void Graph::TestOrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    // count sort
    std::vector<int> deg(num_item);
    int max_deg = 0;
    for (int i = 0; i < num_item; ++i) {
        deg[i] = getVDeg(i);
        max_deg = std::max(max_deg, deg[i]);
        this->oriIndex[i] = i;
    }
    {
        cout << "Fast sort" << endl;
        sort(this->oriIndex.begin(), this->oriIndex.end(), [&](uint a, uint b){return deg[a] > deg[b];});
        for (int i = 0; i < num_item; ++i)
        {
            this->mapIndex[this->oriIndex[i]] = i;
        }
    }
}
void Graph::BlockOrder()
{
    this->Flag_Reo = true;
    uint num_item = this->m_nv;
    uint num_user = this->m_nu;
    this->mapIndex = vector<uint>(num_item, 0);
    this->oriIndex = vector<uint>(num_item, 0);

    const int blk=4, maxWid=32;
    // priority_queue<pair<int,uint>,vector<pair<int,uint>>, greater<pair<int,uint>>>Q;
    priority_queue<pair<int,uint>>Q;
    for (int u=0; u < num_user; u++)
    {
        sort(m_uedges[u].begin(), m_uedges[u].end(), [&](uint vi, uint vj){return m_vdeg[vi] > m_vdeg[vj];});
        Q.push({m_udeg[u],u});
    }
    vector<uint> point(num_user,0);
    vector<bool> visited(num_item,false);
    int usedIndex = 0;
    for (int c=0; c<(m_nv+maxWid-1)/maxWid;c++)
    {
        set<int>S;
        while (!Q.empty() && S.size() < maxWid)
        {
            auto [len,u] = Q.top();
            Q.pop();
            int add=0;
            while (S.size() < maxWid && point[u] < m_udeg[u])
            {
                int v= m_uedges[u][point[u]++];
                if (visited[v]) continue;
                visited[v] = true;
                S.insert(v);
                // cout <<"v = "<<v<<endl;
                add++;
                if (add >= blk)break;
            }
            if (point[u] < m_udeg[u]) Q.push({m_udeg[u]-point[u],u});
        }
        for (auto v:S)
        {
            this->mapIndex[v] = usedIndex++;
            this->oriIndex[this->mapIndex[v]] = v;
        }
        // cout <<S.size()<<endl;
    }
    if (usedIndex != num_item)
    {
        cout << usedIndex << endl;
        assert(0);
    }
    // for (int i = 0; i < num_item; ++i)
    // {
    //     this->oriIndex[this->mapIndex[i]] = i;
    // }
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