#include <csignal>
#include <limits>
#include <sys/stat.h>
#include "graph.h"
#include "algo.h"
#include <filesystem>
// #define DEBUG
using namespace std;
namespace po = boost::program_options;

namespace { 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
} // namespace
std::vector<double> Accumulator::values;
std::vector<std::string> Accumulator::descs;

void quality_metrics(const int& vq, const Graph& graph, const Config& config, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

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
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

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
void Extra_quality_metrics(const int& vq, const Graph& graph, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return ;

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
double Fast_metrics(const int& vq, const Graph& graph, const std::vector<double>& est_lue_values, const std::vector<int>& k_list, const double& time_cost, vector<map<string,vector<double>>>& metric_record, int ext_k=5)
{
    string gt_file = "data/" + graph.m_graph + "/groundTruth/" + to_string(vq) + ".txt";
    vector<pair<int,double>> ground_truth = read_ground_truth(gt_file);
    map<string,vector<double>> results;
    if (ground_truth.size() == 0) return 1;

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

    std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
        std::greater<std::pair<double,int>>());

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

    std::partial_sort(item_swing.begin(), item_swing.begin()+topK, item_swing.end(),
        std::greater<std::pair<double,int>>());

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
int main(int argc, char **argv){
    /* set config */
    Config config(argc, argv);
    config.check();
    /* set parameter */
    vector<int> k_list = {20, 50, 100};
    vector<map<string,vector<double>>> metric_record;

    string saveFolder = config.saveFolder;
    make_dir(saveFolder);

    string resultFile = saveFolder + "/" + config.strGraph + "-" + config.strAlgo ;
    if (config.strAlgo == Test) resultFile = saveFolder + "/" + config.strAlgo ;
    if (config.DeBug) resultFile = saveFolder + "/ComPare";
    std::vector<std::string> resultHeader = {"Algo", "dataset", "Group", "FSI"};
    cout << "FSI:" << config.FSI_opt << endl;
    std::vector<std::string> resultRow = {config.strAlgo, config.strGraph, config.freqRatio, to_string(config.FSI_opt)};
    resultHeader.push_back("epsilon");
    resultRow.push_back(to_string(config.epsilon));

    int &query_count = config.queryCount;
    vector<int> seeds = loadSeed(config.strFolder, config.strGraph, query_count, config.freqRatio);
    config.display();
    /* load graph */
    auto readTime = std::chrono::steady_clock::now();
    Graph graph(config.strFolder, config.strGraph);
    double graphTime = (std::chrono::steady_clock::now() - readTime).count() / TIMES_PER_SEC;
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

    {
        Timer tm(0, "Preprocess");
        if (config.FSI_opt)
        {
            config.CompRatio = 1;
            cout << "Preprocess..." << endl;
            if (config.ReOrderType == "Deg") graph.DegOrder();
            else if (config.ReOrderType == "Sample") graph.SampleOrder();
            else
            {
                config.ReOrderType = "none";
                graph.OriOrder();
            }
            double reo = tm.cost() / TIMES_PER_SEC;
            cout << "Reorder cost: " << reo << endl;
            if (config.strAlgo == Truncated) fast_set.build(graph, config.maxClickPerUser);
            else
            {
                config.CompRatio = fast_set.build(graph);
            }
            cout << "Build cost: " << (tm.cost() / TIMES_PER_SEC - reo )<< endl;

            resultHeader.push_back("ReOrder");
            resultRow.push_back(config.ReOrderType);
            resultHeader.push_back("CompRatio");
            resultRow.push_back(to_string(config.CompRatio));
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
    else if(config.strAlgo==ASC){
        if (config.FSI_opt)graph.mapBuild(fast_set);
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
        int RefSize = 0;
        if (config.FSI_opt)graph.mapBuild(fast_set);
        cout << "start Swing with Adaptive Swing!" << endl;
        for(const auto& vq: seeds){
            std::fill(swing.begin(), swing.end(), 0.0);
            {
                Timer tm(1, "allTime");
                if (config.FSI_opt) FastKASC(vq, swing, graph, config, fast_set);
                else PRSwing(vq, swing, graph, config);
                query_cost = tm.cost();
            }
            /* save Metric */
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
    else if(config.strAlgo==Truth){
        cout << "start Swing with Ground Truth!" << endl;
        int query_count = 0;
        make_dir("data/" + config.strGraph + "/groundTruth");
        {
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
    else{
        cerr << "Unknown algorithm: " << config.strAlgo << endl;
        return ERROR_IN_COMMAND_LINE;
    }

    if (config.strAlgo != Truth)
    {
        /* Metric Save */
        Metric_save(resultFile, config, k_list, metric_record, resultHeader, resultRow, query_count);

        /* Accumulator Save */
        Accumulator::save(query_count, resultFile, config, resultHeader, resultRow);

        /* Timer Save */
        // cout << Timer::used(1)*1000/query_count << " milli-seconds per query" << endl;
        Timer::save(query_count, resultFile, resultHeader, resultRow);
    }
    return SUCCESS;
}