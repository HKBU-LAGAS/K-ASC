#ifndef UTILS_H
#define UTILS_H
#include<iostream>
#include<vector>
#include<string>
#include<assert.h>
#include <algorithm>    // std::find
#include <ctime>
#include <chrono>
#include <fstream>
#include <queue>
#include <cstring>
#include <math.h>       /* log */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <random>
#include <iomanip>
#include <sys/stat.h>
#include <map>
#include <unordered_map>
#include <boost/program_options.hpp>
#include <bitset>
#include <stack>
#include <ankerl/unordered_dense.h>

using namespace std;

typedef unsigned int uint;
typedef unsigned char uint8;
typedef long long int64;
typedef unsigned long long uint64;
typedef std::pair<int, int> ipair;
typedef std::pair<double, double> dpair;
#define MP std::make_pair

#ifndef TIMES_PER_SEC
#define TIMES_PER_SEC (1.0e9)
#endif

#define SIZE(t) (int)(t.size())
#define ALL(t) (t).begin(), (t).end()
#define FOR(i, n) for(int (i)=0; (i)<((int)(n)); (i)++)

const std::string Exact = "Exact";
const std::string Truncated = "Truncated";
const std::string NaiveMC = "NaiveMC";
const std::string GNS = "GNS";
const std::string GSS = "GSS";
const std::string GWS = "GWS";
const std::string ASC = "ASC";
const std::string KASC = "KASC";
const std::string UNS = "UNS";

const std::string AdGWS = "AdGWS";
const std::string AdGWS2 = "AdGWS2";
const std::string AdGWS4 = "AdGWS4";
const std::string AdExact = "AdExact";
const std::string AdGNS = "AdGNS";

const std::string ComPare = "ComPare";
const std::string FastExact = "FastExact";
const std::string FastReorder = "FastReorder";
const std::string LvReo = "LvReo";
const std::string MixSet = "MixSet";
const std::string DouBucket = "DouBucket";
const std::string DBMC = "DBMC";
const std::string MonteCarlo = "MonteCarlo";
const std::string ExtMC = "ExtMC";
const std::string GroupMC = "GroupMC";
const std::string FastMC = "FastMC";
const std::string CorMC = "CorMC";
const std::string CompIns = "CompIns";
const std::string Test = "Test";
const std::string Record = "Record";
const std::string Truth = "Truth";
const std::string CutMC = "CutMC";
const std::string Topk = "Topk";
const std::string Ufilter = "Ufilter";
const std::string FastAdap = "FastAdap";
const std::string Wedge = "Wedge";
const std::string WedgeMix = "WedgeMix";
const std::string PostRefine = "PostRefine";
const std::vector<std::string> Algos = {
    Exact, Truncated, NaiveMC, GNS, ASC, KASC,
    GWS, GSS,  UNS, AdExact, AdGNS, AdGWS, AdGWS2,AdGWS4,
    MonteCarlo, FastMC, Truth, FastExact,  FastAdap, Wedge, FastReorder, CompIns, Record,
};

void save_csv_row(const std::string& filename, const std::vector<std::string>& header, const std::vector<std::string>& row);
const int hotTHR=2048;

struct Config{
    std::string strFolder;
    std::string strGraph;
    std::string strAlgo;
    std::string saveFolder;
    std::string freqRatio;
    double alpha;
    double epsilon;
    int queryCount;
    double SampleRatio;
    double SamplePower;
    int maxUserPerItem;
    int maxClickPerUser;
    int maxTopk;
    int ext_k;
    int beta;
    int UfilterLim;
    double probf;
    string ReOrderType;
    string PostRefineType;
    bool FSI_opt;
    bool ValueRecord;
    double AdaptRho;
    double Lambda;
    double PostRefineSigma;
    int Prepare;
    double CompRatio;
    bool DeBug;
    double eps;
    int seedLimit;
    bool NoRefine;
    int RfWid;

