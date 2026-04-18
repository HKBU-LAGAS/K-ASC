#include <csignal>
#include <limits>
#include <sys/stat.h>
#include "graph.h"
#include "algo.h"
#include <filesystem>
#define DEBUG
// #include "DotHash.h"
using namespace std;
namespace po = boost::program_options;

namespace { 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
} // namespace
// Accumulator static member initialization
std::vector<double> Accumulator::values;
std::vector<std::string> Accumulator::descs;

void quality_metrics(const int& vq, const Graph& graph, const Config& config, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }
    /* node inforamtion */
    results["ID"] = vector<double>({(double)vq});
    results["deg"] = vector<double>({(double) graph.getVDeg(vq)});
    results["degUsed"] = vector<double>({(double) graph.vSorted[vq].size()});
    results["Time"] = vector<double>({time_cost / TIMES_PER_SEC});

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    results["Precision"]=precision;

    vector<double> recall = calc_Recall(est_lue_items, ground_truth, k_list, ext_k);
    results["Recall"]=recall;

    vector<double> ndcg = calc_NDCG(est_lue_items, ground_truth, k_list);
    results["NDCG"]=ndcg;

    vector<double> avg_error = calc_AvgError(real_values, ground_truth, k_list);
    results["AvgError"]=avg_error;

    metric_record.push_back(results);
    return ;
}
void Topk_quality_metrics(const int& vq, const Graph& graph, const Config& config, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, const int RefSize, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }
    /* node inforamtion */
    results["RefSize"] = vector<double>({(double)RefSize});
    results["ID"] = vector<double>({(double)vq});
    results["deg"] = vector<double>({(double) graph.getVDeg(vq)});
    results["degUsed"] = vector<double>({(double) graph.vSorted[vq].size()});
    results["Time"] = vector<double>({time_cost / TIMES_PER_SEC});

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    results["Precision"]=precision;

    vector<double> recall = calc_Recall(est_lue_items, ground_truth, k_list, ext_k);
    results["Recall"]=recall;

    vector<double> ndcg = calc_NDCG(est_lue_items, ground_truth, k_list);
    results["NDCG"]=ndcg;

    vector<double> avg_error = calc_AvgError(real_values, ground_truth, k_list);
    results["AvgError"]=avg_error;

    metric_record.push_back(results);
    return ;
}
void CompIns_quality_metrics(const int& vq, const Graph& graph, const Config& config, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const vector<pair<string,double>>& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }
    /* node inforamtion */
    results["ID"] = vector<double>({(double)vq});
    results["deg"] = vector<double>({(double) graph.getVDeg(vq)});
    for (auto [s, t]: time_cost)
    {
        results[s] = vector<double>({t});
    }

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    results["Precision"]=precision;

    vector<double> recall = calc_Recall(est_lue_items, ground_truth, k_list, ext_k);
    results["Recall"]=recall;

    vector<double> ndcg = calc_NDCG(est_lue_items, ground_truth, k_list);
    results["NDCG"]=ndcg;

    vector<double> avg_error = calc_AvgError(real_values, ground_truth, k_list);
    results["AvgError"]=avg_error;

    metric_record.push_back(results);
    return ;
}
void Extra_quality_metrics(const int& vq, const Graph& graph, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }
    /* node inforamtion */
    results["ID"] = vector<double>({(double)vq});
    results["deg"] = vector<double>({(double) graph.getVDeg(vq)});
    results["Deg"] = vector<double>({(double) graph.getVNDeg(vq)});
    results["Time"] = vector<double>({time_cost / TIMES_PER_SEC});
    results["Rho"] = vector<double>({time_cost / TIMES_PER_SEC / (graph.getVDeg(vq)-1) / graph.getVNDeg(vq)});

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    results["Precision"]=precision;

    vector<double> recall = calc_Recall(est_lue_items, ground_truth, k_list, ext_k);
    results["Recall"]=recall;

    vector<double> ndcg = calc_NDCG(est_lue_items, ground_truth, k_list);
    results["NDCG"]=ndcg;

    vector<double> avg_error = calc_AvgError(real_values, ground_truth, k_list);
    results["AvgError"]=avg_error;

    metric_record.push_back(results);
    return ;
}
void AdGNS_quality_metrics(const int& vq, const Graph& graph, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int n_rounds, double ratio, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }
    /* node inforamtion */
    results["ID"] = vector<double>({(double)vq});
    results["deg"] = vector<double>({(double) graph.getVDeg(vq)});
    results["Deg"] = vector<double>({(double) graph.getVNDeg(vq)});
    results["Time"] = vector<double>({time_cost / TIMES_PER_SEC});
    results["nRounds"] = vector<double>({(double) n_rounds});
    results["Rho"] = vector<double>({time_cost / TIMES_PER_SEC * (graph.getVDeg(vq)-1)  / graph.getVNDeg(vq) / n_rounds});
    results["Ratio"] = vector<double>({ratio});

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    results["Precision"]=precision;

    vector<double> recall = calc_Recall(est_lue_items, ground_truth, k_list, ext_k);
    results["Recall"]=recall;

    vector<double> ndcg = calc_NDCG(est_lue_items, ground_truth, k_list);
    results["NDCG"]=ndcg;

    vector<double> avg_error = calc_AvgError(real_values, ground_truth, k_list);
    results["AvgError"]=avg_error;

    metric_record.push_back(results);
    return ;
}
double Fast_metrics(const int& vq, const Graph& graph, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    /* 计算多种质量指标
     * vq: 查询节点
     * dataset: 数据集名称
     * est_lue_values: 估计结果的value列表，按item索引存储估计值
     * k_list: 需要计算的K值列表
     * 返回: 多种指标的结果拼接在一起的列表
     */
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return 1;

    // 提取估计的topK item列表
    bool reoFlag = graph.getReo();
    vector<int> est_lue_items;
    int topK = 1 + ext_k + *max_element(k_list.begin(), k_list.end());
    vector<double> real_values(est_lue_values.size());
    {
        std::vector<std::pair<double,int>> item_swing; // <swing,item>
        item_swing.reserve(est_lue_values.size());
        for (int i = 0; i < est_lue_values.size(); ++i)
        {
            int ind = reoFlag? graph.oriIndex[i]:i;
            if (ind == vq) continue;
            item_swing.emplace_back(est_lue_values[i], ind);
            real_values[ind] = est_lue_values[i];
        }

        // 排序后取topK
        std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
            std::greater<std::pair<double,int>>());
        for (int i = 0; i < topK; ++i) est_lue_items.push_back(item_swing[i].second);
    }

    /* metric */
    vector<double> precision = calc_Precision(est_lue_items, ground_truth, k_list);
    return *min_element(precision.begin(), precision.end());
}
vector<int> loadSeed(string folder, string data_name, int &count, string freq_str){
    std::string filename = folder + "/" + data_name + "/queries/seeds-" + freq_str + ".txt";
    FILE *fin = fopen(filename.c_str(), "r");
    int s;
    vector<int> seeds;
    int i=0;
    while (fscanf(fin, "%d", &s) != EOF) {
        seeds.push_back(s);
        i++;
        if(i>=count)
            break;
    }
    if (i<count){
        cout << "Warning: only " << i << " seeds loaded, less than queryCount " << count << endl;
        count = i;
    }
    fclose(fin);
    cout << "read seed Done!" << endl;
    return seeds;
}

