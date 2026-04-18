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
const std::string USS = "USS";
const std::string USS2 = "USS2";
const std::string USS3 = "USS3";
const std::string USS4 = "USS4";
const std::string USS5 = "USS5";
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
    Exact, Truncated, NaiveMC, GNS, ASC, USS, KASC,
    GWS, GSS,  UNS, AdExact, AdGNS, AdGWS, AdGWS2,AdGWS4, USS2, USS3, USS4, USS5,
    MonteCarlo, FastMC, Truth, FastExact,  FastAdap, Wedge, FastReorder, CompIns, Record,
    GroupMC, CorMC, WedgeMix, CutMC, DouBucket, DBMC, LvReo, MixSet, Test, Ufilter, Topk, ExtMC,
    PostRefine, ComPare,
};

void save_csv_row(const std::string& filename, const std::vector<std::string>& header, const std::vector<std::string>& row);
const int hotTHR=2048;

struct Config{
    std::string strFolder;
    std::string strGraph;
    std::string strAlgo;
    std::string saveFolder;
    std::string freqRatio;
    std::string UfilterOpt;
    double alpha;
    double epsilon;
    int queryCount;
    bool NonCold;
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
            ("data-folder,f", po::value<string>()->required(), "graph data folder")
            ("graph-name,g", po::value<string>()->required(), "graph file name")
            ("algo,a", po::value<string>()->required(), "algorithm name")
            ("alpha", po::value<double>()->default_value(1.0), "alpha")
            ("beta", po::value<int>()->default_value(1), "beta")
            ("eps", po::value<double>()->default_value(0.5), "eps")
            ("epsilon,e", po::value<double>()->default_value(0.1), "epsilon")
            ("probf,e", po::value<double>()->default_value(0.0001), "probf")
            ("freqRatio,r",po::value<string>()->default_value("all"), "frequency ratio")
            ("saveFolder,s", po::value<string>()->default_value("results"), "folder to save results")
            ("queryCount", po::value<int>()->default_value(1000), "number of queries")
            ("NonCold", po::value<bool>()->default_value(false), "ignore non-cold query")
            ("SampleRatio", po::value<double>()->default_value(1.0), "sample ratio")
            ("SamplePower", po::value<double>()->default_value(1.0), "sample power")
            ("maxUserPerItem", po::value<int>()->default_value(700), "max number of user of per item")
            ("maxClickPerUser",po::value<int>()->default_value(600), "max number of click of per user")
            ("maxTopk", po::value<int>()->default_value(10), "max number of topk")
            ("UfilterOpt", po::value<string>()->default_value("None"), "User pairs filter option")
            ("ExtraCandidate", po::value<int>()->default_value(5), "Extra Candidate number")
            ("UfilterLim", po::value<int>()->default_value(20), "User filter lower limit")
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
            ("RfWid", po::value<int>()->default_value(5), "Refine window")
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
        if (vm.count("NonCold"))
        {
            this->NonCold = vm["NonCold"].as<bool>();
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
        if (vm.count("UfilterOpt"))
        {
            this->UfilterOpt = vm["UfilterOpt"].as<string>();
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
        std::cout << "NonCold: " << NonCold << '\n';
        std::cout << "ReOrderType: " << ReOrderType << '\n';
        std::cout << "FSI_opt: " << FSI_opt << '\n';
        std::cout << "ValueRecord: " << ValueRecord << '\n';
        std::cout << "AdaptRho: " << AdaptRho << '\n';
        if (strAlgo == MonteCarlo){
            std::cout << "SampleRatio: " << SampleRatio << '\n';
            std::cout << "SamplePower: " << SamplePower << '\n';
        }
        if (strAlgo == Wedge)
        {
            std::cout << "SampleRatio: " << SampleRatio << '\n';
            std::cout << "beta: " << beta << '\n';
        }
        if (strAlgo == Truncated)
        {
            std::cout << "maxUserPerItem: " << maxUserPerItem << '\n';
            std::cout << "maxClickPerUser: " << maxClickPerUser << '\n';
        }
        if (UfilterOpt != "None")
        {
            std::cout << "UfilterOpt: " << UfilterOpt << '\n';
            std::cout << "UfilterLim: " << UfilterLim << '\n';
        }
        if (strAlgo == ExtMC)
        {
            std:: cout << "ExtraCandidate: " << ext_k << std::endl;
        }
        if (strAlgo == PostRefine){
            std:: cout << "ExtraCandidate: " << ext_k << std::endl;
            std:: cout << "PostRefineType: " << PostRefineType << std::endl;
        }
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
    void setDefault(){
        // alpha=1.0;
        // if(epsilon==0){
        //     epsilon=0.5;
        // }
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

template <typename _T>
class iVector
{
public:
    uint m_size;
    _T* m_data;
    uint m_num;

    void free_mem()
    {
        delete[] m_data;
    }

    iVector()
    {
        //printf("%d\n",VectorDefaultSize);
        m_size = VectorDefaultSize;
        m_data = new _T[VectorDefaultSize];
        m_num = 0;
    }
    iVector( uint n )
    {
        if ( n == 0 )
        {
            n = VectorDefaultSize;
        }
//      printf("iVector allocate: %d\n",n);
        m_size = n;
        m_data = new _T[m_size];
        m_num = 0;
    }
    void push_back( _T d )
    {
        // if ( m_num == m_size )
        // {
        //     re_allocate( m_size*2 );
        // }
        m_data[m_num] = d ;
        m_num++;        
    }
    void push_back( const _T* p, uint len )
    {
        while ( m_num + len > m_size )
        {
            re_allocate( m_size*2 );
        }
        memcpy( m_data+m_num, p, sizeof(_T)*len );
        m_num += len;
    }

    void re_allocate( uint size )
    {
        if ( size < m_num )
        {
            return;
        }
        _T* tmp = new _T[size];
        memcpy( tmp, m_data, sizeof(_T)*m_num );
        m_size = size;
        delete[] m_data;
        m_data = tmp;
    }
    void Sort()
    {
        if ( m_num < 20 )
        {
            int k ;
            _T tmp;
            for ( int i = 0 ; i < m_num-1 ; ++i )
            {
                k = i ;
                for ( int j = i+1 ; j < m_num ; ++j )
                    if ( m_data[j] < m_data[k] ) k = j ;
                if ( k != i )
                {
                    tmp = m_data[i];
                    m_data[i] = m_data[k];
                    m_data[k] = tmp;
                }
            }
        }
        else sort( m_data, m_data+m_num );
    }
    void unique()
    {
        if ( m_num == 0 ) return;
        Sort();
        uint j = 0;
        for ( uint i = 0 ; i < m_num ; ++i )
            if ( !(m_data[i] == m_data[j]) )
            {
                ++j;
                if ( j != i ) m_data[j] = m_data[i];
            }
        m_num = j+1;
    }
    int BinarySearch( _T& data )
    {
        for ( int x = 0 , y = m_num-1 ; x <= y ; )
        {
            int p = (x+y)/2;
            if ( m_data[p] == data ) return p;
            if ( m_data[p] < data ) x = p+1;
            else y = p-1;
        }
        return -1;
    }
    void clean()
    {
        m_num = 0;
    }
    void assign( iVector& t )
    {
        m_num = t.m_num;
        m_size = t.m_size;
        delete[] m_data;
        m_data = t.m_data;
    }

    bool remove( _T& x )
    {
        for ( int l = 0 , r = m_num ; l < r ; )
        {
            int m = (l+r)/2;

            if ( m_data[m] == x )
            {
                m_num--;
                if ( m_num > m ) memmove( m_data+m, m_data+m+1, sizeof(_T)*(m_num-m) );
                return true;
            }
            else if ( m_data[m] < x ) l = m+1;
            else r = m;
        }
        return false;
    }

    void sorted_insert( _T& x )
    {
        if ( m_num == 0 )
        {
            push_back( x );
            return;
        }

        if ( m_num == m_size ) re_allocate( m_size*2 );

        int l,r;

        for ( l = 0 , r = m_num ; l < r ; )
        {
            int m = (l+r)/2;
            if ( m_data[m] < x ) l = m+1;
            else r = m;
        }

        if ( l < m_num && m_data[l] == x )
        {
            //printf("Insert Duplicate....\n");
            //cout<<x<<endl;
    //      break;
        }
        else
        {
            if ( m_num > l )
            {
                memmove( m_data+l+1, m_data+l, sizeof(_T)*(m_num-l) );
            }
            m_num++;
            m_data[l] = x;
        }
    }

    bool remove_unsorted( _T& x )
    {
        for ( int m = 0 ; m < m_num ; ++m )
        {
            if ( m_data[m] == x )
            {
                m_num--;
                if ( m_num > m ) memcpy( m_data+m, m_data+m+1, sizeof(_T)*(m_num-m) );
                return true;
            }
        }
        return false;
    }

    _T& operator[]( uint i )
    {
        //if ( i < 0 || i >= m_num ) 
        //{
        //  printf("iVector [] out of range!!!\n");
        //}
        return m_data[i];
    }
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //close range check for [] in iVector if release

};

template <typename _T>
struct iMap
{   
    _T* m_data;
    int m_num;
    int cur;
    iVector<int> occur;
    _T nil; 
    iMap()
    {
        m_data = NULL;
        m_num = 0;
        //nil = std::make_pair((long)-9,(long)-9);
        //nil = 1073741834;
    }
    iMap(int size){
        initialize(size);
    }
    void free_mem()
    {
        delete[] m_data;
        occur.free_mem();
    }

    void initialize( int n )
    {
        occur.re_allocate(n);
        occur.clean();
        m_num = n;
        nil = -9;
        if ( m_data != NULL )
            delete[] m_data;
        m_data = new _T[m_num];
        for ( int i = 0 ; i < m_num ; ++i )
            m_data[i] = nil;
        cur = 0;
    }
    void clean()
    {
        for ( int i = 0 ; i < occur.m_num ; ++i ){
            m_data[occur[i]] = nil;
        }
        // occur.clean();
        occur.m_num = 0;
        cur = 0;
    }
    
    //init keys 0-n, value as 0
    void init_keys(int n){
        occur.re_allocate(n);
        occur.clean();
        m_num = n;
        nil = -9;
        if ( m_data != NULL )
            delete[] m_data;
        m_data = new _T[m_num];
        for ( int i = 0 ; i < m_num ; ++i ){
            m_data[i] = 0;
            occur.push_back( i );
            cur++;
        }
    }
    //reset all values to be zero
    void reset_zero_values(){
        // for ( int i = 0 ; i < m_num ; ++i )
            // m_data[i] = 0.0;
        memset( m_data, 0.0, m_num*sizeof(_T) );
    }

    void reset_one_values(){
        for ( int i = 0 ; i < m_num ; ++i )
            m_data[i] = 1.0;
        // memset( m_data, 0.0, m_num*sizeof(_T) );
    }

    _T get( int p )
    {
        //if ( p < 0 || p >= m_num ) 
        //{
        //  printf("iMap get out of range!!!\n");
        //  return -8;
        //}
        return m_data[p];
    }
    _T& operator[](  int p )
    {
        //if ( i < 0 || i >= m_num ) 
        //{
        //  printf("iVector [] out of range!!!\n");
        //}
        return m_data[p];
    }
    void erase( int p )
    {
        //if ( p < 0 || p >= m_num ) 
        //{
        //  printf("iMap get out of range!!!\n");
        //}
        m_data[p] = nil;
        cur--;
    }
    bool notexist( int p )
    {
        return m_data[p] == nil ;
    }
    bool exist( int p )
    {
        return !(m_data[p] == nil);
    }
    void insert( int p , _T d )
    {
        //if ( p < 0 || p >= m_num ) 
        //{
        //  printf("iMap insert out of range!!!\n");
        //}
        if ( m_data[p] == nil )
        {
            occur.push_back( p );
            cur++;
        }
        m_data[p] = d;
    }
    void inc( int p , _T x )
    {
        if ( m_data[p] == nil ){
            insert(p, x);
        }
        else{
            m_data[p] += x;
        }
    }
    void dec( int p )
    {
        //if ( m_data[p] == nil )
        //{
        //  printf("dec some unexisted point\n" );
        //}
        m_data[p]--;
    }
    //close range check when release!!!!!!!!!!!!!!!!!!!!    
};

template <typename _T>
struct myMap
{   
    _T* m_data;
    int* m_keys;
    long m_num;
    long cur;
    _T nil; 
    myMap(){
        m_data = NULL;
        m_num = 0;
        cur = 0;
    }
    myMap(long size){
        initialize(size);
    }
    void free_mem(){
        delete[] m_data;
        delete[] m_keys;
        m_num = 0;
        cur = 0;
    }

    void initialize( long n ){
        m_num = n;
        nil = 0;
        if ( m_data != NULL )
            delete[] m_data;
        if( m_keys != NULL )
            delete[] m_keys;
        m_data = new _T[m_num];
        m_keys = new int[m_num];
        for ( long i = 0 ; i < m_num ; ++i )
            m_data[i] = nil;
        cur = 0;
    }
    void clean(){
        for(long i=0; i<cur; i++){
            m_data[m_keys[i]]=nil;
        }
        cur = 0;
    }

    _T get( int p ){
        return m_data[p];
    }

    int getid( int p ){
        return m_keys[p];
    }

    _T& operator[](  int p ){
        return m_data[p];
    }

    bool notexist( int p ){
        return !m_data[p];
    }

    void insert( int p , _T d ){
        if(!m_data[p]){
            m_keys[cur]=p;
            cur++;
        }
        m_data[p] = d;
    }

    void inc( int p , _T x ){
        if (!m_data[p]){
            m_keys[cur]=p;
            m_data[p] = x;
            cur++;
        }
        else{
            m_data[p] += x;
        }
    }
};

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

struct TopkHeap
{
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> minHeap;
    unordered_map<int, double> value_map;
    int Topk;
    TopkHeap(int topk) : Topk(topk) {}

    void push(int item, double value)
    {
        value_map[item] = value;
        minHeap.push({value, item});  // 始终插入新值

        // 堆大小超过Topk时，弹出无效项
        while (value_map.size() > Topk)
        {
            auto [min_value, min_item] = minHeap.top();
            if (value_map[min_item] != min_value)
            {
                // 堆顶是过期数据，弹出
                minHeap.pop();
            }
            else
            {
                // 堆顶是有效数据，弹出并同步 value_map
                value_map.erase(min_item);
                minHeap.pop();
            }
        }
        if (minHeap.size() > 10 * Topk)
        {
            // 重新构造堆对象
            minHeap = priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>();
            for (const auto& [it, val] : value_map)
            {
                minHeap.push({val, it});
            }
        }
    }
};

struct MyHeap
{
    int Topk;
    set<pair<double, uint>> H;
    set<pair<double, uint>> Act;
    map<uint, set<pair<double, uint>>::iterator> item_positions;
    MyHeap(int topk) : Topk(topk) {}

    void push(uint item, double value)
    {
        // 检查 item 是否已存在
        auto it = item_positions.find(item);
        if (it != item_positions.end())
        {
            auto set_it = it->second;
            if (H.count(*set_it) > 0) {
                H.erase(set_it);
            } else {
                Act.erase(set_it);
            }
            item_positions.erase(it);
        }

        auto p = make_pair(value, item);
        if (H.size() < Topk)
        {
            auto insert_it = H.insert(p).first;
            item_positions[item] = insert_it;
        }else
        {
            if (value > H.begin()->first)
            {
                auto smallest = *H.begin();
                H.erase(H.begin());
                item_positions.erase(smallest.second);

                auto insert_it_act = Act.insert(smallest).first;
                item_positions[smallest.second] = insert_it_act;

                auto insert_it_h = H.insert(p).first;
                item_positions[item] = insert_it_h;
            }else
            {
                auto inserted_it_act = Act.insert(p).first;
                item_positions[item] = inserted_it_act;
            }
        }
    }
    pair<double,uint> topk(int r) // Top K
    {
        if (H.empty()) return {-1, -1};
        auto v = *H.begin();
        return {v.first / r, v.second};
    }
    pair<double,uint> top(int r) // Top K
    {
        if (H.empty()) return {-1, -1};
        auto v = *H.rbegin();
        return {v.first / r, v.second};
    }
    pair<double,uint> topk1(int r)//Top K+1
    {
        if (Act.empty()) return {-1, -1};
        auto v = *Act.rbegin();
        return {v.first / r, v.second};
    }
};

class BloomFilter {
public:
    // 构造函数
    // m: 位数组的大小 (bits)
    // k: 哈希函数的数量
    BloomFilter(uint64_t m=8, uint8_t k=1) : m_bits(m), m_num_hashes(k) {
        m_bit_array.resize(m / 8 + 1, 0); // 分配字节数组并初始化为0
    }
    void init(uint64_t m, uint8_t k)
    {
        if (m % 8 != 0) {
            m = (m / 8 + 1) * 8;
        }
        m_bits = m;
        m_num_hashes = k;
        m_bit_array.resize(m / 8 + 1, 0); // 分配字节数组并初始化为0
    }
    void fuzzy_init(uint64_t n, double p)
    {
        uint64_t m = static_cast<uint64_t>(-1.0 * n * std::log(p) / (std::log(2) * std::log(2)));
        uint8_t k = static_cast<uint8_t>(std::round((static_cast<double>(m) / n) * std::log(2)));
        if (m % 8 != 0) {
            m = (m / 8 + 1) * 8;
        }
        m_bits = m;
        m_num_hashes = k;
        m_bit_array.resize(m / 8 + 1, 0); // 分配字节数组并初始化为0
    }
    // 添加元素
    void add(const uint64_t& item) {
        for (uint8_t i = 0; i < m_num_hashes; ++i) {
            uint64_t hash_val = hash(item, i);
            // 设置对应的位为1
            m_bit_array[hash_val / 8] |= (1 << (hash_val % 8));
        }
    }
    void add(uint a, uint b) {
        if (a>b) swap(a,b);
        uint64_t item = static_cast<uint64_t>(a) << 32 | b;
        for (uint8_t i = 0; i < m_num_hashes; ++i) {
            uint64_t hash_val = hash(item, i);
            // 设置对应的位为1
            m_bit_array[hash_val / 8] |= (1 << (hash_val % 8));
        }
    }

    // 查询元素是否存在（可能误报）
    bool query(const uint64_t& item) const {
        for (uint8_t i = 0; i < m_num_hashes; ++i) {
            uint64_t hash_val = hash(item, i);
            // 如果某一位是0，则元素肯定不存在
            if (!((m_bit_array[hash_val / 8] >> (hash_val % 8)) & 1)) {
                return false;
            }
        }
        // 所有位都是1，元素可能存在
        return true;
    }
    bool query(uint a, uint b) const {
        if (a>b)swap(a,b);
        uint64_t item = static_cast<uint64_t>(a) << 32 | b;
        for (uint8_t i = 0; i < m_num_hashes; ++i) {
            uint64_t hash_val = hash(item, i);
            // 如果某一位是0，则元素肯定不存在
            if (!((m_bit_array[hash_val / 8] >> (hash_val % 8)) & 1)) {
                return false;
            }
        }
        // 所有位都是1，元素可能存在
        return true;
    }

private:
    uint64_t m_bits;
    uint8_t m_num_hashes;
    std::vector<uint8_t> m_bit_array;

    // 生成k个不同的哈希值
    // 使用简单的双哈希法 (double hashing)
    uint64_t hash(const uint64_t& item, uint8_t i) const {
        // 使用 std::hash 作为基础哈希函数
        static std::hash<uint64_t> hasher;
        uint64_t h1 = hasher(item);
        uint64_t h2 = hasher(h1); // 用第一个哈希值生成第二个
        return (h1 + i * h2) % m_bits;
    }
};
template<size_t m_bits>
class FastBF {
public:
    std::bitset<m_bits> m_bit_array;
    // 构造函数
    // m: 位数组的大小 (bits)
    // k: 哈希函数的数量
    FastBF(uint8_t k=7) : m_num_hashes(k) {
         // 分配字节数组并初始化为0
    }

    // 添加元素
    void add(const uint& item) {
        for (uint8_t i = 0; i < m_num_hashes; ++i) {
            uint64_t hash_val = hash(item, i);
            // 设置对应的位为1
            m_bit_array[hash_val] = 1;
        }
    }
    int count()
    {
        return m_bit_array.count();
    }
    // 查询元素交集
    double Intersection(const FastBF& tmp) const {
        return static_cast<double>((m_bit_array & tmp.m_bit_array).count()) / m_num_hashes;
    }
    double Intersection_corrected(const FastBF& tmp) const {
        return - static_cast<double>(m_bits) / m_num_hashes * std::log(1.0 - static_cast<double>((m_bit_array & tmp.m_bit_array).count()) / m_bits);
    }
    // 并集推交集
    double Intersection_Union_corrected(const FastBF& tmp) const {
        return static_cast<double>(m_bits) / m_num_hashes * std::log((1.0 - static_cast<double>((m_bit_array | tmp.m_bit_array).count()) / m_bits) / (1 - static_cast<double>(m_bit_array.count()) / m_bits) / (1 - static_cast<double>(tmp.m_bit_array.count()) / m_bits));;
    }

private:
    uint8_t m_num_hashes;

    // 生成k个不同的哈希值
    // 使用简单的双哈希法 (double hashing)
    uint64_t hash(const uint& item, uint8_t i) const {
        // 使用 std::hash 作为基础哈希函数
        static std::hash<uint64_t> hasher;
        uint64_t h1 = hasher(item);
        uint64_t h2 = hasher(h1); // 用第一个哈希值生成第二个
        return (h1 + i * h2) % m_bits;
    }
};

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