    Config(int argc, char** argv){
        namespace po = boost::program_options;
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("data-folder,f", po::value<string>()->default_value("data"), "graph data folder")
            ("graph-name,g", po::value<string>()->required(), "graph file name")
            ("algo,a", po::value<string>()->required(), "algorithm name")
            ("alpha", po::value<double>()->default_value(1.0), "alpha")
            ("beta", po::value<int>()->default_value(1), "beta")
            ("eps", po::value<double>()->default_value(0.8), "eps")
            ("epsilon,e", po::value<double>()->default_value(0.05), "epsilon")
            ("probf", po::value<double>()->default_value(0.0001), "probf")
            ("freqRatio,r",po::value<string>()->default_value("all"), "frequency ratio")
            ("saveFolder,s", po::value<string>()->default_value("results"), "folder to save results")
            ("queryCount", po::value<int>()->default_value(1000), "number of queries")
            ("SampleRatio", po::value<double>()->default_value(1.0), "sample ratio")
            ("SamplePower", po::value<double>()->default_value(1.0), "sample power")
            ("maxUserPerItem", po::value<int>()->default_value(600), "max number of user of per item")
            ("maxClickPerUser",po::value<int>()->default_value(600), "max number of click of per user")
            ("maxTopk", po::value<int>()->default_value(20), "max number of topk")
            ("ExtraCandidate", po::value<int>()->default_value(5), "Extra Candidate number")
            ("ReOrderType", po::value<string>()->default_value("Sample"), "reorder type")
            ("PostRefineType", po::value<string>()->default_value("VarBound"), "Post Refine type")
            ("FSI_opt", po::value<bool>()->default_value(true), "Fast Set Intersection option")
            ("ValueRecord", po::value<bool>()->default_value(false), "Value record")
            ("AdaptRho", po::value<double>()->default_value(0.5), "Parameter for adaptive awareness")
            ("Lambda", po::value<double>()->default_value(0.02), "Lambda")
            ("PostRefineSigma", po::value<double>()->default_value(0.1), "Post Refine sigma")
            ("Prepare", po::value<bool>()->default_value(false), "Prepare")
            ("DeBug", po::value<bool>()->default_value(false), "Debug")
            ("SeedLimit", po::value<int>()->default_value(-1), "Seed limit")
            ("NoRefine", po::value<bool>()->default_value(false), "NoRefine")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc),  vm); // can throw
        po::notify(vm);

        if (vm.count("help")){
            cout << desc << '\n';
            exit(0);
        }
        if (vm.count("data-folder")){
            this->strFolder = vm["data-folder"].as<string>();
        }
        if (vm.count("graph-name")){
            this->strGraph = vm["graph-name"].as<string>();
        }
        if (vm.count("alpha")){
            this->alpha = vm["alpha"].as<double>();
        }
        if (vm.count("algo")){
            this->strAlgo = vm["algo"].as<string>();
        }
        if (vm.count("eps")){
            this->eps = vm["eps"].as<double>();
        }
        if (vm.count("epsilon")){
            this->epsilon = vm["epsilon"].as<double>();
        }
        if (vm.count("probf"))
        {
            this->probf = vm["probf"].as<double>();
        }
        if (vm.count("freqRatio")){
            this->freqRatio = vm["freqRatio"].as<string>();
        }
        if (vm.count("saveFolder")){
            this->saveFolder = vm["saveFolder"].as<string>();
        }
        if (vm.count("queryCount"))
        {
            this->queryCount = vm["queryCount"].as<int>();
        }
        if (vm.count("SampleRatio"))
        {
            this->SampleRatio = vm["SampleRatio"].as<double>();
        }
        if (vm.count("SamplePower"))
        {
            this->SamplePower = vm["SamplePower"].as<double>();
        }
        if (vm.count("maxUserPerItem"))
        {
            this->maxUserPerItem = vm["maxUserPerItem"].as<int>();
        }
        if (vm.count("maxClickPerUser"))
        {
            this->maxClickPerUser = vm["maxClickPerUser"].as<int>();
        }
        if (vm.count("maxTopk"))
        {
            this->maxTopk = vm["maxTopk"].as<int>();
        }
        if (vm.count("ExtraCandidate"))
        {
            this->ext_k = vm["ExtraCandidate"].as<int>();
        }
        if (vm.count("UfilterLim"))
        {
            this->UfilterLim = vm["UfilterLim"].as<int>();
        }
        if (vm.count("ReOrderType"))
        {
            this->ReOrderType = vm["ReOrderType"].as<string>();
        }
        if (vm.count("PostRefineType"))
        {
            this->PostRefineType = vm["PostRefineType"].as<string>();
        }
        if (vm.count("beta"))
        {
            this->beta = vm["beta"].as<int>();
        }
        if (vm.count("FSI_opt"))
        {
            this->FSI_opt = vm["FSI_opt"].as<bool>();
        }
        if (vm.count("ValueRecord"))
        {
            this->ValueRecord = vm["ValueRecord"].as<bool>();
        }
        if (vm.count("AdaptRho"))
        {
            this->AdaptRho = vm["AdaptRho"].as<double>();
        }
        if (vm.count("Lambda"))
        {
            this->Lambda = vm["Lambda"].as<double>();
        }
        if (vm.count("PostRefineSigma"))
        {
            this->PostRefineSigma = vm["PostRefineSigma"].as<double>();
        }
        if (vm.count("Prepare"))
        {
            this->Prepare = vm["Prepare"].as<bool>();
        }
        if (vm.count("DeBug"))
        {
            this->DeBug = vm["DeBug"].as<bool>();
        }
        if (vm.count("SeedLimit"))
        {
            this->seedLimit = vm["SeedLimit"].as<int>();
        }
        if (vm.count("NoRefine"))
        {
            this->NoRefine = vm["NoRefine"].as<bool>();
        }
        if (vm.count("RfWid"))
        {
            this->RfWid = vm["RfWid"].as<int>();
        }
    }

    void display(){
        std::cout << "====================Configurations==================" << std::endl;
        std::cout << "data folder: " << strFolder << '\n';
        std::cout << "graph file name: " << strGraph << '\n';
        std::cout << "algorithm: " << strAlgo << '\n';
        std::cout << "absolute/relative error epsilon: " << epsilon << '\n';
        std::cout << "alpha: " << alpha << '\n';
        std::cout << "fail probability: " << probf << '\n';
        std::cout << "freqRatio: " << freqRatio << '\n';
        std::cout << "queryCount: " << queryCount << '\n';
        std::cout << "saveFolder: " << saveFolder << '\n';
        std::cout << "ReOrderType: " << ReOrderType << '\n';
        std::cout << "FSI_opt: " << FSI_opt << '\n';
        std::cout << "ValueRecord: " << ValueRecord << '\n';
        std::cout << "AdaptRho: " << AdaptRho << '\n';
        if (strAlgo == ASC)
        {
            std::cout << "Lambda: " << Lambda << std::endl;
            std::cout << "PostRefineType: " << PostRefineType << std::endl;
            std::cout << "AdaptRho: " << AdaptRho << std::endl;
            std::cout << "ExtraCandidate: " << ext_k << std::endl;
        }
        std::cout << "====================Configurations==================" << std::endl;
    }
    void check(){
        auto f = std::find(Algos.begin(), Algos.end(), strAlgo);
        assert (f != Algos.end());
    }
};

