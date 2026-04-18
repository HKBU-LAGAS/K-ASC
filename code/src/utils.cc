#include<iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <ios>
#include <fstream>
#include <unordered_set>
#include "utils.h"
#include <map>
#include <filesystem>
#include <unordered_map>

using namespace std;

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   uint64 vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

void disp_mem_usage(){
    double vm, rss;

    process_mem_usage(vm, rss);
    vm/=1024;
    rss/=1024;
    //cout<< "Memory Usage:" << msg << " vm:" << vm << " MB  rss:" << rss << " MB"<<endl;
    cout<< "PhysicalMem(MB) "<< rss << endl;
    cout<< "VirtualMem(MB) "<< vm << endl;
}

uint getProcMemory(){
	struct rusage r_usage;
	getrusage(RUSAGE_SELF, &r_usage);
	//string strMemUsage = to_string(r_usage.ru_maxrss);
	return r_usage.ru_maxrss / 1024.0 ;
	//return strMemUsage;
}

vector<double> Timer::timeUsed;
vector<string> Timer::timeUsedDesc;

void xorshifinit(){
    x_state = (uint32_t)time(NULL);
}
uint32_t xorshift32(void){
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	x_state ^= x_state << 13;
	x_state ^= x_state >> 17;
	x_state ^= x_state << 5;
	return x_state;
}

void fastSrand(){
    g_seed = time(NULL);
}

uint32_t fastRand() {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

// save file
// 检查文件是否存在
bool file_exists(const std::string& filename) {
	std::ifstream infile(filename);
	return infile.good();
}
void make_dir(const std::string& dir_path, bool clear_if_exists)
{
	try {
		if (clear_if_exists && std::filesystem::exists(dir_path)) {
			// 安全检查：确保它是一个目录，而不是一个文件
			if (std::filesystem::is_directory(dir_path)) {
				std::filesystem::remove_all(dir_path);
			} else {
				// 如果路径存在但是个文件，这也是个问题，remove_all也能处理，但最好明确指出
				std::cout << "Warning: Path '" << dir_path << "' exists but is a file. Removing it." << std::endl;
				std::filesystem::remove(dir_path);
			}
		}

		std::filesystem::create_directories(dir_path);
	} catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Failed to create directories: " << e.what() << std::endl;
		exit(1);
	}
}
void make_dir_for_file(const std::string& file_path)
{
	// 1. 将字符串路径转换为 filesystem::path 对象
	std::filesystem::path path_obj(file_path);

	// 2. 检查路径是否有父目录
	if (path_obj.has_parent_path()) {
		// 3. 获取父目录路径
		std::filesystem::path parent_dir = path_obj.parent_path();

		// 4. 如果父目录不存在，则创建它
		//    create_directories 会递归创建所有不存在的父目录
		if (!std::filesystem::exists(parent_dir)) {
			try {
				std::filesystem::create_directories(parent_dir);
				std::cout << "Created directory: " << parent_dir.string() << std::endl;
			} catch (const std::filesystem::filesystem_error& e) {
				std::cerr << "Failed to create directories for file '" << file_path << "': " << e.what() << std::endl;
				exit(1); // 或者抛出异常，根据你的错误处理策略
			}
		}
	}
	// 如果没有父目录（例如 "file.txt"），则无需任何操作，文件将在当前目录创建。
}
// 添加一行到CSV（如果首次写入自动加表头）
void save_csv_row(const std::string& filename,
				  const std::vector<std::string>& header,
				  const std::vector<std::string>& row)
{
	make_dir_for_file(filename);
	bool need_header = !file_exists(filename);

	std::ofstream fout(filename, std::ios::app);
	if (!fout) {
		std::cerr << "Can't open file: " << filename << std::endl;
		return;
	}

	// 写表头
	if (need_header) {
		for (size_t i = 0; i < header.size(); ++i) {
			fout << header[i];
			if (i != header.size() - 1) fout << ',';
		}
		fout << '\n';
	}

	// 写数据行
	for (size_t i = 0; i < row.size(); ++i) {
		fout << row[i];
		if (i != row.size() - 1) fout << ',';
	}
	fout << '\n';

	fout.close();
}

// 读取ground truth
vector<pair<int,double>> read_ground_truth(const std::string& filename) {
	std::vector<pair<int,double>> ground_truth;
	ground_truth.reserve(500);  // 如果ground truth行数固定，提前分配
	std::ifstream fin(filename);
	if (!fin.is_open()) {
		std::cerr << "Failed to open " << filename << std::endl;
		return ground_truth;
	}
	int item;
	double value;
	while (fin >> item >> value) {
		// 数据有序, 读取到value为0时停止
		if (value == 0) break;
		ground_truth.emplace_back(item, value);
	}
	return ground_truth;
}