void store_ground_truth(const int &query_node, int topK, const std::vector<double>& swing, std::string save_path)
{
    make_dir(save_path);
    save_path = save_path + "/" + to_string(query_node) + ".txt";
    std::ofstream fout(save_path);
    if (!fout) { std::cerr << "Cannot open file for writing: " << save_path << std::endl; return; }

    std::vector<std::pair<double,int>> item_swing; // <swing,item>
    for (size_t i = 0; i < swing.size(); ++i)
    {
        if (i == query_node) continue;
        item_swing.emplace_back(swing[i], i);
    }

    // 排序后取topK
    std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
        std::greater<std::pair<double,int>>());

    // 存储格式：item1 swing1 item2 swing2 ...
    // fout << query_node;
    for (int i = 0; i < topK; ++i)
        fout << item_swing[i].second << " " << item_swing[i].first << '\n';
    fout.close();
    // std::cout << "Ground truth saved to " << save_path << std::endl;
}
void view_ground_truth(const int &query_node, int topK, const std::vector<double>& swing, std::string save_path, const Graph& graph)
{
    make_dir(save_path);
    save_path = save_path + "/" + to_string(query_node) + ".txt";
    std::ofstream fout(save_path);
    if (!fout) { std::cerr << "Cannot open file for writing: " << save_path << std::endl; return; }

    std::vector<std::pair<double,int>> item_swing; // <swing,item>
    for (size_t i = 0; i < swing.size(); ++i)
    {
        if (i == query_node) continue;
        item_swing.emplace_back(swing[i], i);
    }

    // 排序后取topK
    std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
        std::greater<std::pair<double,int>>());

    // 存储格式：item1 swing1 item2 swing2 ...
    // fout << query_node;
    for (int i = 0; i < topK; ++i)
        fout << item_swing[i].second << " " << item_swing[i].first << " " << graph.m_vdeg[item_swing[i].second] << '\n';
    fout.close();
    // std::cout << "Ground truth saved to " << save_path << std::endl;
}
vector<string> metric_type = {"Precision", "Recall", "NDCG", "AvgError"};
void Metric_save(string saveFolder, const Config &config, const vector<int>& k_list, const vector<map<string,vector<double>>> &metric_record, vector<string> result_header, vector<string> result_output, double query_count)
{
    // save metrics
    string savefile = saveFolder + "-metric.csv";
    vector<string> metric_header = result_header;
    vector<std::string> output = result_output;
    metric_header.push_back("PreTime");
    output.push_back(to_string(Timer::used(0)));
    metric_header.push_back("InsTime");
    output.push_back(to_string(Timer::used(2)/query_count));
    metric_header.push_back("Time");
    output.push_back(to_string(Timer::used(1)/query_count));

    for (int k=0;k<k_list.size();k++)
        for (auto tp: metric_type)
            metric_header.push_back(tp + '@' + to_string(k_list[k]));

    for (int i=0;i<k_list.size();i++)
    {
        map<string,vector<double>> metric_map;
        for (auto tp:metric_type)
        {
            metric_map[tp] = vector<double>(k_list.size(), 0.0);
        }
        for (auto metric: metric_record)
        {
            for (auto tp:metric_type)
                metric_map[tp][i] += metric[tp][i];
        }

        for (auto tp: metric_type)
        {
            output.push_back(to_string(metric_map[tp][i]/metric_record.size()));
            cout << tp << "@" << k_list[i] << ": " << metric_map[tp][i]/metric_record.size() << "\t";
        }
        cout << endl;
    }
    save_csv_row(savefile, metric_header, output);
    // save each metric record
    string recordfile = saveFolder + "-MetricRecord.csv";
    for (auto metric: metric_record)
    {
        vector<string> metric_header = result_header;
        vector<string> output = result_output;
        for (auto [v,_]:metric)
        {
            if (std::find(metric_type.begin(), metric_type.end(), v) != metric_type.end())continue;
            metric_header.push_back(v);
            if (v == "Time")output.push_back(to_string(metric[v][0]));
            else if (v == "Rho")
            {
                std::stringstream ss;
                ss << std::scientific << std::setprecision(6) << metric[v][0];
                output.push_back(ss.str());
            }else if (v == "SetInt" || v == "SizeInt" || v == "SimpInt")
            {
                std::stringstream ss;
                ss << std::scientific << std::setprecision(6) << metric[v][0];
                output.push_back(ss.str());
            }
            else output.push_back(to_string((int)metric[v][0]));
        }
        // vector<std::string> output = {to_string((int)metric["ID"][0]), to_string((int)metric["ID"][1]), to_string(metric["Time"][0])};

        for (int i=0;i<k_list.size();i++)
        {
            for (auto tp:metric_type)
            {
                metric_header.push_back(tp + '@' + to_string(k_list[i]));
               output.push_back(to_string(metric[tp][i]));
            }
        }

        save_csv_row(recordfile, metric_header, output);
    }
}
void Accumulator_save(int query_count, const string &saveFolder, const Config &config)
{
    Accumulator::show(query_count);
    string filename = saveFolder + "/" + config.strGraph + "-" + config.strAlgo + "-count.csv";
    vector<std::string> header = {"Algo","dataset","most freq"};
    vector<std::string> write_row = {config.strAlgo, config.strGraph, config.freqRatio};
    for (int i=0;i<Accumulator::length();i++)
    {
        header.push_back(Accumulator::get_des(i));
        write_row.push_back(to_string(Accumulator::get(i)/query_count));
    }
    save_csv_row(filename, header, write_row);
}
void preprocess_BloomFilter(const Graph& graph, const BPMaximalClique& fast_set, BloomFilter& bf) {
    std::cout << "Starting preprocessing of valid user pairs..." << std::endl;

    uint num_users = graph.getNu();
    vector<uint> users(num_users);
    vector<uint> rank(num_users);
    for (int i=0;i<num_users;i++) users[i] = i;
    sort(users.begin(),users.end(),[&](int i, int j){return graph.getUDeg(i) < graph.getUDeg(j);});
    for (uint i=0;i<num_users;i++)
    {
        rank[users[i]]=i;
    }
    vector<uint> valid_pairs(num_users);
    uint64_t optimal = 0;
    for (uint i=0;i<num_users;i++)
    {
        uint u1 = users[i];
        if (graph.getUDeg(u1) <= 20) continue;
        uint64_t dsum = 0;
        for (auto v:graph.m_uedges[u1])
        {
           dsum += graph.getVDeg(v);
        }
        if (dsum < num_users - i)
        {
            unordered_map<uint,uint> user2pair;
            for (auto v:graph.m_uedges[u1])
            {
                for (auto u2:graph.m_vedges[v])
                {
                    user2pair[u2] += 1;
                }
            }
            for (auto [u2,c]:user2pair)
            {
                if (c>=2)
                {
                    if (rank[u2] > i)
                    {
                        bf.add(u1,u2);
                        valid_pairs[u1] += 1;
                        valid_pairs[u2] += 1;
                    }
                }
            }
        }else
        {
            for (uint j = i+1;j<num_users;j++)
            {
                uint u2 = users[j];
                vector<uint> C;
                fast_item_intersection(fast_set, graph, C, u1,u2);
                if (C.size()>=2) bf.add(u1,u2);
                valid_pairs[u1] += 1;
                valid_pairs[u2] += 1;
            }
        }
        optimal += min(dsum, static_cast<uint64_t>(num_users-i));
    }
    int64_t valid_sum = 0;
    for (uint i=0;i<num_users;i++)
    {
        uint u = users[i];
        valid_sum += valid_pairs[u];
        save_csv_row("results/Ufilter/state/"+graph.m_graph+"/pairs.csv",{"degree","valid"},{to_string(graph.getUDeg(u)),to_string(valid_pairs[u])});
    }
    // cout << "optimal = " << total << " " << "square = " << static_cast<uint64_t>(num_users) * num_users <<'\n';
    save_csv_row("results/Ufilter/state/statics.csv",{"dataset","optimal","square","valid sum"},{graph.m_graph,to_string(optimal), to_string(static_cast<uint64_t>(num_users) * num_users), to_string(valid_sum)});

    std::cout << "Preprocessing finished." << std::endl;
}
void preprocess_BFset(const Graph& graph, const BPMaximalClique& fast_set, vector<FastBF<1024>>& bf) {
    std::cout << "Starting preprocessing of valid user pairs..." << std::endl;

    uint num_users = graph.getNu();

    vector<int> cnt;
    for (uint i=0;i<num_users;i++)
    {
        if (graph.getUDeg(i) > 100) continue;

        for (auto v:graph.m_uedges[i])
        {
            bf[i].add(v);
        }
        cnt.push_back(bf[i].count());
    }

    save_csv_row("results/FastBF/state/statics.csv",{"dataset","min","max","mean"},{graph.m_graph,to_string(*std::min_element(cnt.begin(), cnt.end())), to_string(*std::max_element(cnt.begin(), cnt.end())), to_string(cnt.empty() ? 0.0 : std::accumulate(cnt.begin(), cnt.end(), 0.0) / cnt.size())});

    std::cout << "Preprocessing finished." << std::endl;
}
int Debug_Insc(const Graph& graph, int v1, int v2)
{
    unordered_set<uint>S1(graph.m_vedges[v1].begin(), graph.m_vedges[v1].end());
    unordered_set<uint>S2(graph.m_vedges[v2].begin(), graph.m_vedges[v2].end());
    if (S1.size() > S2.size())swap(S1,S2);
    cout << S1.size() << ' ' << S2.size() << '\n';
    vector<uint> result;
    result.clear();
    for (uint x : S1) {
        if (S2.count(x)) {
            result.push_back(x);
        }
    }
    return result.size();
}
int main(int argc, char **argv){
    /* set config */
    Config config(argc, argv);
    config.check();
    /* set parameter */
    // vector<int> k_list = {10, 20, 50, 100, 200};
    vector<int> k_list = {20, 50, 100};
    vector<map<string,vector<double>>> metric_record;

    string saveFolder = config.saveFolder;
    if (config.NonCold == true){
        saveFolder = saveFolder + "-NonCold";
        config.saveFolder = saveFolder;
    }
    make_dir(saveFolder);

    string resultFile = saveFolder + "/" + config.strGraph + "-" + config.strAlgo ;
    if (config.strAlgo == Test) resultFile = saveFolder + "/" + config.strAlgo ;
    if (config.DeBug) resultFile = saveFolder + "/ComPare";
    std::vector<std::string> resultHeader = {"Algo", "dataset", "Group", "FSI"};
    cout << "FSI:" << config.FSI_opt << endl;
    std::vector<std::string> resultRow = {config.strAlgo, config.strGraph, config.freqRatio, to_string(config.FSI_opt)};
    resultHeader.push_back("epsilon");
    resultRow.push_back(to_string(config.epsilon));
    // resultHeader.push_back("SampleRatio");
    // resultRow.push_back(to_string(config.SampleRatio));


    int &query_count = config.queryCount;
    vector<int> seeds = loadSeed(config.strFolder, config.strGraph, query_count, config.freqRatio);
    config.display();
    /* load graph */

    // double vm1, rss1;
    // process_mem_usage(vm1, rss1);
    auto readTime = std::chrono::steady_clock::now();
    Graph graph(config.strFolder, config.strGraph);
    double graphTime = (std::chrono::steady_clock::now() - readTime).count() / TIMES_PER_SEC;
    // double vm2, rss2;
    // process_mem_usage(vm2, rss2);
    graph.preprocess();
    // resultHeader.push_back("GraphSize");
    // resultRow.push_back(to_string((rss2-rss1)/1024.0));
    cout << "Graph read time: " << graphTime << endl;

    if (config.seedLimit != -1)
    {
        vector<int> newSeeds;
        for (auto v:seeds)
        {
            if (graph.getVDeg(v) > config.seedLimit)continue;
            newSeeds.push_back(v);
        }
        seeds = newSeeds;
        query_count = seeds.size();
    }

    BPMaximalClique fast_set;

    /*{
        if (config.Prepare)
        {
            double vm1, rss1;
            double vm2, rss2;
            double rat = 0;
            {
                Timer tm(2,"Total");
                {
                    Timer tm (0, "GRO");
                    if (config.ReOrderType == "none")
                    {
                        graph.OriOrder();
                    }else if (config.ReOrderType == "Deg")
                    {
                        graph.DegOrder();
                    }
                }
                {
                    Timer tm(1, "BSR");
                    process_mem_usage(vm1, rss1);
                    rat = fast_set.build(graph);
                    process_mem_usage(vm2, rss2);
                }
            }
            vector<string>hd,wt;
            hd.push_back("dataset");
            wt.push_back(config.strGraph);
            hd.push_back("Type");
            wt.push_back(config.ReOrderType);
            hd.push_back("Ratio");
            wt.push_back(to_string(rat));

            for (int i = 0; i<Timer::length();i++)
            {
                hd.push_back(Timer::des(i));
                wt.push_back(to_string(Timer::used(i)));
            }

            hd.push_back("vm");
            wt.push_back(to_string((vm2-vm1)/1024) + "(MB)");
            hd.push_back("rss");
            wt.push_back(to_string((rss2-rss1)/1024) + "(MB)");

            save_csv_row(saveFolder + "/stat.csv" ,hd,wt);
            return 0;
        }
    }*/


    {
        Timer tm(0, "Preprocess");
        if (config.FSI_opt)
        {
            config.CompRatio = 1;
            cout << "Preprocess..." << endl;
            if (config.ReOrderType == "Deg") graph.DegOrder();
            else if (config.ReOrderType == "Heap") graph.HeapOrder();
            else if (config.ReOrderType == "Block") graph.BlockOrder();
            else if (config.ReOrderType == "Sample") graph.SampleOrder();
            else if (config.ReOrderType == "FastDeg") graph.AdapDegOrder();
            else if (config.ReOrderType == "Butterfly") graph.ButterflyOrder();
            else
            {
                config.ReOrderType = "none";
                graph.OriOrder();
            }
            // double vm1, rss1;
            // process_mem_usage(vm1, rss1);
            double reo = tm.cost() / TIMES_PER_SEC;
            cout << "Reorder cost: " << reo << endl;
            if (config.strAlgo == Truncated) fast_set.build(graph, config.maxClickPerUser);
            else
            {
                config.CompRatio = fast_set.build(graph);
                // config.CompRatio = fast_set.ReoBuild(graph);
            }
            // resultHeader.push_back("bsrTime");
            // resultRow.push_back(to_string(tm.cost()/TIMES_PER_SEC));
            cout << "Build cost: " << (tm.cost() / TIMES_PER_SEC - reo )<< endl;

            // double vm2, rss2;
            // process_mem_usage(vm2, rss2);
            // resultHeader.push_back("bsrSpace");
            // resultRow.push_back(to_string((rss2 - rss1)/1024.0));
            resultHeader.push_back("ReOrder");
            resultRow.push_back(config.ReOrderType);
            resultHeader.push_back("CompRatio");
            resultRow.push_back(to_string(config.CompRatio));
            // resultHeader.push_back("reoTime");
            // resultRow.push_back(to_string(reo));
            cout << "Preprocess is Done." << endl;
        }
        if (config.ReOrderType == "Simple") fast_set.SimpleBuild(graph);
    }
    if (!config.FSI_opt) graph.setBuild();
    double query_cost = 0;
    std::vector<double> swing(graph.getNv());
    if(config.strAlgo==Exact){
        cout << "start Swing with Exact!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastExactSwing(vq, config.alpha, config.epsilon, swing, graph, fast_set);
                else ExactSwing(vq, config.alpha, config.epsilon, swing, graph);
                query_cost = tm.cost();
            }
            /* save Metric */
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
        }
    }
    else if (config.strAlgo==Truncated)
    {
        cout << "start Swing with Truncated swing!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt)FastTruncSwing(vq, swing, graph, config, fast_set);
                else TruncSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            /* save Metric */
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
        }