void process_mem_usage(double& vm_usage, double& resident_set);
void disp_mem_usage();

uint getProcMemory();

class Timer {
public:
    static std::vector<double> timeUsed;
    static std::vector<string> timeUsedDesc;
    int id;
    std::chrono::steady_clock::time_point startTime;
    bool showOnDestroy;

    Timer(int id, string desc = "", bool showOnDestroy = false) {
        this->id = id;
        while ((int) timeUsed.size() <= id) {
            timeUsed.push_back(0);
            timeUsedDesc.push_back("");
        }
        timeUsedDesc[id] = desc;
        startTime = std::chrono::steady_clock::now();
        this->showOnDestroy = showOnDestroy;
    }

    static int length()
    {
        return timeUsed.size();
    }
    static string des(int id) {
        return timeUsedDesc[id];
    }
    static double used(int id) {
        return timeUsed[id] / TIMES_PER_SEC;
    }

    double cost() {
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count();
        return duration;
    }

    void add(int id, long duration)
    {
        timeUsed[id] += duration;
    }

    ~Timer() {
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count();
        if (showOnDestroy) {
            std::cout << "time spend on " << timeUsedDesc[id] << ":" << duration / TIMES_PER_SEC << "s" << std::endl;
        }
        timeUsed[id] += duration;
    }