vector<double> calc_Precision(const std::vector<int>& est_lue, const std::vector<pair<int,double>>& ground_truth, const std::vector<int>& k_list, int ext_k) {
	/* Precision@K
	 * est_lue: 估计结果的item列表，按估计值从大到小排序
	 * ground_truth: 真实结果的item-value对列表，按value从大到小排序
	 * k_list: 需要计算的K值列表
	 * 返回: 对应K值的Precision列表
	 */
	vector<double> precision_list;
	for (int k : k_list) {
		std::unordered_set<int> gt_set;
		for (int i=0; i < ground_truth.size() && i < k; ++i) {
			gt_set.insert(ground_truth[i].first);   // 只插入 item
		}
		if (ground_truth.size() > k)
		{
			int ext = 0;
			while (ext + k < ground_truth.size() && ground_truth[ext+k].second == ground_truth[k-1].second)
			{
				gt_set.insert(ground_truth[ext+k].first);
				ext++;
			}
		}
		int hit = 0;
		for (size_t i = 0; i < est_lue.size() && i < k; ++i) {
			if (gt_set.count(est_lue[i]))
				++hit;
		}
		// int cur_k = std::min(k + ext_k, static_cast<int>(est_lue.size()));
		int cur_k = std::min(k, static_cast<int>(ground_truth.size()));
		double precision = cur_k == 0 ? 0.0 : static_cast<double>(hit) / cur_k;
		precision_list.push_back(precision);
	}
	return precision_list;
}

vector<double> calc_Recall(const std::vector<int>& est_lue, const std::vector<std::pair<int, double>>& ground_truth, const std::vector<int>& k_list, int ext_k)
{
	std::vector<double> recall_list;
	for (int k : k_list)
	{
		std::unordered_set<int> gt_set;
		for (int i = 0; i < ground_truth.size() && i < k; ++i)
		{
			gt_set.insert(ground_truth[i].first); // ground_truth 前k个 item
		}
		if (ground_truth.size() > k)
		{
			int ext = 0;
			while (ext + k < ground_truth.size() && ground_truth[ext+k].second == ground_truth[k-1].second)
			{
				gt_set.insert(ground_truth[ext+k].first);
				ext++;
			}
		}
		int hit = 0;
		for (size_t i = 0; i < est_lue.size() && i < k + ext_k; ++i)
		{
			if (gt_set.count(est_lue[i]))
				++hit; // 交集计数
		}

		int cur_k_gt = std::min(k, static_cast<int>(ground_truth.size()));
		double recall = min((cur_k_gt == 0) ? 0.0 : static_cast<double>(hit) / cur_k_gt, 1.0);
		recall_list.push_back(recall);
	}
	return recall_list;
}

vector<double> calc_NDCG(const std::vector<int>& est_lue, const std::vector<std::pair<int, double>>& ground_truth, const std::vector<int>& k_list, int ext_k)
{
	std::vector<double> ndcg_list;
	for (int k : k_list)
	{
		// 1. 取ground_truth前k个item作为relevant
		std::unordered_set<int> gt_set;
		for (int i = 0; i < ground_truth.size() && i < k; ++i)
			gt_set.insert(ground_truth[i].first);
		if (ground_truth.size() > k)
		{
			int ext = 0;
			while (ext + k < ground_truth.size() && ground_truth[ext+k].second == ground_truth[k-1].second)
			{
				gt_set.insert(ground_truth[ext+k].first);
				ext++;
			}
		}
		// 2. 计算DCG
		double dcg = 0.0;
		for (int i = 0; i < est_lue.size() && i < k + ext_k; ++i)
		{
			if (gt_set.count(est_lue[i]))
				dcg += 1.0 / std::log2(i + 2); // i从0 log2(0+2)=log2(2)=1
		}

		// 3. 计算IDCG，假设所有relevant项目都被命中且排前面
		int rel_count = std::min(k, static_cast<int>(gt_set.size()));
		double idcg = 0.0;
		for (int i = 0; i < rel_count; ++i)
		{
			idcg += 1.0 / std::log2(i + 2);
		}

		// 4. NDCG
		double ndcg = (idcg == 0) ? 0.0 : dcg / idcg;
		ndcg_list.push_back(ndcg);
	}
	return ndcg_list;
}

vector<double> calc_AvgError(const std::vector<double>& est_lue, const std::vector<pair<int,double>>& ground_truth, const std::vector<int>& k_list)
{
	/* AvgError@K
	 * est_lue: 估计结果的item-value对列表，按item索引存储估计值
	 * ground_truth: 真实结果的item-value对列表，按value从大到小排序
	 * k_list: 需要计算的K值列表
	 * 返回: 对应topK的AvgError列表
	 */
	vector<double>AvgError_list;
	for (int k : k_list)
	{
		double sum_error = 0.0;
		for (int i=0; i < ground_truth.size() && i < k; ++i) {
			int item = ground_truth[i].first;
			double truth = ground_truth[i].second;
			double estimate = est_lue[item]; // 若无则0
			sum_error += std::fabs(truth - estimate) / truth;
		}
		int cur_k = std::min(k, static_cast<int>(ground_truth.size()));
		double avg_error = cur_k == 0 ? 0.0 : sum_error / cur_k;
		AvgError_list.push_back(avg_error);
	}
	return AvgError_list;
}

double EmpiricalFun(int n, double var, double limt, double p){
	return sqrt(2 * var * log(3/p) / n) + 3 * limt / n * log(3/p) ;
}