#ifdef DEBUG
        resultHeader.push_back("maxUserPerItem");
        resultRow.push_back(to_string(config.maxUserPerItem));
        resultHeader.push_back("maxClickPerUser");
        resultRow.push_back(to_string(config.maxClickPerUser));
#endif
    }
    else if(config.strAlgo==NaiveMC){
        cout << "start Swing with NaiveMC!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastMCSwing(vq, swing, graph, config, fast_set);
                else MonteCarloSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==GNS){
        // freopen((saveFolder + "/" + config.strGraph + " GNS.csv").c_str(), "w", stdout);
        // cout << "start Swing with GNS!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastGNS(vq, swing, graph, config, fast_set);
                else GroupMCSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==USS){
        cout << "start Swing with User Subset Sampling!" << endl;
        // freopen((saveFolder + "/" + config.strGraph + " GSS.csv").c_str(), "w", stdout);
        if (config.FSI_opt)graph.mapBuild(fast_set);
        HashSpace::init(graph.getNv());
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastUSS5(vq, swing, graph, config, fast_set);
                else
                {
                    ConstUSS(vq,swing,graph,config);
                }
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);

            // swap(graph.Flag_Reo,tmp_opt);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            // swap(graph.Flag_Reo,tmp_opt);

            if (config.ValueRecord){
                // if (config.FSI_opt)
                // {
                //     vector<double> swCopy = swing;
                //     for (int i=0;i<swing.size();i++)
                //     {
                //         swing[graph.oriIndex[i]] = swCopy[i];
                //     }
                // }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==USS2){
        cout << "start Swing with User Subset Sampling!" << endl;
        // freopen((saveFolder + "/" + config.strGraph + " GSS.csv").c_str(), "w", stdout);
        graph.setBuild();
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastUSS2(vq, swing, graph, config, fast_set);
                else
                {
                    assert(0);
                }
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);

            bool tmp_opt=false;
            // swap(graph.Flag_Reo,tmp_opt);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            // swap(graph.Flag_Reo,tmp_opt);

            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==USS3){
        cout << "start Swing with User Subset Sampling!" << endl;
        // freopen((saveFolder + "/" + config.strGraph + " GSS.csv").c_str(), "w", stdout);
        graph.setBuild();
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastUSS3(vq, swing, graph, config, fast_set);
                else
                {
                    assert(0);
                }
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);

            bool tmp_opt=false;
            // swap(graph.Flag_Reo,tmp_opt);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            // swap(graph.Flag_Reo,tmp_opt);

            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==USS4){
        cout << "start Swing with User Subset Sampling!" << endl;
        // freopen((saveFolder + "/" + config.strGraph + " GSS.csv").c_str(), "w", stdout);
        graph.setBuild();
        graph.mapBuild(fast_set);
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastUSS4(vq, swing, graph, config, fast_set);
                else
                {
                    assert(0);
                }
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);

            bool tmp_opt=false;
            // swap(graph.Flag_Reo,tmp_opt);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            // swap(graph.Flag_Reo,tmp_opt);

            if (config.ValueRecord){
                // if (config.FSI_opt)
                // {
                //     vector<double> swCopy = swing;
                //     for (int i=0;i<swing.size();i++)
                //     {
                //         swing[graph.oriIndex[i]] = swCopy[i];
                //     }
                // }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==USS5){
        cout << "start Swing with User Subset Sampling!" << endl;
        // freopen((saveFolder + "/" + config.strGraph + " GSS.csv").c_str(), "w", stdout);
        // graph.setBuild();
        graph.mapBuild(fast_set);
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastUSS5(vq, swing, graph, config, fast_set);
                else
                {
                    assert(0);
                }
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);

            bool tmp_opt=false;
            // swap(graph.Flag_Reo,tmp_opt);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            // swap(graph.Flag_Reo,tmp_opt);

            if (config.ValueRecord){
                // if (config.FSI_opt)
                // {
                //     vector<double> swCopy = swing;
                //     for (int i=0;i<swing.size();i++)
                //     {
                //         swing[graph.oriIndex[i]] = swCopy[i];
                //     }
                // }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==ASC){

        // double before_vm, before_rss;
        // process_mem_usage(before_vm, before_rss);
        // auto mapStart = std::chrono::steady_clock::now();
        if (config.FSI_opt)graph.mapBuild(fast_set);
        // double mapTime = (std::chrono::steady_clock::now() - mapStart).count() / TIMES_PER_SEC;
        // double after_vm, after_rss;
        // process_mem_usage(after_vm, after_rss);

        // resultHeader.push_back("mapTime");
        // resultRow.push_back(to_string(mapTime));
        // resultHeader.push_back("mapSpace");
        // resultRow.push_back(to_string((after_rss - before_rss)/1024.0));
        cout << "start Swing with Adaptive Swing!" << endl;
        for(const auto& vq: seeds){
	        std::fill(swing.begin(), swing.end(), 0.0);
            // cout << "query count: " << ++query_count << "\tquery node: " << vq << endl;
            {
                Timer tm(1, "allTime");
	            if (config.FSI_opt) FastASC(vq, swing, graph, config, fast_set);
                else PRSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record, config.ext_k);
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
#ifdef DEBUG
        resultHeader.push_back("probf");
        resultRow.push_back(to_string(config.probf));
        resultHeader.push_back("AdaptRho");
        resultRow.push_back(to_string(config.AdaptRho));
#endif
    }
    else if (config.strAlgo==KASC){
        Accumulator::add(4, 0, "RefineSize");
        int RefSize = 0;
        if (config.FSI_opt)graph.mapBuild(fast_set);
        cout << "start Swing with Adaptive Swing!" << endl;
        for(const auto& vq: seeds){
            // RandomUtils::rng.seed(20251013);
            std::fill(swing.begin(), swing.end(), 0.0);
            // cout << "query count: " << ++query_count << "\tquery node: " << vq << endl;
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastKASC(vq, swing, graph, config, fast_set);
                else PRSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph,graph);
            /* save Metric */
            // quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record, config.ext_k);
            int temp_RefSize = Accumulator::get(4) - RefSize;
            Topk_quality_metrics(vq, graph, config, swing, k_list, query_cost,temp_RefSize, metric_record, config.ext_k);
            RefSize += temp_RefSize;
            // save_csv_row(saveFolder + "/" + config.strGraph + "-" + config.strAlgo + "-statistic.csv", {"id","degree", "total time", "appr gain", "sep gain","max var","Recall@10", "NDCG@10", "Recall@20", "NDCG@20", "Recall@50", "NDCG@50", "Recall@100", "NDCG@100"}, {to_string(vq), to_string(graph.getVDeg(vq)), to_string(cost/TIMES_PER_SEC),to_string(Accumulator::get(3) - appr_gain), to_string(Accumulator::get(4) - sep_gain), to_string(Accumulator::get(5) - max_gain), to_string(metric["Recall"][0]), to_string(metric["NDCG"][0]), to_string(metric["Recall"][1]), to_string(metric["NDCG"][1]), to_string(metric["Recall"][2]), to_string(metric["NDCG"][2]), to_string(metric["Recall"][3]), to_string(metric["NDCG"][3])});
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
#ifdef DEBUG
        resultHeader.push_back("ExtCand");
        resultRow.push_back(to_string(config.ext_k));
        resultHeader.push_back("PRsigma");
        resultRow.push_back(to_string(config.PostRefineSigma));
        resultHeader.push_back("maxTopk");
        resultRow.push_back(to_string(config.maxTopk));
        resultHeader.push_back("AdaptRho");
        resultRow.push_back(to_string(config.AdaptRho));
#endif
    }

    else if(config.strAlgo==MonteCarlo){
        cout << "start Swing with Monte-Carlo!" << endl;
        // saveFolder = saveFolder + "-" + to_string(config.SampleRatio) + "-" + to_string(config.SamplePower);
        for(const auto& vq: seeds){
            // cout << "query count: " << ++query_count << "\tquery node: " << vq << endl;
            // if (graph.getVDeg(vq) < 1000) continue;
            {
                Timer tm(1, "MonteCarlo Swing");
                if (config.FSI_opt) FastMCSwing(vq, swing, graph, config, fast_set);
                else MonteCarloSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            // store_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
        }
#ifdef DEBUG
        resultHeader.push_back("SampRatio");
        resultRow.push_back(to_string(config.SampleRatio));
#endif
    }
    else if(config.strAlgo==Truth){
        cout << "start Swing with Ground Truth!" << endl;
        int query_count = 0;
        make_dir("data/" + config.strGraph + "/groundTruth");
        {
            std::vector<double> swing(graph.getNv());
            for(const auto& vq: seeds){
                std::fill(swing.begin(), swing.end(), 0.0);
                // cout << "query count: " << ++query_count << "\tquery node: " << vq << endl;
                {
                    Timer tm(1, "Exact Swing");
                    FastExactSwing(vq, config.alpha, config.epsilon, swing, graph, fast_set);
                }
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                store_ground_truth(vq, 500, swing, "data/" + config.strGraph + "/groundTruth");
            }
        }
    }
    else if(config.strAlgo==AdExact){
        cout << "start Swing with Exact!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastExactSwing(vq, config.alpha, config.epsilon, swing, graph, fast_set);
                else ExactSwing(vq, config.alpha, config.epsilon, swing, graph);
                query_cost = tm.cost();
            }
            /* save Metric */
            Extra_quality_metrics(vq, graph, swing, k_list, query_cost, metric_record);
        }
    }
    else if(config.strAlgo==AdGNS){
        cout << "start Swing with GNS!" << endl;
        for(const auto& vq: seeds){
            // int Ratio_sum = 0;
            // for (int i = (1<<6); i > 0 ; i>>=1)
            // {
            //     int tmp = Ratio_sum + i;
            //     {
            //         config.SampleRatio = tmp;
            //         std::fill(swing.begin(), swing.end(), 0.0);
            //         {
            //             Timer tm(1, "allTime");
            //             if (config.FSI_opt) FastGNS(vq, swing, graph, config, fast_set);
            //             else GroupMCSwing(vq, swing, graph, config);
            //             query_cost = tm.cost();
            //         }
            //         double p = Fast_metrics(vq, graph, swing, k_list, query_cost, metric_record);
            //         if (p > 0.99) continue;
            //     }
            //     Ratio_sum += i;
            // }
            // Ratio_sum += 1;
            // config.SampleRatio = Ratio_sum;
            long long n_rounds = 0;
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                n_rounds = AdaptGNS(vq, swing, graph, config, fast_set);
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            AdGNS_quality_metrics(vq, graph, swing, k_list, query_cost, metric_record, n_rounds, config.SampleRatio);
        }
// #ifdef DEBUG
//         resultHeader.push_back("SampRatio");
//         resultRow.push_back(to_string(config.SampleRatio));
// #endif
    }
    else if(config.strAlgo==CompIns){
        // freopen((saveFolder + "/" + config.strGraph + "-Ins_log.csv").c_str(), "w", stdout);
        // cout << "start Swing with GNS!" << endl;
        vector<double> timeLast(5);
        vector<pair<string,double>> timeReco(5);
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "Time");
                // CompGNS(vq, swing, graph, config, fast_set);
                CompExt(vq, swing, graph, config, fast_set);
            }
            for (int i=1;i<=3;i++)
            {
                timeReco[i] = {Timer::des(i),Timer::used(i) - timeLast[i]};
                timeLast[i] = Timer::used(i);
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            CompIns_quality_metrics(vq, graph, config, swing, k_list, timeReco, metric_record);
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }

        for (int i=2;i<=3;i++)
        {
            resultHeader.push_back(Timer::des(i));
            resultRow.push_back(to_string(Timer::used(i)/query_count));
        }
    }
    else if(config.strAlgo==Test){
        freopen((saveFolder + "/" + config.strGraph + "-Ins_log.csv").c_str(), "w", stdout);
        // cout << "start Swing with GNS!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                TestGNS(vq, swing, graph, config, fast_set);
                query_cost = tm.cost();
            }
            // view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            quality_metrics(vq, graph, config, swing, k_list, query_cost, metric_record);
            if (config.ValueRecord){
                if (config.FSI_opt)
                {
                    vector<double> swCopy = swing;
                    for (int i=0;i<swing.size();i++)
                    {
                        swing[graph.oriIndex[i]] = swCopy[i];
                    }
                }
                view_ground_truth(vq, 500, swing, saveFolder + "/" + config.strGraph, graph);
            }
        }
    }
    else if(config.strAlgo==Record){
        // freopen(("data/" + config.strGraph + "/BSR/user_dist.csv").c_str(), "w", stdout);
        string out_file = "data/" + config.strGraph + "/BSR/" + config.ReOrderType + "-user_dist.csv";
        string stat_file = "data/" + config.strGraph + "/BSR/" + config.ReOrderType +"-user_dist.stat";
        std::filesystem::create_directories(std::filesystem::path(out_file).parent_path());
        ofstream fout(out_file);
        ofstream fout_stat(stat_file);
        fout << "user_id\tudeg\tbsr_deg\n";
        // cout << "start Swing with GNS!" << endl;
        int n = graph.getNu();
        vector<int> user_idx(n);
        for (int i=0;i<n;i++) user_idx[i] = i;
        const auto& udeg = graph.m_udeg;
        const auto& BSR = fast_set.graph;
        sort(user_idx.begin(), user_idx.end(),[&udeg](const int a, const int b){return udeg[a] > udeg[b];});
        long long edge = 0, bsrEdge = 0;
        for (int i=0;i<n;i++)
        {
            auto u = user_idx[i];
            edge += udeg[u];
            bsrEdge += BSR[u].deg;
            if (udeg[u] == 0) continue;
            fout << u << "\t" << udeg[u] << "\t" << BSR[u].deg << '\n';
        }
        fout_stat << "CompressRatio" << "\t" << "Edges" << "\t" << "bsrEdges" << '\n';
        fout_stat << (double)bsrEdge/ edge  << "\t" << edge << "\t" << bsrEdge << '\n';
        fout.close();
        fout_stat.close();
        cout << "Saved to: " << out_file << '\n';
        cout << "Total users: " << n << "\t" << (double)bsrEdge/ edge  << "\t" << edge << "\t" << bsrEdge << '\n';
        return 0;
    }
    else{
        cerr << "Unknown algorithm: " << config.strAlgo << endl;
        return ERROR_IN_COMMAND_LINE;
    }

    /* Results Save */
    if (config.DeBug)
    {
        resultHeader.push_back("SampleTime");
        resultRow.push_back(to_string(Timer::used(4)/query_count));
        resultHeader.push_back("Addition");
        resultRow.push_back(to_string(Timer::used(3)/query_count));
    }
    /* Metric Save */
    if (config.strAlgo != Truth)
        Metric_save(resultFile, config, k_list, metric_record, resultHeader, resultRow, query_count);
    /* Accumulator Save */
    Accumulator::save(query_count, resultFile, config, resultHeader, resultRow);

    /* Timer Save */
    // cout << Timer::used(1)*1000/query_count << " milli-seconds per query" << endl;
    Timer::save(query_count, resultFile, resultHeader, resultRow);

    return SUCCESS;
}