    static void show(int count=1, bool debug = false) {
        cout << "##### Timer #####" << endl;
        // cout << "length: " << length() << endl;
        cout <<"query count: " << count << '\n';
        for (int i = 0; i < (int) timeUsed.size(); i++) {
            if (timeUsed[i] > 0) {
                char str[100];
                sprintf(str, "%.6lf", timeUsed[i] / count / TIMES_PER_SEC);
                string s = str;
                if ((int) s.size() < 15) s = " " + s;
                char t[100];
                memset(t, 0, sizeof t);
                sprintf(t, "%4d %s %s", i, s.c_str(), timeUsedDesc[i].c_str());
                cout << t << endl;
            }
        }
    }

    // 保存到文件
    static void save(int query_count, const string &saveFolder, std::vector<std::string> resultHeader, std::vector<std::string> resultRow){
        show(query_count);
        string filename = saveFolder + "-TimeCost.csv";
        for (int i = 0; i < (int) timeUsed.size(); i++)
        {
            if (timeUsed[i] == 0) continue;
            resultHeader.push_back(timeUsedDesc[i]);
            if (timeUsedDesc[i] =="Preprocess") resultRow.push_back(to_string(used(i)));
            else resultRow.push_back(to_string(used(i)/query_count));
        }
        save_csv_row(filename, resultHeader, resultRow);
    }

    static void reset(int id){
        if(id>=0 && id<timeUsed.size()){
            timeUsed[id] = 0;
        }
    }

    static void clearAll() {
        timeUsed.clear();
        timeUsedDesc.clear();
    }
};

class Accumulator {
public:
    static std::vector<double> values;         // 各累加器数值
    static std::vector<std::string> descs;     // 描述

    // 确保id存在
    static void ensureId(int id, const std::string& desc = "") {
        while ((int)values.size() <= id) {
            values.push_back(0);
            descs.push_back("");
        }
        if (!desc.empty()) descs[id] = desc;
    }

    // 累加
    static void add(int id, double v, const std::string& desc = "") {
        ensureId(id, desc);
        values[id] += v;
    }

    // size
    static int length()
    {
        return values.size();
    }
    // 取值
    static double get(int id) {
        ensureId(id);
        return values[id];
    }
    static string get_des(int id) {
        ensureId(id);
        return descs[id];
    }

    // 显示全部
    static void show(int count=1) {
        std::cout << "##### Accumulator #####" << std::endl;
        for (int i = 0; i < (int)values.size(); ++i) {
            if (values[i] != 0) {
                std::cout << std::setw(3) << i << ": " << std::setw(12) << values[i] / count
                          << "   " << descs[i] << std::endl;
            }
        }
    }

    // 保存到文件
    static void save(int query_count, const string &saveFolder, const Config &config, std::vector<std::string> header,
        std::vector<std::string> write_row)
    {
        show(query_count);
        string filename = saveFolder + "-count.csv";
        // vector<std::string> header = {"Algo","dataset","most freq"};
        // vector<std::string> write_row = {config.strAlgo, config.strGraph, config.freqRatio};
        for (int i=0;i<length();i++)
        {
            header.push_back(get_des(i));
            write_row.push_back(to_string(get(i)/query_count));
        }
        save_csv_row(filename, header, write_row);
    }

    // 清零某个
    static void reset(int id) {
        if (id >= 0 && id < (int)values.size())
            values[id] = 0;
    }

    // 清空全部
    static void clearAll() {
        values.clear();
        descs.clear();
    }
};

const int VectorDefaultSize=20;

static uint32_t g_seed;
void fastSrand();
uint32_t fastRand();

static uint32_t x_state;
void xorshifinit();
uint32_t xorshift32(void);
bool file_exists(const std::string& filename);

void make_dir(const std::string& dir_path, bool clear_if_exists=false);
void make_dir_for_file(const std::string& file_path);
double EmpiricalFun(int n, double d, double e, double p);
vector<double> calc_AvgError(const std::vector<double>& est_lue, const std::vector<pair<int,double>>& ground_truth, const std::vector<int>& k_list);
vector<double> calc_NDCG(const std::vector<int>& est_lue, const std::vector<std::pair<int, double>>& ground_truth, const std::vector<int>& k_list, int ext_k=0);
vector<double> calc_Recall(const std::vector<int>& est_lue, const std::vector<std::pair<int, double>>& ground_truth, const std::vector<int>& k_list, int ext_k=0);
vector<double> calc_Precision(const std::vector<int>& est_lue, const std::vector<pair<int,double>>& ground_truth, const std::vector<int>& k_list, int ext_k=0);
vector<pair<int,double>> read_ground_truth(const std::string& filename);

class Alias{
public:
    double* p;
    // double* w;
    uint* h;
    uint* map;
    int n;
    Alias(vector<pair<uint, uint> > pi){
        double sum = 0;
        n = pi.size();
        stack<uint> small;
        stack<uint> big;
        p = new double[n];
        // w = new double[n];
        h = new uint[n];
        map = new uint[n];
        for(int i = 0; i < n; i++){
            sum += pi[i].second;
            map[i] = pi[i].first;
        }
        for(int i = 0; i < n; i++){
            p[i] = pi[i].second / sum * n ;
            // w[i] = sum / pi[i].second;
            if(p[i] > 1)
                big.push(i);
            else
                small.push(i);
        }
        while(!small.empty() && !big.empty()){
            uint smallId = small.top();
            small.pop();
            uint bigId = big.top();
            h[smallId] = bigId;
            p[bigId] -= (1-p[smallId]);
            if(p[bigId] < 1){
                small.push(bigId);
                big.pop();
            }
        }
    }

    ~Alias(){
        delete[] p;
        // delete[] w;
        delete[] h;
        delete[] map;
    }

    // 2. 拷贝构造函数 (深拷贝)
    Alias(const Alias& other) : n(other.n) {
        p = new double[n];
        h = new uint[n];
        map = new uint[n];
        // 复制内容，而不是指针地址
        std::copy(other.p, other.p + n, p);
        std::copy(other.h, other.h + n, h);
        std::copy(other.map, other.map + n, map);
    }

    // 3. 拷贝赋值运算符 (深拷贝)
    Alias& operator=(const Alias& other) {
        if (this == &other) { // 防止自我赋值
            return *this;
        }
        // 释放旧资源
        delete[] p;
        delete[] h;
        delete[] map;

        // 分配新资源并拷贝
        n = other.n;
        p = new double[n];
        h = new uint[n];
        map = new uint[n];
        std::copy(other.p, other.p + n, p);
        std::copy(other.h, other.h + n, h);
        std::copy(other.map, other.map + n, map);

        return *this;
    }

    uint generateRandom(std::mt19937& rng){
        std::uniform_real_distribution<> dist_real(0.0, 1.0);
        int firstId = dist_real(rng) * n;
        uint secondId = dist_real(rng) < p[firstId] ? map[firstId] : map[h[firstId]];
        return secondId;
    }
    // pair<uint,double> generateWeightRandom(std::mt19937& rng){
    //     std::uniform_real_distribution<> dist_real(0.0, 1.0);
    //     int firstId = dist_real(rng) * n;
    //     uint secondId = dist_real(rng) < p[firstId] ? map[firstId] : map[h[firstId]];
    //     return {secondId, w[secondId]};
    // }
};

namespace RandomUtils {
    // 声明一个全局的随机数引擎。
    inline std::mt19937 rng(20251013);
}
namespace HashSpace {
    // 声明一个全局的随机数引擎。
    inline uint *pool_mask = NULL;
    inline uint *pool_cnt = NULL;
    inline int Ask_index;
    inline void init(const int num_v)
    {
        Ask_index = 0;
        pool_mask = new uint[num_v];
        pool_cnt = new uint[num_v];
    }
    inline void clear()
    {
        Ask_index++;
    }
    inline void insert(const uint vid)
    {
        if (pool_mask[vid] != Ask_index) pool_cnt[vid] = 0;
        pool_mask[vid] = Ask_index;
        pool_cnt[vid] ++;
    }
    inline bool query(const uint vid)
    {
        return (pool_mask[vid] == Ask_index) && (pool_cnt[vid]--);
    }
}
namespace HashSpace4BSR {
    // 声明一个全局的随机数引擎。
    inline uint *pool_mask = NULL;
    inline uint *pool_state = NULL;
    inline int *pool_cnt = NULL;
    inline int Ask_index;
    inline void init(const int num_v)
    {
        Ask_index = 0;
        pool_mask = new uint[num_v];
        pool_state = new uint[num_v];
        pool_cnt = new int[num_v];
    }
    inline void clear()
    {
        Ask_index++;
    }
    inline void insert(const uint base, const uint& state)
    {
        if (pool_mask[base] != Ask_index) pool_cnt[base] = 0;
        pool_mask[base] = Ask_index;
        pool_state[base] = state;
        pool_cnt[base] ++;
    }
    inline uint query(const uint base)
    {
        if ((pool_mask[base] == Ask_index) && (pool_cnt[base]--)) return pool_state[base];
        else return 0;
    }
}
template<typename T>
std::vector<T> random_sample(const std::vector<T>& source, size_t count) {
    if (count > source.size()) {
        // 根据需要处理错误，可以抛出异常或返回一个空/完整的 vector
        // 这里我们选择返回一个完整的副本
        return source;
    }

    std::vector<T> sampled_elements;

    // 静态的生成器可以避免每次调用都重新播种，提高性能
    // 但在多线程环境下需要使用 thread_local
    // static std::random_device rd;
    static std::mt19937 gen(998244353);

    std::sample(
        source.begin(),
        source.end(),
        std::back_inserter(sampled_elements),
        count,
        gen
    );

    return sampled_elements;
}

struct NeighborIndex {
    enum Type {
        SMALL_LINEAR,
        SORTED_VECTOR,
        FLAT_HASH
    } type = SMALL_LINEAR;

    std::vector<uint> vec;
    ankerl::unordered_dense::set<uint> hset;

    NeighborIndex() = default;

    template <class It>
    NeighborIndex(It first, It last) {
        build(first, last);
    }

    template <class It>
    void build(It first, It last) {
        const size_t dj = std::distance(first, last);

        if (dj <= 32) {
            type = SMALL_LINEAR;
            vec.assign(first, last);
            hset.clear();
        } else if (dj <= 1024) {
            type = SORTED_VECTOR;
            vec.assign(first, last);
            std::sort(vec.begin(), vec.end());
            hset.clear();
        } else {
            type = FLAT_HASH;
            vec.clear();
            hset.clear();
            hset.reserve(dj);
            hset.insert(first, last);
        }
    }

    inline bool contains(uint v) const {
        switch (type) {
        case SMALL_LINEAR:
            for (uint x : vec) {
                if (x == v) return true;
            }
            return false;

        case SORTED_VECTOR:
            return std::binary_search(vec.begin(), vec.end(), v);

        case FLAT_HASH:
            return hset.find(v) != hset.end();
        }
        return false;
    }
};

struct FastRNG {
    uint64_t x = 88172645463325252ull;

    inline uint32_t next() {
        x ^= x << 7;
        x ^= x >> 9;
        return (uint32_t)x;
    }
};

inline uint32_t fast_range(uint32_t x, uint32_t n) {
    return (uint64_t(x) * n) >> 32;
}
#endif