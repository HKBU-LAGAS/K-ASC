#include "algo.h"
#include <math.h>
#include <iostream>
#include <queue>
#include <set>
#include <list>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <functional>
// #include "util.hpp"

using namespace std;

inline std::mt19937& getGlobalRng() {
	static std::random_device rd;
	static std::mt19937 rng(20251013);
	return rng;
}
void intersection(const std::unordered_set<uint>& A, const std::unordered_set<uint>& B, vector<uint>& result){
	const std::unordered_set<uint>& small = (A.size() < B.size()) ? A : B;
	const std::unordered_set<uint>& big   = (A.size() < B.size()) ? B : A;
	result.clear();
	for (uint x : small) {
		if (big.count(x)) {
			result.push_back(x);
		}
	}
}
void intersection(const ankerl::unordered_dense::set<uint>& A, const ankerl::unordered_dense::set<uint>& B, vector<uint>& result){
	const auto& small = (A.size() < B.size()) ? A : B;
	const auto& big   = (A.size() < B.size()) ? B : A;
	result.clear();
	for (uint x : small) {
		if (big.count(x)) {
			result.push_back(x);
		}
	}
}
int intersection(const ankerl::unordered_dense::set<uint>& A, const ankerl::unordered_dense::set<uint>& B){
	const auto& small = (A.size() < B.size()) ? A : B;
	const auto& big   = (A.size() < B.size()) ? B : A;
	int size = 0;
	for (uint x : small) {
		if (big.count(x)) {
			// result.push_back(x);
			size++;
		}
	}
	return size;
}
void Naive_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& result, uint ui, uint uj){
	const std::unordered_set<uint> A(graph.m_uedges[ui].begin(),graph.m_uedges[ui].end());
	const std::unordered_set<uint> B(graph.m_uedges[uj].begin(),graph.m_uedges[uj].end());
	result.clear();
	for (uint x : A) {
		if (B.count(x)) {
			result.push_back(x);
		}
	}
}
int fast_item_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& Cij, uint ui, uint uj)
{
	// result id -= Number of User
	int *sets_base = fast_set.sets_base;
	PackState *sets_state = fast_set.sets_state;
	const UVertex& u = fast_set.graph[ui];
	const UVertex& v = fast_set.graph[uj];
	int *pool_base = fast_set.pool_base;
	PackState *pool_state = fast_set.pool_state;
	Cij.clear();
	{
		int len= bp_intersect_filter_simd4x(pool_base + u.start, pool_state + u.start, u.deg,
													 pool_base + v.start, pool_state + v.start, v.deg,
													 sets_base , sets_state);
		for (int k=0; k<len; k++)
		{
			int v_base = sets_base[k];
			int v_state = sets_state[k];
			int v_high = (v_base << PACK_SHIFT);
			while (v_state)
			{
				int v = (v_high | __builtin_ctz(v_state));
				Cij.push_back(v - graph.getNu());
				v_state &= (v_state - 1);
			}
		}
	}
	return Cij.size();
}
int fast_item_intersection(const BPMaximalClique& fast_set, const Graph& graph, uint ui, uint uj)
{
	// result id -= Number of User
	const UVertex& u = fast_set.graph[ui];
	const UVertex& v = fast_set.graph[uj];
	int *pool_base = fast_set.pool_base;
	PackState *pool_state = fast_set.pool_state;
	return bp_intersect_filter_simd4x_count(pool_base + u.start, pool_state + u.start, u.deg,
													 pool_base + v.start, pool_state + v.start, v.deg);

}
int Reorder_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& Cij, uint ui, uint uj)
{
	// result id -= Number of User
	int *sets_base = fast_set.sets_base;
	PackState *sets_state = fast_set.sets_state;
	const UVertex& u = fast_set.graph[ui];
	const UVertex& v = fast_set.graph[uj];
	int *pool_base = fast_set.pool_base;
	PackState *pool_state = fast_set.pool_state;
	Cij.clear();
	{
		int len= bp_intersect_filter_simd4x(pool_base + u.start, pool_state + u.start, u.deg,
													 pool_base + v.start, pool_state + v.start, v.deg,
													 sets_base , sets_state);
		for (int k=0; k<len; k++)
		{
			int v_base = sets_base[k];
			int v_state = sets_state[k];
			int v_high = (v_base << PACK_SHIFT);

			while (v_state)
			{
				int v = (v_high | __builtin_ctz(v_state));
				// if (graph.getReo()) Cij.push_back(graph.oriIndex[v]);
				// else Cij.push_back(v);
				Cij.push_back(v);
				v_state &= (v_state - 1);
			}
		}
	}
	return u.deg + v.deg;
}
int Simple_intersection(const BPMaximalClique& fast_set, const Graph& graph, vector<uint>& Cij, uint ui, uint uj)
{
	// result id -= Number of User
	int *sets_base = fast_set.sets_base;
	const UVertex& u = fast_set.graph[ui];
	const UVertex& v = fast_set.graph[uj];
	int *pool_base = fast_set.pool_base;
	Cij.clear();
	{
		int len= intersect_simd4x(pool_base + u.start, u.deg, pool_base + v.start, v.deg, sets_base );
		for (int k=0; k<len; k++)
		{
			int v = sets_base[k];
			Cij.push_back(v);
		}
	}
	return u.deg + v.deg;
}

inline int intersect_simd4x_count(const std::vector<int>& a, const std::vector<int>& b)
{
	return intersect_simd4x_count(a.data(), (int)a.size(), b.data(), (int)b.size());
}

int Simple_intersection(const Graph& graph, const uint ui, const uint uj)
{
	return intersect_simd4x_count(graph.uSorted[ui], graph.uSorted[uj]);
}
void ExactSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph){
	// std::fill(swing.begin(), swing.end(), 0.0);
	const vector<uint> &U_star = graph.m_vedges[vq];
	// int u_filter = graph.m_vedges[vq].size() - U_star.size();
	// cout << "u_filter: " << u_filter << endl;

	for (int i=0; i<U_star.size(); i++){
		// uint ui = U_star[i];
		// set<uint> Iui(graph.m_uedges[ui].begin(),graph.m_uedges[ui].end());
		// set<uint> Iui = Iu_sets[i];
		for (int j=0; j<i; j++) {
			// uint uj = U_star[j];
			// set<uint> Iuj(graph.m_uedges[uj].begin(),graph.m_uedges[uj].end());
			// set<uint> Iuj = Iu_sets[j];
			vector<uint> Cij;
			{
				Timer tm(2, "intersection");
				// set_intersection(Iui.begin(), Iui.end(), Iuj.begin(), Iuj.end(), inserter(Cij, Cij.begin()));
				intersection(graph.uedgeSet[U_star[i]], graph.uedgeSet[U_star[j]], Cij);
			}

			double gamma = 1.0 / (alpha + (double)Cij.size());
			{
				Timer tm(3, "addition");
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void FastExactSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph, const BPMaximalClique& fast_set){
	const vector<uint> &U_star = graph.m_vedges[vq];
	for (int i=0; i<U_star.size(); i++){
		for (int j=0; j<i; j++) {
			vector<uint> Cij;
			{
				Timer tm(2, "intersection");
				// fast_item_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
				Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
			}

			// statistic the counts
			// Accumulator::add(0, 1, "intersections");
			// Accumulator::add(1, Cij.size()<2, "invalid");

			double gamma = 1.0 / (alpha + (double)Cij.size());
			{
				Timer tm(3, "addition");
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void FastReorderSwing(int vq, double alpha, double eps, std::vector<double>& swing, const Graph& graph, const BPMaximalClique& fast_set){
	{
		Timer tm(4,"Preprocess");
		std::fill(swing.begin(), swing.end(), 0.0);
	}

	{
		Timer tm(5, "Estimation");

		const vector<uint> &U_star = graph.m_vedges[vq];
		for (int i=0; i<U_star.size(); i++){
			// uint ui = U_star[i];
			for (int j=0; j<i; j++) {
				// uint uj = U_star[j];
				vector<uint> Cij;
				{
					Timer tm(2, "intersection");
					Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
				}
				double gamma = 1.0 / (alpha + (double)Cij.size());
				{
					Timer tm(3, "addition");
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
	}
}
void DouBuckSwing(int vq, const Config& config, std::vector<double>& swing, const Graph& graph, const DouBuck& db){
	{
		Timer tm(4,"Preprocess");
		std::fill(swing.begin(), swing.end(), 0.0);
	}

	{
		Timer tm(5, "Estimation");

		const vector<uint> &U_star = graph.m_vedges[vq];
		for (int i=0; i<U_star.size(); i++){
			// uint ui = U_star[i];
			for (int j=0; j<i; j++) {
				// uint uj = U_star[j];
				vector<uint> Cij;
				{
					Timer tm(2, "intersection");
					uint ui = U_star[i], uj = U_star[j];
					int csz = 0;
					{
						Timer tmOI(6, "ord Intsct");
						Reorder_intersection(db.fast_set,graph,Cij,ui,uj);
					}
					csz = Cij.size();
					{
						Timer tmHI(7, "hot Intsct");
						auto temp = db.hot_set[ui] & db.hot_set[uj];
						size_t pos = temp._Find_first();
						while (pos < temp.size()) {
							Cij.push_back(pos);
							pos = temp._Find_next(pos);
						}
					}
					Accumulator::add(0, csz, "Ord size");
					Accumulator::add(1, Cij.size() - csz, "hot size");
					Accumulator::add(2, 1, "total user pairs");
					Accumulator::add(3, Cij.size() == csz, "Ord user pairs");
					Accumulator::add(4, csz == 0, "hot user pairs");
				}
				double gamma = 1.0 / (config.alpha + (double)Cij.size());
				{
					Timer tm(3, "addition");
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
	}
}
void DBMCSwing(int vq, const Config& config, std::vector<double>& swing, const Graph& graph, const DouBuck& db, std::mt19937& rng){
	{
		Timer tm(4,"Preprocess");
		std::fill(swing.begin(), swing.end(), 0.0);
	}

	if (graph.getVDeg(vq) < 2)
	{
		// cout << vq << " degree < 2, return" << endl;
		return;
	}

	vector<uint>U_star;
	int dvq = 0;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			// U_star[u]=dvq++;
			dvq++;
			U_star.push_back(u);
		}
	}
	// set random
	if (dvq < 2) return;
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 * graph.getNu() / graph.getM() * config.SampleRatio * std::pow(graph.getVDeg(vq), config.SamplePower);
	// double delta = 1.0 * graph.getNu() / graph.getM() * graph.getVDeg(vq);
	double probf = 1.0 / graph.getNv();
	double limit_r = 1.0 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 * limit_r * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon * delta) * log(1 / probf);
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;

	// cout << "detal: " << delta << ", probf: " << probf << ", limit_r: " << limit_r << ", n_rounds: " << n_rounds << endl;
	// cout << "vq: " << vq << ", dvq: " << dvq << ", n_rounds: " << n_rounds << ", gamma0: " << gamma0 << endl;

	{
		Timer tm(5, "Estimation");
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(rng);
			uint k2 = rand_u2(rng);
			if (k2>=k1) k2++;

			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");

				uint ui = U_star[k1], uj = U_star[k2];
				int csz = 0;
				{
					Timer tmOI(6, "ord Intsct");
					Reorder_intersection(db.fast_set,graph,Cij,ui,uj);
				}
				csz = Cij.size();
				{
					Timer tmHI(7, "hot Intsct");
					auto temp = db.hot_set[ui] & db.hot_set[uj];
					size_t pos = temp._Find_first();
					while (pos < temp.size()) {
						Cij.push_back(pos);
						pos = temp._Find_next(pos);
					}
				}
				Accumulator::add(0, csz, "Ord size");
				Accumulator::add(1, Cij.size() - csz, "hot size");
				Accumulator::add(2, 1, "total user pairs");
				Accumulator::add(3, Cij.size() == csz, "Ord user pairs");
				Accumulator::add(4, csz == 0, "hot user pairs");
			}

			{
				Timer tm3(3, "addition");
				double gamma =  gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}

void TruncSwing(int vq, std::vector<double>& swing, const Graph& graph,  const Config& config){
	vector<uint> U_star = random_sample(graph.m_vedges[vq], config.maxUserPerItem);
	vector<unordered_set<uint>> Iu_sets;
	{
		Timer tm(4, "prepare Iu sets");
		for (int i=0; i<U_star.size(); i++){
			uint ui = U_star[i];
			vector<uint> edge_sample = random_sample(graph.m_uedges[ui], config.maxClickPerUser);
			unordered_set<uint> Iui(edge_sample.begin(),edge_sample.end());
			Iu_sets.push_back(Iui);
		}
	}

	for (int i=0; i<U_star.size(); i++){
		for (int j=0; j<i; j++) {
			unordered_set<uint> &Iui = Iu_sets[i];
			unordered_set<uint> &Iuj = Iu_sets[j];
			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");
				// set_intersection(Iui.begin(), Iui.end(), Iuj.begin(), Iuj.end(), inserter(Cij, Cij.begin()));
				intersection(Iui, Iuj, Cij);
			}
			double gamma = 1.0 / (config.alpha + (double)Cij.size());
			{
				Timer tm3(3, "addition");
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void FastTruncSwing(int vq, std::vector<double>& swing, const Graph& graph,  const Config& config, const BPMaximalClique& fast_set){
	vector<uint> U_star = random_sample(graph.m_vedges[vq], config.maxUserPerItem);
	vector<unordered_set<uint>> Iu_sets;

	for (int i=0; i<U_star.size(); i++){
		for (int j=0; j<i; j++) {
			vector<uint> Cij;
			{
				Timer tm(2, "intersection");
				Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
			}
			double gamma = 1.0 / (config.alpha + (double)Cij.size());
			{
				Timer tm3(3, "addition");
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void MonteCarloSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	int n_rounds = 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, n_rounds, "SampleRnd");

	// cout << "detal: " << delta << ", probf: " << probf << ", limit_r: " << limit_r << ", n_rounds: " << n_rounds << endl;
	// cout << "vq: " << vq << ", dvq: " << dvq << ", n_rounds: " << n_rounds << ", gamma0: " << gamma0 << endl;
	for (int c=1; c<=n_rounds; c++) {
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		vector<uint> Cij;
		{
			Timer tm2(2, "intersection");
			// Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			intersection(graph.uedgeSet[U_star[k1]], graph.uedgeSet[U_star[k2]], Cij);
			// fast_item_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
		}

		double gamma =  gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
			}
		}
	}
}
void FastMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	int n_rounds = 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, n_rounds, "SampleRnd");

	// cout << "detal: " << delta << ", probf: " << probf << ", limit_r: " << limit_r << ", n_rounds: " << n_rounds << endl;
	// cout << "vq: " << vq << ", dvq: " << dvq << ", n_rounds: " << n_rounds << ", gamma0: " << gamma0 << endl;
	for (int c=1; c<=n_rounds; c++) {
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		vector<uint> Cij;
		{
			Timer tm2(2, "intersection");
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// fast_item_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
		}

		double gamma =  gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
			}
		}
	}
}
void GroupMCSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(0, n_rounds, "SampleRnd");

	unordered_map<pair<int,int>, int, PairHash> S;
	{
		Timer tm2(4, "Sampling");
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			S[{k1,k2}] += 1;
		}
	}
	// cout << "Sset: " << S.size() << " rounds: " << n_rounds << " fullset: " << dvq << endl;
	// unordered_map<uint,int>cnt;
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm2(2, "intersection");
			intersection(graph.uedgeSet[U_star[k1]], graph.uedgeSet[U_star[k2]], Cij);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << rcost << endl;
		}

		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
}
void FastGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(0, n_rounds, "SampleRnd");

	unordered_map<pair<int,int>, int, PairHash> S;
	{
		Timer tm2(4, "Sampling");
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			S[{k1,k2}] += 1;
		}
	}
	// cout << "Sset: " << S.size() << " rounds: " << n_rounds << " fullset: " << dvq << endl;
	// unordered_map<uint,int>cnt;
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm2(2, "intersection");
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << rcost << endl;
		}

		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastUSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, sqrt(3*log(2/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2)));
	// cout << "r: " << r << endl;
	// double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	if (fabs(r-1) < config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, ui, uj);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;

		for (int i=0;i<U_star.size();i++)
		{
			const uint ui = U_star[i];
			const auto& edges_i = graph.m_uedges[ui];
			const uint di = edges_i.size();
			const int Ii = (int)std::ceil(r * di);

			for (int j=0;j<i;j++)
			{
				const uint uj = U_star[j];
				const auto& edges_j = graph.m_uedges[uj];
				const uint dj = edges_j.size();
				const int Ij = (int)std::ceil(r * dj);

				HashSpace::clear();
				vector<uint> Cij;

				{
					Timer tm3(3,"Sampling");

					// cout << r << " " << Ii << " " << Ij << endl;
					std::uniform_int_distribution<int> rand_ui(0, di - 1);
					for (int k = 0; k < Ii; ++k)
					{
						HashSpace::insert(edges_i[rand_ui(RandomUtils::rng)]);
					}

					std::uniform_int_distribution<int> rand_uj(0, dj - 1);
					for (int k = 0; k < Ij; ++k)
					{
						uint v = edges_j[rand_uj(RandomUtils::rng)];
						if (HashSpace::query(v)) Cij.push_back(v);
					}

				}

				if (Cij.size() > 0)
				{
					Timer tm2(2, "intersection");
					int Csize = Cij.size();
					if (r<1)
					{
						Csize = fast_item_intersection(fast_set, graph, ui, uj);
						// cout << Cij.size() << " " << Csize << "\n";
					}
					double add = 1.0 / (r*r*(config.alpha+Csize));
					for (auto vt: Cij)
					{
						uint newId = graph.mapIndex[vt];
						swing[newId] += add;
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastUSS2(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random

	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	// FastRNG rng;
	// cout << "r: " << r << " eps: " << config.eps << endl;
	if (r > config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, ui, uj);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			uint di = graph.m_uedges[ui].size();
			const auto& edges_i = graph.m_uedges[ui];
			const int Ii = (int)std::ceil(r * di);
			std::uniform_int_distribution<int> rand_ui(0, di - 1);
			for (int j=i+1; j<U_star.size();j++)
			{
				uint uj = U_star[j];
				const auto& hash_j = graph.uedgeSet[uj];

				vector<uint> Cij;
				{
					Timer tm3(3,"Sampling");

					for (int k = 0; k < Ii; ++k)
					{
						uint v = edges_i[rand_ui(RandomUtils::rng)];
						// uint32_t r = rng.next();
						// uint32_t idx = fast_range(r, di);
						// uint v = edges_i[idx];

						// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
						if (hash_j.contains(v)) Cij.push_back(v);
					}
				}

				if (Cij.size() > 0)
				{
					Timer tm2(2, "intersection");
					int Csize = Cij.size();
					if (r<1)
					{
						Csize = fast_item_intersection(fast_set, graph, ui, uj);
						// Csize = Simple_intersection(graph, ui, uj);
						// cout << Cij.size() << " " << Csize << "\n";
					}
					double add = 1.0 * di / (Ii *(config.alpha+Csize));
					for (auto vt: Cij)
					{
						uint newId = graph.mapIndex[vt];
						swing[newId] += add;
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastUSS3(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
	// cout << "r: " << r << endl;
	// double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	FastRNG rng;
	if (r > config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, ui, uj);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}
	else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;

		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			uint di = graph.m_uedges[ui].size();
			if (r * di < 1)continue;
			const auto& edges_i = graph.m_uedges[ui];
			const int Ii = (int)std::ceil(r * di);
			for (int j=i+1; j<U_star.size();j++)
			{
				uint uj = U_star[j];
				const auto& hash_j = graph.uedgeSet[uj];

				vector<uint> Cij;

				{
					Timer tm3(3,"Sampling");

					std::uniform_int_distribution<int> rand_ui(0, di - 1);
					for (int k = 0; k < Ii; ++k)
					{
						uint v = edges_i[rand_ui(RandomUtils::rng)];
						// uint32_t r = rng.next();
						// uint32_t idx = fast_range(r, di);
						// uint v = edges_i[idx];

						// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
						if (hash_j.contains(v)) Cij.push_back(v);
					}
				}

				if (Cij.size() > 0)
				{
					Timer tm2(2, "intersection");
					int Csize = Cij.size();
					if (r<1)
					{
						Csize = fast_item_intersection(fast_set, graph, ui, uj);
						// Csize = Simple_intersection(graph, ui, uj);
						// cout << Cij.size() << " " << Csize << "\n";
					}
					double add = 1.0 * di / (Ii *(config.alpha+Csize));
					for (auto vt: Cij)
					{
						uint newId = graph.mapIndex[vt];
						swing[newId] += add;
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastUSS4(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
	// cout << "r: " << r << endl;
	// double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	if (r > config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, ui, uj);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;
		FastRNG rng;
		for (int i=0;i<U_star.size();i++)
		{
			for (int j=i+1;j<U_star.size();j++)
			{
				uint ui = U_star[i];
				uint uj = U_star[j];

				const UVertex& u = fast_set.graph[ui];
				const UVertex& v = fast_set.graph[uj];

				uint di = u.deg;
				uint dj = v.deg;
				double p = r;
				vector<uint> Cij;
				if (di < dj)
				{
					Timer tm3(3,"Sampling");
					const auto& edges = fast_set.pool_base + u.start;
					const auto& state = fast_set.pool_state + u.start;
					auto& hash = graph.uedgeMap[uj];
					const int Isize = (int)std::ceil(r * di);
					p = (double)Isize / di;
					for (int k = 0; k < Isize; ++k)
					{
						// int x = rand_ui(RandomUtils::rng)
						// uint v = edges[];
						uint32_t rg = rng.next();
						uint32_t idx = fast_range(rg, di);
						// cout << "idx: " << idx << " d: " << di << endl;
						int base = edges[idx];
						int stat = state[idx];
						// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
						auto it = hash.find(base);
						if (it != hash.end())
						{
							stat &= it->second;
							base <<= PACK_SHIFT;
							while (stat)
							{
								int x = (base | __builtin_ctz(stat));
								Cij.push_back(x);
								stat &= (stat - 1);
							}
						}
					}
				}
				else
				{
					Timer tm3(3,"Sampling");
					const auto& edges = fast_set.pool_base + v.start;
					const auto& state = fast_set.pool_state + v.start;
					auto& hash = graph.uedgeMap[ui];
					const int Isize = (int)std::ceil(r * dj);
					p = (double)Isize / dj;
					for (int k = 0; k < Isize; ++k)
					{
						// int x = rand_ui(RandomUtils::rng)
						// uint v = edges[];
						uint32_t rg = rng.next();
						uint32_t idx = fast_range(rg, dj);
						// cout << "idx: " << idx << " d: " << di << endl;
						int base = edges[idx];
						int stat = state[idx];
						// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
						auto it = hash.find(base);
						if (it != hash.end())
						{
							stat &= it->second;
							base <<= PACK_SHIFT;
							while (stat)
							{
								int x = (base | __builtin_ctz(stat));
								Cij.push_back(x);
								stat &= (stat - 1);
							}
						}
					}
				}

				if (Cij.size() > 0)
				{
					int Csize = 0;
					{
						Timer tm2(2, "intersection");
						Csize = fast_item_intersection(fast_set, graph, ui, uj);
						// vector<uint> tempC;
						// Reorder_intersection(fast_set, graph, tempC, ui, uj);
						// Csize = tempC.size();
					}
					double add = 1.0 / (p*(config.alpha+Csize));
					for (auto vt: Cij)
					{
						// uint newId = graph.mapIndex[vt];
						swing[vt] += add;
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastUSS5(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
	// cout << "r: " << r << endl;
	// double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	if (r > config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, ui, uj);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}
	else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;
		FastRNG rng;
		for (int i=0;i<U_star.size();i++)
		{
			for (int j=i+1;j<U_star.size();j++)
			{
				uint ui = U_star[i];
				uint uj = U_star[j];

				const UVertex& u = fast_set.graph[ui];
				const UVertex& v = fast_set.graph[uj];

				uint di = u.deg;
				uint dj = v.deg;
				if (min(di,dj) * r <= 2)
				{
					double p = r;
					vector<uint> Cij;
					if (di < dj)
					{
						Timer tm3(3,"Sampling");
						const auto& edges = fast_set.pool_base + u.start;
						const auto& state = fast_set.pool_state + u.start;
						auto& hash = graph.uedgeMap[uj];
						const int Isize = (int)std::ceil(r * di);
						p = (double)Isize / di;
						for (int k = 0; k < Isize; ++k)
						{
							// int x = rand_ui(RandomUtils::rng)
							// uint v = edges[];
							uint32_t rg = rng.next();
							uint32_t idx = fast_range(rg, di);
							// cout << "idx: " << idx << " d: " << di << endl;
							int base = edges[idx];
							int stat = state[idx];
							// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
							auto it = hash.find(base);
							if (it != hash.end())
							{
								stat &= it->second;
								base <<= PACK_SHIFT;
								while (stat)
								{
									int x = (base | __builtin_ctz(stat));
									Cij.push_back(x);
									stat &= (stat - 1);
								}
							}
						}
					}
					else
					{
						Timer tm3(3,"Sampling");
						const auto& edges = fast_set.pool_base + v.start;
						const auto& state = fast_set.pool_state + v.start;
						auto& hash = graph.uedgeMap[ui];
						const int Isize = (int)std::ceil(r * dj);
						p = (double)Isize / dj;
						for (int k = 0; k < Isize; ++k)
						{
							// int x = rand_ui(RandomUtils::rng)
							// uint v = edges[];
							uint32_t rg = rng.next();
							uint32_t idx = fast_range(rg, dj);
							// cout << "idx: " << idx << " d: " << di << endl;
							int base = edges[idx];
							int stat = state[idx];
							// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
							auto it = hash.find(base);
							if (it != hash.end())
							{
								stat &= it->second;
								base <<= PACK_SHIFT;
								while (stat)
								{
									int x = (base | __builtin_ctz(stat));
									Cij.push_back(x);
									stat &= (stat - 1);
								}
							}
						}
					}

					if (Cij.size() > 0)
					{
						int Csize = 0;
						{
							Timer tm2(2, "intersection");
							Csize = fast_item_intersection(fast_set, graph, ui, uj);
							// vector<uint> tempC;
							// Reorder_intersection(fast_set, graph, tempC, ui, uj);
							// Csize = tempC.size();
						}
						double add = 1.0 / (p*(config.alpha+Csize));
						for (auto vt: Cij)
						{
							// uint newId = graph.mapIndex[vt];
							swing[vt] += add;
						}
					}
				}
				else
				{
					int Csize = 0;
					{
						Timer tm2(2, "intersection");
						Csize = fast_item_intersection(fast_set, graph, ui, uj);
					}
					if (Csize == 0) continue;
					double p = r * (config.alpha+2) / (config.alpha + Csize);
					vector<uint> Cij;
					if (di < dj)
					{
						Timer tm3(3,"Sampling");
						const auto& edges = fast_set.pool_base + u.start;
						const auto& state = fast_set.pool_state + u.start;
						auto& hash = graph.uedgeMap[uj];
						const int Isize = (int)std::ceil(p * di);
						p = (double)Isize / di;
						for (int k = 0; k < Isize; ++k)
						{
							// int x = rand_ui(RandomUtils::rng)
							// uint v = edges[];
							uint32_t rg = rng.next();
							uint32_t idx = fast_range(rg, di);
							// cout << "idx: " << idx << " d: " << di << endl;
							int base = edges[idx];
							int stat = state[idx];
							// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
							auto it = hash.find(base);
							if (it != hash.end())
							{
								stat &= it->second;
								base <<= PACK_SHIFT;
								while (stat)
								{
									int x = (base | __builtin_ctz(stat));
									Cij.push_back(x);
									stat &= (stat - 1);
								}
							}
						}
					}
					else
					{
						Timer tm3(3,"Sampling");
						const auto& edges = fast_set.pool_base + v.start;
						const auto& state = fast_set.pool_state + v.start;
						auto& hash = graph.uedgeMap[ui];
						const int Isize = (int)std::ceil(p * dj);
						p = (double)Isize / dj;
						for (int k = 0; k < Isize; ++k)
						{
							// int x = rand_ui(RandomUtils::rng)
							// uint v = edges[];
							uint32_t rg = rng.next();
							uint32_t idx = fast_range(rg, dj);
							// cout << "idx: " << idx << " d: " << di << endl;
							int base = edges[idx];
							int stat = state[idx];
							// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
							auto it = hash.find(base);
							if (it != hash.end())
							{
								stat &= it->second;
								base <<= PACK_SHIFT;
								while (stat)
								{
									int x = (base | __builtin_ctz(stat));
									Cij.push_back(x);
									stat &= (stat - 1);
								}
							}
						}
					}

					if (Cij.size() > 0)
					{

						double add = 1.0 / (p * (config.alpha + Csize));
						for (auto vt: Cij)
						{
							// uint newId = graph.mapIndex[vt];
							swing[vt] += add;
						}
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void ConstUSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double lambda = 0.5 * dvq * (dvq -1) / (config.alpha + 2) * config.Lambda;
	double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
	// cout << "r: " << r << endl;
	// double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	// S.reserve(n_rounds);
	FastRNG rng;
	if (r > config.eps)
	{
		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			for (int j=0;j<i;j++)
			{
				Timer tm3(3,"Sampling");
				vector<uint> Cij;
				uint uj = U_star[j];
				{
					Timer tm2(2, "intersection");
					// Reorder_intersection(fast_set, graph, Cij, ui, uj);
					intersection(graph.uedgeSet[ui], graph.uedgeSet[uj], Cij);
					int Csize = Cij.size();
					double add = 1.0/(config.alpha+Csize);
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}
	else
	{
		// cout << "r: " << r << " deg: " << graph.getVDeg(vq) << endl;

		for (int i=0;i<U_star.size();i++)
		{
			uint ui = U_star[i];
			uint di = graph.m_uedges[ui].size();
			// if (r * di < 1)continue;
			const auto& edges_i = graph.m_uedges[ui];
			const int Ii = (int)std::ceil(r * di);
			for (int j=i+1; j<U_star.size();j++)
			{
				uint uj = U_star[j];
				const auto& hash_j = graph.uedgeSet[uj];

				vector<uint> Cij;

				{
					Timer tm3(3,"Sampling");

					std::uniform_int_distribution<int> rand_ui(0, di - 1);
					for (int k = 0; k < Ii; ++k)
					{
						uint v = edges_i[rand_ui(RandomUtils::rng)];
						// uint32_t r = rng.next();
						// uint32_t idx = fast_range(r, di);
						// uint v = edges_i[idx];

						// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
						if (hash_j.contains(v)) Cij.push_back(v);
					}
				}

				if (Cij.size() > 0)
				{
					Timer tm2(2, "intersection");
					int Csize = Cij.size();
					if (r<1)
					{
						// Csize = fast_item_intersection(fast_set, graph, ui, uj);
						Csize = intersection(graph.uedgeSet[ui], graph.uedgeSet[uj]);
						// Csize = Simple_intersection(graph, ui, uj);
						// cout << Cij.size() << " " << Csize << "\n";
					}
					double add = 1.0 * di / (Ii *(config.alpha+Csize));
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void FastGSS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return;
	vector<uint>U_star;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			U_star.push_back(u);
		}
	}
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 * config.beta / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(0, n_rounds, "SampleRnd");
	unordered_map<pair<uint,uint>, int, PairHash> S;
	// S.reserve(n_rounds);
	{
		Timer tm2(4, "Sampling");
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;

			uint ui = U_star[k1], uj = U_star[k2];
			if (ui > uj) swap(ui ,uj);
			if (graph.getUDeg(ui) < graph.getUDeg(uj) ) swap(ui ,uj);

			// Timer tm5(5, "HashTime");
			S[{ui,uj}] += 1;
		}
	}
	// cout << "Sset: " << S.size() << " rounds: " << n_rounds << " fullset: " << dvq << endl;
	// unordered_map<uint,int>cnt;
	// unordered_set<uint> Hash;
	for (auto [up, c]: S)
	{
		auto [ui, uj] = up;

		int Csize = 0;
		{
			Timer tm2(2, "intersection");
			Csize = fast_item_intersection(fast_set, graph, ui, uj);
		}
		if (Csize < 2)continue;
		{
			Timer tm3(3, "addition");
			double gamma =  c * gamma0 / (config.alpha + Csize);
			// Hash.clear();
			const int dui = graph.getUDeg(ui), duj = graph.getUDeg(uj);
			// const auto& uiEdge = graph.m_uedges[ui];
			const auto& ujEdge = graph.m_uedges[uj];
			// std::uniform_int_distribution<int> rand_ui(0, dui-1);
			const auto& uiHash = graph.uedgeSet[ui];
			std::uniform_int_distribution<int> rand_uj(0, duj-1);
			// for (int i=0, ed=dui/config.beta; i<ed; i++)
			// {
			// 	int vt = uiEdge[rand_ui(RandomUtils::rng)];
			// 	if (vt != vq) Hash.insert(vt);
			// }

			for (int i=0, ed=duj/config.beta; i<ed; i++)
			{
				int vt = ujEdge[rand_uj(RandomUtils::rng)];
				// int vt = ujEdge[i];
				if (uiHash.find(vt) != uiHash.end())
				{
					swing[vt] += gamma;
				}
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void TestGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(0, n_rounds, "SampleRnd");

	unordered_map<pair<int,int>, int, PairHash> S;
	for (int c=1; c<=n_rounds; c++) {
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		if (k1 > k2) swap(k1 ,k2);
		S[{k1,k2}] += 1;
	}
	// cout << "Sset: " << S.size() << " rounds: " << n_rounds << " fullset: " << dvq << endl;
	// unordered_map<uint,int>cnt;
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm2(2, "intersection");
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << Cij.size() << endl;
		}
		Accumulator::add(1, Cij.size()>1, "valid");
		Accumulator::add(2, Cij.size()<2, "invalid");
		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void CompGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	// double gamma0 = 1.0 / n_rounds ;
	// Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(0, n_rounds, "SampleRnd");

	unordered_map<pair<int,int>, int, PairHash> S;
	for (int c=1; c<=n_rounds; c++) {
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		if (k1 > k2) swap(k1 ,k2);
		S[{k1,k2}] += 1;
	}
	// cout << "Sset: " << S.size() << " rounds: " << n_rounds << " fullset: " << dvq << endl;
	// unordered_map<uint,int>cnt;
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm2(2, "SetInt");
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << Cij.size() << endl;
		}
		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm3(3, "SizeInt");
			fast_item_intersection(fast_set, graph, U_star[k1], U_star[k2]);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << Cij.size() << endl;
		}
		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		// vector<uint> Cij1;
		{
			Timer tm4(4, "SimpInt");
			Simple_intersection(graph, U_star[k1], U_star[k2]);
			// Simple_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// Naive_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			// cout << min(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << max(graph.getUDeg(U_star[k1]), graph.getUDeg(U_star[k2])) << '\t' << tm2.cost()/TIMES_PER_SEC << '\t' << Cij.size() << endl;
		}
		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			for (auto vt: Cij) {
				swing[vt] += gamma;
				// cnt[vt] += 1;
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
void CompExt(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto &U_star = graph.vSorted[vq];
	{
		Timer tm(2, "SetIntersection");
		for (int i=0; i<U_star.size(); i++)
		{
			for (int j=0; j<i; j++)
			{
				vector<uint> Cij;
				{
					// fast_item_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
					Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
				}
				swing[vq] += Cij.size();
			}
		}
	}
	{
		Timer tm(3, "Sizeintersection");
		for (int i=0; i<U_star.size(); i++)
		{
			for (int j=0; j<i; j++)
			{
				int size = 0;
				{
					size = fast_item_intersection(fast_set,graph,U_star[i],U_star[j]);
					// Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
				}
				swing[vq] += size;
			}
		}
	}
	// for (auto [vt,c]: cnt)
	// {
	// 	cout << graph.oriIndex[vt] << "\t" << c << endl;
	// }
}
long long AdaptGNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return 0;
	vector<uint>U_star;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			U_star.push_back(u);
		}
	}
	int dvq = U_star.size();
	if (dvq < 2) return 0;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);;
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, n_rounds, "SampleRnd");

	unordered_map<pair<int,int>, int, PairHash> S;
	for (int c=1; c<=n_rounds; c++) {
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		if (k1 > k2) swap(k1 ,k2);
		S[{k1,k2}] += 1;
	}
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		{
			Timer tm2(2, "intersection");
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
		}

		double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				swing[vt] += gamma;
			}
		}
	}
	return n_rounds;
}
void FastGWS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return ;

	vector<pair<uint,uint>> UsampList;
	vector<pair<uint,uint>> U_star;
	double udeg_sum = 0;
	vector<unordered_set<uint>> Iu_sets;
	{
		for (auto& u: graph.m_vedges[vq]) {
			if (graph.m_udeg[u] >= 2){
				U_star.emplace_back(u,graph.getUDeg(u));
				UsampList.emplace_back(U_star.size()-1,graph.getUDeg(u)-1);
				udeg_sum += graph.getUDeg(u)-1;
			}
		}
		if (U_star.size() < 2) return ;
	}
	int dvq = U_star.size();
	int beta = config.beta;
	/* set parameter */
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	// double ratio = min(max(limit_r/delta, 1.0/config.epsilon),60.0);
	double ratio = 1.0 / config.Lambda;
	// if (graph.getVDeg(vq) < 500) ratio = 200;
	// else if (graph.getVDeg(vq) < 1000) ratio = 100;
	// else if (graph.getVDeg(vq) < 2000) ratio = 90;
	// else if (graph.getVDeg(vq) < 10000) ratio = 70;
	int n_rounds = 4.0 * (ratio * (1 + config.epsilon/3) - 1) / (config.epsilon * config.epsilon) * log(2 / config.probf);
	// int n_rounds = 4300;
	n_rounds = n_rounds / beta;
	Accumulator::add(0, n_rounds * beta, "n_rounds");

	unordered_map<uint,vector<pair<uint,double>>>S;
	/* Sampling */
	{
		Timer tm_samp(3, "Sampling");
		map<uint,uint> Su;
		// std::uniform_int_distribution<int> rand_u(0, dvq-1);
		Alias alias_sample(UsampList);
		for (int c=1; c<=n_rounds; c++)
		{
			// uint ku = rand_u(rng);
			uint ku = alias_sample.generateRandom(RandomUtils::rng);
			Su[ku] += 1;
		}
		// cout << "set U: " << Su.size() << endl;
		std::uniform_int_distribution<int> rand_v; // 在循环外创建
		using param_t = std::uniform_int_distribution<int>::param_type;
		for (auto [ku, cu]: Su){
			auto [u,udeg] = U_star[ku];
			// std::uniform_int_distribution<int> rand_v(0, udeg-2);
			rand_v.param(param_t(0, udeg - 2));
			unordered_map<uint,uint>Sv;
			for (int i = 0; i<cu * beta; i++)
			{
				uint kv = rand_v(RandomUtils::rng);
				uint v = graph.m_uedges[u][kv];
				if (v == vq)
				{
					v = graph.m_uedges[u][udeg-1];
				}
				Sv[v] += 1;
			}
			for (auto [v, cv]: Sv)
			{
				S[v].emplace_back(u,cv);
			}
		}
	}
	{
		Timer tm_est(4, "Estimation");
		// map<pair<uint,uint>,int> InsTemp;
		const double scale_factor = udeg_sum / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * udeg_sum;
		// const double scale_factor = dvq / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * dvq;
		int tot_rounds = 0;
		bool flag_opt = false;
		if (flag_opt){
			int topk = config.maxTopk;
			vector<pair<int,uint>>S_sort;
			S_sort.reserve(S.size());
			priority_queue<double,vector<double>, greater<double>>top_values;
			for (auto& [vt, Sv_Cnt]:S)
			{
				if (Sv_Cnt.size() < 2) continue;
				S_sort.emplace_back(vt, Sv_Cnt.size());
			}
			sort(S_sort.begin(), S_sort.end(), [](const auto&a, const auto&b){return a.second > b.second;});
			for (auto& [vt, Sv_sz]:S_sort){
				// if (run_cnt-- == 0) break;
				auto& Sv_Cnt = S[vt];
				if (Sv_Cnt.size()<2) continue;
				// cout << vt << "\t" << Sv_Cnt.size() << endl;
				int hit_pair = 0;
				Accumulator::add(1, 0.5 * Sv_Cnt.size() * (Sv_Cnt.size()-1), "sum S^2");
				{
					Timer tm_calc(6, "Calculation in Est");
					double sum = 0, sum2=0;
					for (int i = 0 ; i < Sv_Cnt.size(); i++) sum+=Sv_Cnt[i].second, sum2+=Sv_Cnt[i].second*Sv_Cnt[i].second;
					sum = (sum*sum - sum2)/2;
					double partial_swing = 0.0;
					bool early_stop = false;
					for (int i = 0 ; i < Sv_Cnt.size(); i++)
					{
						// if (Sv_Cnt[i].second >3) cout << "multiV: " << Sv_Cnt[i].first << " " << Sv_Cnt[i].second << endl;
						if (early_stop) break;
						for (int j = i+1; j < Sv_Cnt.size(); j++)
						{
							hit_pair += Sv_Cnt[i].second * Sv_Cnt[j].second;
							if (top_values.size() >= topk)
							{
								if (partial_swing + sum /(config.alpha + 2) < top_values.top())
								{
									early_stop = true;
									break;
								}
							}
							sum -= Sv_Cnt[i].second * Sv_Cnt[j].second;
							Accumulator::add(3, Sv_Cnt[i].second * Sv_Cnt[j].second, "hit pair");
							uint ui = Sv_Cnt[i].first, uj = Sv_Cnt[j].first;
							if (ui > uj)
							{
								// swap(ui, uj);
								assert(0);
							}
							int Csize=0;
							/*if (InsTemp.count({ui,uj}))
							{
								Csize = InsTemp[{ui,uj}];
								map_win += 1;
							}
							else*/
							{
								// const UVertex& u = fast_set.graph[U_star[ui].first];
								// const UVertex& v = fast_set.graph[U_star[uj].first];
								// const UVertex& u = fast_set.graph[ui];
								// const UVertex& v = fast_set.graph[uj];
								// int *pool_base = fast_set.pool_base;
								// PackState *pool_state = fast_set.pool_state;
								// Csize = bp_intersect_filter_simd4x_count(pool_base + u.start, pool_state + u.start, u.deg,
								// 												 pool_base + v.start, pool_state + v.start, v.deg);
								Csize= fast_item_intersection(fast_set, graph, ui, uj);
								// InsTemp[{ui,uj}] = Csize;
								// intersection(Iu_sets[ui], Iu_sets[uj], Cij);
								// Csize = Cij.size();
							}
							partial_swing += 1.0 / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
							// partial_swing += (U_star[ui].second + U_star[uj].second-2) / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
						}
					}
					// cout << vt << "\t" << partial_swing << endl;
					// cout << vt << "\t" << (0.5 * Sv_Cnt.size()*(Sv_Cnt.size()-1)) << endl;
					swing[graph.mapIndex[vt]] = partial_swing * scale_factor;
					tot_rounds += Sv_Cnt.size() * (Sv_Cnt.size()-1) / 2;
					if (top_values.size() < topk) top_values.push(partial_swing);
					else
					{
						if (partial_swing > top_values.top())
						{
							top_values.pop();
							top_values.push(partial_swing);
						}
					}
				}
			}
		}else
		{
			for (auto& [ori_vt, Sv_Cnt]:S){
				auto vt = graph.mapIndex[ori_vt];
				if (Sv_Cnt.size()<2) continue;
				// cout << vt << "\t" << Sv_Cnt.size() << endl;
				Accumulator::add(1, 0.5 * Sv_Cnt.size() * (Sv_Cnt.size()-1), "sum S^2");
				{
					Timer tm_calc(6, "Calculation in Est");
					double partial_swing = 0.0;
					for (int i = 0 ; i < Sv_Cnt.size(); i++)
					{
						for (int j = i+1; j < Sv_Cnt.size(); j++)
						{
							uint ui = Sv_Cnt[i].first, uj = Sv_Cnt[j].first;
							int Csize=0;
							vector<uint>Cij;
							{
								Timer tm_pre(2, "intsetction");
								Csize= fast_item_intersection(fast_set, graph, ui, uj);
								// Reorder_intersection(fast_set, graph, Cij, ui, uj);
								// Csize = Cij.size();
							}
							// vector<uint> Cij;
							// {
							// 	Timer tm2(2, "intersection");
							// 	Reorder_intersection(fast_set, graph, Cij, ui, uj);
							// 	Csize = Cij.size();
							// }
							partial_swing += 1.0 / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
							// double gamma =  scale_factor / (config.alpha + Csize) / Csize * Sv_Cnt[i].second * Sv_Cnt[j].second ;
							// {
							// 	Timer tm3(3, "addition");
							// 	for (auto vt: Cij) {
							// 		swing[vt] += gamma;
							// 	}
							// }
						}
					}
					// cout << vt << "\t" << (0.5 * Sv_Cnt.size()*(Sv_Cnt.size()-1)) << endl;
					// tot_rounds += Sv_Cnt.size() * (Sv_Cnt.size()-1) / 2;
					swing[vt] = partial_swing * scale_factor;
				}
			}
		}
		// cout << "map saves:\t" << map_win << endl;
		// cout << "tot_rounds:\t" << tot_rounds << endl;
	}
}
void ASCGWS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return ;

	vector<pair<uint,uint>> UsampList;
	vector<pair<uint,uint>> U_star;
	double udeg_sum = 0;
	vector<unordered_set<uint>> Iu_sets;
	{
		// Timer tm_pre(0, "prepare U^sart");
		for (auto& u: graph.m_vedges[vq]) {
			if (graph.m_udeg[u] >= 2){
				U_star.emplace_back(u,graph.getUDeg(u));
				UsampList.emplace_back(U_star.size()-1,graph.getUDeg(u)-1);
				udeg_sum += graph.getUDeg(u)-1;
			}
		}
		if (U_star.size() < 2) return ;
	}
	int dvq = U_star.size();
	int beta = config.beta;
	/* set parameter */
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	// double ratio = min(max(limit_r/delta, 1.0/config.epsilon),60.0);
	double ratio = 1.0 / config.Lambda;
	// if (graph.getVDeg(vq) < 500) ratio = 200;
	// else if (graph.getVDeg(vq) < 1000) ratio = 100;
	// else if (graph.getVDeg(vq) < 2000) ratio = 90;
	// else if (graph.getVDeg(vq) < 10000) ratio = 70;
	int n_rounds = 4.0 * (ratio * (1 + config.epsilon/3) - 1) / (config.epsilon * config.epsilon) * log(2 / config.probf);
	n_rounds = n_rounds / beta;
	Accumulator::add(0, n_rounds * beta, "n_rounds");
	if (n_rounds > udeg_sum )
	{
		for (int i=0; i<U_star.size(); i++){
			for (int j=0; j<i; j++) {
				vector<uint> Cij;
				{
					Timer tm(2, "intersection");
					// fast_item_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
					Reorder_intersection(fast_set,graph,Cij,U_star[i].first,U_star[j].first);
				}
				double gamma = 1.0 / (config.alpha + (double)Cij.size());
				{
					Timer tm(3, "addition");
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
		return ;
	}
	unordered_map<uint,vector<pair<uint,double>>>S;
	/* Sampling */
	{
		Timer tm_samp(3, "Sampling");
		map<uint,uint> Su;
		// std::uniform_int_distribution<int> rand_u(0, dvq-1);
		Alias alias_sample(UsampList);
		for (int c=1; c<=n_rounds; c++)
		{
			// uint ku = rand_u(rng);
			uint ku = alias_sample.generateRandom(RandomUtils::rng);
			Su[ku] += 1;
		}
		// cout << "set U: " << Su.size() << endl;
		std::uniform_int_distribution<int> rand_v; // 在循环外创建
		using param_t = std::uniform_int_distribution<int>::param_type;
		for (auto [ku, cu]: Su){
			auto [u,udeg] = U_star[ku];
			// std::uniform_int_distribution<int> rand_v(0, udeg-2);
			rand_v.param(param_t(0, udeg - 2));
			unordered_map<uint,uint>Sv;
			for (int i = 0; i<cu * beta; i++)
			{
				uint kv = rand_v(RandomUtils::rng);
				uint v = graph.m_uedges[u][kv];
				if (v == vq)
				{
					v = graph.m_uedges[u][udeg-1];
				}
				Sv[v] += 1;
			}
			for (auto [v, cv]: Sv)
			{
				S[v].emplace_back(u,cv);
			}
		}
	}
	{
		Timer tm_est(4, "Estimation");
		// map<pair<uint,uint>,int> InsTemp;
		const double scale_factor = udeg_sum / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * udeg_sum;
		// const double scale_factor = dvq / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * dvq;
		int tot_rounds = 0;
		bool flag_opt = false;
		if (flag_opt){
			int topk = config.maxTopk;
			vector<pair<int,uint>>S_sort;
			S_sort.reserve(S.size());
			priority_queue<double,vector<double>, greater<double>>top_values;
			for (auto& [vt, Sv_Cnt]:S)
			{
				if (Sv_Cnt.size() < 2) continue;
				S_sort.emplace_back(vt, Sv_Cnt.size());
			}
			sort(S_sort.begin(), S_sort.end(), [](const auto&a, const auto&b){return a.second > b.second;});
			for (auto& [vt, Sv_sz]:S_sort){
				// if (run_cnt-- == 0) break;
				auto& Sv_Cnt = S[vt];
				if (Sv_Cnt.size()<2) continue;
				// cout << vt << "\t" << Sv_Cnt.size() << endl;
				int hit_pair = 0;
				Accumulator::add(1, 0.5 * Sv_Cnt.size() * (Sv_Cnt.size()-1), "sum S^2");
				{
					Timer tm_calc(6, "Calculation in Est");
					double sum = 0, sum2=0;
					for (int i = 0 ; i < Sv_Cnt.size(); i++) sum+=Sv_Cnt[i].second, sum2+=Sv_Cnt[i].second*Sv_Cnt[i].second;
					sum = (sum*sum - sum2)/2;
					double partial_swing = 0.0;
					bool early_stop = false;
					for (int i = 0 ; i < Sv_Cnt.size(); i++)
					{
						// if (Sv_Cnt[i].second >3) cout << "multiV: " << Sv_Cnt[i].first << " " << Sv_Cnt[i].second << endl;
						if (early_stop) break;
						for (int j = i+1; j < Sv_Cnt.size(); j++)
						{
							hit_pair += Sv_Cnt[i].second * Sv_Cnt[j].second;
							if (top_values.size() >= topk)
							{
								if (partial_swing + sum /(config.alpha + 2) < top_values.top())
								{
									early_stop = true;
									break;
								}
							}
							sum -= Sv_Cnt[i].second * Sv_Cnt[j].second;
							Accumulator::add(3, Sv_Cnt[i].second * Sv_Cnt[j].second, "hit pair");
							uint ui = Sv_Cnt[i].first, uj = Sv_Cnt[j].first;
							if (ui > uj)
							{
								// swap(ui, uj);
								assert(0);
							}
							int Csize=0;
							/*if (InsTemp.count({ui,uj}))
							{
								Csize = InsTemp[{ui,uj}];
								map_win += 1;
							}
							else*/
							{
								// const UVertex& u = fast_set.graph[U_star[ui].first];
								// const UVertex& v = fast_set.graph[U_star[uj].first];
								// const UVertex& u = fast_set.graph[ui];
								// const UVertex& v = fast_set.graph[uj];
								// int *pool_base = fast_set.pool_base;
								// PackState *pool_state = fast_set.pool_state;
								// Csize = bp_intersect_filter_simd4x_count(pool_base + u.start, pool_state + u.start, u.deg,
								// 												 pool_base + v.start, pool_state + v.start, v.deg);
								Csize= fast_item_intersection(fast_set, graph, ui, uj);
								// InsTemp[{ui,uj}] = Csize;
								// intersection(Iu_sets[ui], Iu_sets[uj], Cij);
								// Csize = Cij.size();
							}
							partial_swing += 1.0 / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
							// partial_swing += (U_star[ui].second + U_star[uj].second-2) / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
						}
					}
					// cout << vt << "\t" << partial_swing << endl;
					// cout << vt << "\t" << (0.5 * Sv_Cnt.size()*(Sv_Cnt.size()-1)) << endl;
					swing[graph.mapIndex[vt]] = partial_swing * scale_factor;
					tot_rounds += Sv_Cnt.size() * (Sv_Cnt.size()-1) / 2;
					if (top_values.size() < topk) top_values.push(partial_swing);
					else
					{
						if (partial_swing > top_values.top())
						{
							top_values.pop();
							top_values.push(partial_swing);
						}
					}
				}
			}
		}else
		{
			Timer tm_calc(6, "Calculation in Est");
			for (auto& [ori_vt, Sv_Cnt]:S){
				auto vt = graph.mapIndex[ori_vt];
				// auto vt = ori_vt;
				if (Sv_Cnt.size()<2) continue;
				// cout << vt << "\t" << Sv_Cnt.size()*(Sv_Cnt.size()-1)/2 << endl;
				Accumulator::add(1, 0.5 * Sv_Cnt.size() * (Sv_Cnt.size()-1), "sum S^2");
				{
					double partial_swing = 0.0;
					for (int i = 0 ; i < Sv_Cnt.size(); i++)
					{
						for (int j = i+1; j < Sv_Cnt.size(); j++)
						{
							uint ui = Sv_Cnt[i].first, uj = Sv_Cnt[j].first;
							int Csize=0;
							{
								Timer tm2(2, "intersection");
								Csize= fast_item_intersection(fast_set, graph, ui, uj);
							}
							// vector<uint> Cij;
							// {
							// 	Timer tm2(2, "intersection");
							// 	Reorder_intersection(fast_set, graph, Cij, ui, uj);
							// 	Csize = Cij.size();
							// }
							partial_swing += 1.0 / (config.alpha + Csize) * Sv_Cnt[i].second * Sv_Cnt[j].second;
							// double gamma =  scale_factor / (config.alpha + Csize) / Csize * Sv_Cnt[i].second * Sv_Cnt[j].second ;
							// {
							// 	Timer tm3(3, "addition");
							// 	for (auto vt: Cij) {
							// 		swing[vt] += gamma;
							// 	}
							// }
						}
					}
					// cout << vt << "\t" << (0.5 * Sv_Cnt.size()*(Sv_Cnt.size()-1)) << endl;
					// tot_rounds += Sv_Cnt.size() * (Sv_Cnt.size()-1) / 2;
					swing[vt] = partial_swing * scale_factor;
				}
			}
		}
		// cout << "map saves:\t" << map_win << endl;
		// cout << "tot_rounds:\t" << tot_rounds << endl;
	}
}
void FastUNS(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return ;

	vector<uint> U_star;
	{
		// Timer tm_pre(0, "prepare U^sart");
		for (auto& u: graph.m_vedges[vq]) {
			if (graph.m_udeg[u] >= 2){
				U_star.emplace_back(u);
			}
		}
		if (U_star.size() < 2) return ;
	}
	int dvq = U_star.size();
	/* set parameter */
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	// double ratio = min(max(limit_r/delta, 1.0/config.epsilon),60.0);
	double ratio = 1.0 / config.Lambda;
	// int n_rounds = 4.0 * (ratio * (1 + config.epsilon/3) - 1) / (config.epsilon * config.epsilon) * log(2 / config.probf);
	int n_rounds = sqrt(2 * 2.0 / config.Lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf));;

	Accumulator::add(0, n_rounds, "n_rounds");
	unordered_map<uint,int>Su;
	/* Sampling */
	{
		Timer tm_samp(3, "Sampling");
		std::uniform_int_distribution<int> rand_u(0, dvq-1);
		for (int c=1; c<=n_rounds; c++)
		{
			uint ku = rand_u(RandomUtils::rng);
			Su[ku] += 1;
		}
	}
	{
		Timer tm_est(4, "Estimation");
		// map<pair<uint,uint>,int> InsTemp;
		// const double scale_factor = udeg_sum / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * udeg_sum;
		const double scale_factor = dvq / (static_cast<double>(n_rounds) * (n_rounds - 1)) * dvq;
		// const double scale_factor = 1.0 / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) ;
		// const double scale_factor = dvq / (static_cast<double>(n_rounds) * (n_rounds - 1) * beta * beta) * dvq;
		int tot_rounds = 0;
		bool flag_opt = false;
		unordered_map<uint,uint>cnt;
		for (auto it = Su.begin();it != Su.end();it++)
		{
			for (auto it2 = Su.begin();it2 != it;it2++)
			{
				auto [ui,cu] = *it;
				auto [uj, cv] = *it2;

				// cout << ui << ' ' << uj << endl;
				int Csize=0;
				vector<uint>Cij;
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph,Cij, U_star[ui], U_star[uj]);
					Csize=Cij.size();
				}
				double gamma = 1.0 / (config.alpha + Csize) * cu * cv * scale_factor;
				// double gamma =  scale_factor / (config.alpha + Csize) / Csize * Sv_Cnt[i].second * Sv_Cnt[j].second ;
				{
					Timer tm3(3, "addition");
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
		// cout << "map saves:\t" << map_win << endl;
		// cout << "tot_rounds:\t" << tot_rounds << endl;
	}
}
void PRSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;

	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * U_star.size() * (U_star.size() -1) / 2 / (config.alpha + 2);
	int n_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, n_rounds, "SampleRnd");

	// if (dvq < 10000) n_rounds = min(n_rounds, dvq * (dvq - 1) / 2);
	// cout << "n_rounds: " << n_rounds << endl;
	double Deg = graph.getVNDeg(vq);
	// cout <<"d:" << dvq << " rho:" << (1+dvq/Deg) <<  " 1: " << config.AdaptRho * dvq * (dvq -1)  << " 2: " << (1 + dvq / Deg) * 2 * n_rounds <<endl;
	if (config.AdaptRho * dvq * (dvq -1) < (1 + dvq / Deg) * 2 * n_rounds)
	{
		double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));

			for (int i=0;i<U_star.size();i++)
			{
				uint ui = U_star[i];
				vector<uint> Cij;
				for (int j=0;j<i;j++)
				{
					uint uj = U_star[j];
					{
						Timer tm2(2, "intersection");
						intersection(graph.uedgeSet[ui], graph.uedgeSet[uj], Cij);
					}
					double add = 1.0 / (config.alpha + Cij.size());
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
	}
	else
	{
		std::uniform_int_distribution<int> rand_u1(0, dvq-1);
		std::uniform_int_distribution<int> rand_u2(0, dvq-2);
		double gamma0 = 0.5 * dvq * (dvq -1) / n_rounds;
		if (gamma0 > 10)
		{
			vector<uint> Cij;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				{
					Timer tm2(2, "intersection");
					intersection(graph.uedgeSet[U_star[k1]], graph.uedgeSet[U_star[k2]], Cij);
				}
				double gamma =  gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
		else
		{
			unordered_map<pair<int,int>, int, PairHash> S;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				S[{k1,k2}] += 1;
			}
			for (auto [up, c]: S)
			{
				auto [k1, k2] = up;
				vector<uint> Cij;
				{
					Timer tm2(2, "intersection");
					intersection(graph.uedgeSet[U_star[k1]], graph.uedgeSet[U_star[k2]], Cij);
				}
				double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void FastASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;

	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * U_star.size() * (U_star.size() -1) / 2 / (config.alpha + 2);
	int n_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, n_rounds, "SampleRnd");

	// if (dvq < 10000) n_rounds = min(n_rounds, dvq * (dvq - 1) / 2);
	// cout << "n_rounds: " << n_rounds << endl;
	double Deg = graph.getVNDeg(vq);
	// cout <<"d:" << dvq << " rho:" << (1+dvq/Deg) <<  " 1: " << config.AdaptRho * dvq * (dvq -1)  << " 2: " << (1 + dvq / Deg) * 2 * n_rounds <<endl;
	if (config.AdaptRho * dvq * (dvq -1) < (1 + dvq / Deg) * 2 * n_rounds)
	{
		double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
		if (r > config.eps)
		{
			for (int i=0;i<U_star.size();i++)
			{
				uint ui = U_star[i];
				vector<uint> Cij;
				for (int j=0;j<i;j++)
				{
					uint uj = U_star[j];
					{
						Timer tm2(2, "intersection");
						Reorder_intersection(fast_set, graph, Cij, ui, uj);
					}
					double add = 1.0 / (config.alpha + Cij.size());
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
		else
		{
			FastRNG rng;
			for (int i=0;i<U_star.size();i++)
			{
				for (int j=i+1;j<U_star.size();j++)
				{
					uint ui = U_star[i];
					uint uj = U_star[j];

					const UVertex& u = fast_set.graph[ui];
					const UVertex& v = fast_set.graph[uj];

					uint di = u.deg;
					uint dj = v.deg;
					if (min(di,dj) * r <= 2)
					{
						double p = r;
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(r * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(r * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						if (Cij.size() > 0)
						{
							int Csize = 0;
							{
								Timer tm2(2, "intersection");
								Csize = fast_item_intersection(fast_set, graph, ui, uj);
								// vector<uint> tempC;
								// Reorder_intersection(fast_set, graph, tempC, ui, uj);
								// Csize = tempC.size();
							}
							double add = 1.0 / (p * (config.alpha+Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
					else
					{
						int Csize = 0;
						{
							Timer tm2(2, "intersection");
							Csize = fast_item_intersection(fast_set, graph, ui, uj);
						}
						if (Csize == 0) continue;
						double p = r * (config.alpha+2) / (config.alpha + Csize);
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(p * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(p * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}

						if (Cij.size() > 0)
						{

							double add = 1.0 / (p * (config.alpha + Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		std::uniform_int_distribution<int> rand_u1(0, dvq-1);
		std::uniform_int_distribution<int> rand_u2(0, dvq-2);
		double gamma0 = 0.5 * dvq * (dvq -1) / n_rounds;
		if (gamma0 > 10)
		{
			vector<uint> Cij;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
				}
				double gamma =  gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
		else
		{
			unordered_map<pair<int,int>, int, PairHash> S;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				S[{k1,k2}] += 1;
			}
			for (auto [up, c]: S)
			{
				auto [k1, k2] = up;
				vector<uint> Cij;
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
				}
				double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}
}
void FastKASC(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);

	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * U_star.size() * (U_star.size() -1) / 2 / (config.alpha + 2);
	int total_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	int n_rounds = config.PostRefineSigma * total_rounds;
	int reRounds = (total_rounds - n_rounds) ;
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, total_rounds, "TotalRnd");
	Accumulator::add(2, n_rounds, "SampleRnd");
	Accumulator::add(3, reRounds, "RefineRnd");

	// if (dvq < 10000) n_rounds = min(n_rounds, dvq * (dvq - 1) / 2);
	// cout << "n_rounds: " << n_rounds << endl;
	int vqMap = graph.mapIndex[vq];
	double Deg = graph.getVNDeg(vq);
	if (U_star.size() * (U_star.size() -1) * config.AdaptRho  < 2 * total_rounds * (1 + U_star.size() / Deg))
	{
		double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
		if (r > config.eps)
		{
			for (int i=0;i<U_star.size();i++)
			{
				uint ui = U_star[i];
				vector<uint> Cij;
				for (int j=0;j<i;j++)
				{
					uint uj = U_star[j];
					{
						Timer tm2(2, "intersection");
						Reorder_intersection(fast_set, graph, Cij, ui, uj);
					}
					double add = 1.0 / (config.alpha + Cij.size());
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
		else
		{
			FastRNG rng;
			for (int i=0;i<U_star.size();i++)
			{
				for (int j=i+1;j<U_star.size();j++)
				{
					uint ui = U_star[i];
					uint uj = U_star[j];

					const UVertex& u = fast_set.graph[ui];
					const UVertex& v = fast_set.graph[uj];

					uint di = u.deg;
					uint dj = v.deg;
					if (min(di,dj) * r <= 2)
					{
						double p = r;
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(r * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(r * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						if (Cij.size() > 0)
						{
							int Csize = 0;
							{
								Timer tm2(2, "intersection");
								Csize = fast_item_intersection(fast_set, graph, ui, uj);
								// vector<uint> tempC;
								// Reorder_intersection(fast_set, graph, tempC, ui, uj);
								// Csize = tempC.size();
							}
							double add = 1.0 / (p * (config.alpha+Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
					else
					{
						int Csize = 0;
						{
							Timer tm2(2, "intersection");
							Csize = fast_item_intersection(fast_set, graph, ui, uj);
						}
						if (Csize == 0) continue;
						double p = r * (config.alpha+2) / (config.alpha + Csize);
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(p * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(p * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}

						if (Cij.size() > 0)
						{

							double add = 1.0 / (p * (config.alpha + Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
				}
			}
		}
		return ;
	}
	unordered_set<uint> Act;
	vector<double> variance(swing.size());
	vector<int> exRound(swing.size());
	vector<int> hit(swing.size());
	// cout << "case:" << cas << "sample range: " << last_rounds << " " << n_rounds << endl;
	/* Sampling */
	double gamma0 = 0.5 * dvq * (dvq -1) / n_rounds;
	// cout << "n: " << n_rounds << " gamma: " << gamma0 << endl;
	if (gamma0 > 10)
	{
		vector<uint> Cij;
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			{
				Timer tm2(2, "intersection");
				Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			}
			double gamma =  0.5 * dvq * (dvq -1) / (config.alpha + (double)Cij.size());
			for (auto vt: Cij) {
				if (vt == vqMap) continue;
				Act.insert(vt);
				hit[vt] += 1;

				const int r_old = exRound[vt];
				exRound[vt] += 1;
				const int ex_r = exRound[vt];
				const double diff = gamma - swing[vt];
				swing[vt] += 1 * diff / ex_r;
				if (ex_r > 1)
				{
					// variance[vt] = variance[vt] * (ex_r-2) / (ex_r-1) + wt / (ex_r-1) * (delta_ij - swing[vt]);
					variance[vt] = variance[vt] + ((1.0 * r_old / ex_r * diff * diff - variance[vt]) * 1)/(ex_r-1);
					// variance[vt] = (variance[vt] * (r_old - 1)  + diff * diff / ex_r * r_old * c)/ (ex_r-1);
				}
			}
		}
	}
	else
	{
		unordered_map<pair<int,int>, int, PairHash> S;
		for (int r=0; r<n_rounds; r++)
		{
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			S[{k1,k2}] += 1;
		}
		for (auto [up, c]: S)
		{
			auto [k1, k2] = up;
			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");
				Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			}

			if (Cij.size() == 0) continue;
			double delta_ij =  0.5 * dvq * (dvq-1) / (config.alpha + (double)Cij.size());
			{
				for (auto vt: Cij) {
					if (vt == vqMap) continue;
					Act.insert(vt);
					hit[vt] += c;

					const int r_old = exRound[vt];
					exRound[vt] += c;
					const int ex_r = exRound[vt];
					const double diff = delta_ij - swing[vt];
					swing[vt] += c * diff / ex_r;
					if (ex_r > c)
					{
						// variance[vt] = variance[vt] * (ex_r-2) / (ex_r-1) + wt / (ex_r-1) * (delta_ij - swing[vt]);
						variance[vt] = variance[vt] + ((1.0 * r_old / ex_r * diff * diff - variance[vt]) * c)/(ex_r-1);
						// variance[vt] = (variance[vt] * (r_old - 1)  + diff * diff / ex_r * r_old * c)/ (ex_r-1);
					}
				}
			}
		}
	}

	/* Boundary/Candidate Identification */
	priority_queue<pair<double, uint>, vector<pair<double, uint>>, greater<pair<double, uint>>>Q;
	vector<uint>Refine;
	// cout << "Act: " << Act.size() << endl;
	int K = config.maxTopk;
	{
		Timer tm4(5, "Boundary Identification");
		for (auto vt:Act)
		{
			const int r = exRound[vt];
			swing[vt] *= 1.0 * r / n_rounds;
			variance[vt] = variance[vt] +  1.0 * (n_rounds - r) * (swing[vt] / r * n_rounds * swing[vt] - variance[vt]) / (n_rounds -1);
			exRound[vt] = n_rounds;
			if (Q.size() < K + config.ext_k || Q.top().first < swing[vt])
			{
				if (Q.size() >= K + config.ext_k) Q.pop();
				Q.push({swing[vt],vt});
			}
			// cout << "vt: " << vt << " sw: " << swing[vt] << " var: " << variance[vt] << endl;
		}
		if (Q.size()<=K)return;
		if (config.PostRefineType == "VarBound")//VarBound
		{
			struct node
			{
				unsigned int id;
				double l,r;
			};
			int Bq_len = Q.size();
			vector<node>Bq(Bq_len);
			double beta_kmin = zeta;
			double beta_kmax = 0;
			while (!Q.empty())
			{
				auto [sw, x] = Q.top();
				double lambda = EmpiricalFun(n_rounds, variance[x], zeta, config.probf);
				// cout << "vt: " << graph.oriIndex[x] << " sw: " << sw << " var: " << variance[x] << " hit: " << hit[x] << " LB: "<< sw-lambda << " RB: " << sw+lambda << endl;
				Bq[--Bq_len] = {x, sw-lambda, sw+lambda};
				Q.pop();
			}
			if (Bq.size()>K)
			{// k+1 k+ext_k
				beta_kmax = max(beta_kmax, Bq[K].r);
				beta_kmin = min(beta_kmin, Bq[K-1].l);
				for (int i=K-1, cnt=0; i >=0 && cnt<config.RfWid;i--,cnt++)
				{
					if (Bq[i].l < beta_kmax)
					{
						Refine.push_back(Bq[i].id);
					}
				}
				for (int i=K, cnt=0 ;i < Bq.size(); i++, cnt++)
				{
					if (Bq[i].r > beta_kmin)
					{
						Refine.push_back(Bq[i].id);
						// if (Refine.size() > config.ext_k * 2) break;
					}
				}
			}
			else {// 1 k
				return;
			}
		}
		else if (config.PostRefineType == "Naive")
		{
			while (!Q.empty() && Refine.size() < config.ext_k * 2)
			{
				auto [sw, x] = Q.top();
				Refine.push_back(x);
				Q.pop();
			}
		}
		else if (config.PostRefineType == "Threshold")
		{
			vector<pair<double, uint>> Bq(Q.size());
			int Bq_len = Q.size();
			while (!Q.empty())
			{
				Bq[--Bq_len] = Q.top();
				Q.pop();
			}
			if (Bq.size() > K)
			{
				for (int i = K-1; i >= 0 ; i--)
				{
					if (Bq[i].first - Bq[K].first < Bq[K].first * config.epsilon) Refine.push_back(Bq[i].second);
				}
			}
			for (int i=K; i < Bq.size(); i++)
			{
				if (Bq[K-1].first - Bq[i].first < Bq[i].first * config.epsilon) Refine.push_back(Bq[i].second);
			}
		}
	}
	if (config.NoRefine)return;
	/* Adaptive Pairwise Refinement*/
	// Refine.clear();
	// int cnt = Refine.size();
	{
		Accumulator::add(4, Refine.size(), "RefineSize");
		Timer tm_ref(6,"Refine");

		// for (auto vt:Refine)
		// {
		// 	cout << "vt: " << graph.oriIndex[vt] << " hit: " << hit[vt] << " BFsw: " <<  swing[vt] << endl;
		// }

		sort(Refine.begin(), Refine.end());
		for (auto vt:Refine)swing[vt] = 0;
		int lenRf = Refine.size();
		// cout << "dvq: " << dvq << ' ' << "reRounds: " << reRounds << " lenRf: " <<lenRf<< endl;
		if (2 * reRounds >= dvq)
		{
			vector<int>Iu(dvq);
			vector<int>Uvalid;
			Uvalid.reserve(dvq);
			for (int i=0;i<dvq;i++)
			{
				int u = U_star[i];
				int uNeighbors = 0;
				auto& hash = graph.uedgeMap[u];
				for (int k=0;k<lenRf;k++)
				{
					int vt = Refine[k];
					int base = vt >> PACK_SHIFT;
					int stat = 1 << (vt & PACK_MASK);
					auto it = hash.find(base);
					if (it != hash.end())
					{
						if (stat & it->second) uNeighbors |= 1<<k;
					}
				}
				if (uNeighbors)
				{
					Uvalid.push_back(u);
					Iu[Uvalid.size()-1] = uNeighbors;
				}
			}
			dvq = Uvalid.size();
			// cout << "new dvq: " << dvq << endl;
			if (0.5 * dvq * (dvq-1) < reRounds)
			{
				for (int i=0;i<Uvalid.size();i++)
				{
					uint ui = Uvalid[i];
					for (int j=0;j<i;j++)
					{
						uint uj = Uvalid[j];
						int tp = Iu[i] & Iu[j];
						if (tp == 0) continue;
						int Csize = fast_item_intersection(fast_set, graph, ui, uj);
						double add = 1.0 / (config.alpha + Csize);
						while (tp)
						{
							int vt = Refine[__builtin_ctz(tp)];
							swing[vt] += add;
							tp &= (tp - 1);
						}
					}
				}
			}
			else
			{
				std::uniform_int_distribution<int> rand_u1(0, dvq-1);
				std::uniform_int_distribution<int> rand_u2(0, dvq-2);
				double gamma0 = 0.5 * dvq * (dvq -1) / reRounds;
				// cout << "gamma: " << gamma0 << endl;
				if (gamma0 > 2)
				{
					for (int c=1; c<=reRounds; c++) {
						uint i = rand_u1(RandomUtils::rng);
						uint j = rand_u2(RandomUtils::rng);
						if (j>=i) j++;
						int ui = Uvalid[i], uj = Uvalid[j];

						int tp = Iu[i] & Iu[j];
						if (tp == 0) continue;
						int Csize = fast_item_intersection(fast_set, graph, ui, uj);
						double add = gamma0 / (config.alpha + Csize);
						while (tp)
						{
							uint vt = Refine[__builtin_ctz(tp)];
							swing[vt] += add;
							tp &= (tp - 1);
						}
					}
					// for (auto vt:Refine)
					// {
					// 	cout << "new1 vt: " << graph.oriIndex[vt] << " hit: " << hit[vt] << " BFsw: " <<  swing[vt] << endl;
					// }
				}
				else
				{
					unordered_map<pair<int,int>, int, PairHash> S;
					for (int c=1; c<=reRounds; c++) {
						uint i = rand_u1(RandomUtils::rng);
						uint j = rand_u2(RandomUtils::rng);
						if (j>=i) j++;
						if (i > j) swap(i ,j);
						S[{i,j}] += 1;
					}
					for (auto [up, c]: S)
					{
						auto [i, j] = up;
						int ui = Uvalid[i], uj = Uvalid[j];

						int tp = Iu[i] & Iu[j];
						if (tp == 0) continue;
						int Csize = fast_item_intersection(fast_set, graph, ui, uj);
						double add = c * gamma0 / (config.alpha + Csize);
						while (tp)
						{
							uint vt = Refine[__builtin_ctz(tp)];
							swing[vt] += add;
							tp &= (tp - 1);
						}
					}

					// for (auto vt:Refine)
					// {
					// 	cout << "new2 vt: " << graph.oriIndex[vt] << " hit: " << hit[vt] << " BFsw: " <<  swing[vt] << endl;
					// }
				}
			}
		}
		else
		{
			std::uniform_int_distribution<int> rand_u1(0, dvq-1);
			std::uniform_int_distribution<int> rand_u2(0, dvq-2);
			double gamma0 = 0.5 * dvq * (dvq -1) / reRounds;
			vector<uint> Cij;
			Cij.reserve(Refine.size());
			if (gamma0 > 5)
			{
				for (int c=1; c<=reRounds; c++) {
					uint i = rand_u1(RandomUtils::rng);
					uint j = rand_u2(RandomUtils::rng);
					if (j>=i) j++;
					int ui = U_star[i], uj = U_star[j];
					auto &hashi = graph.uedgeMap[ui];
					auto &hashj = graph.uedgeMap[uj];
					bool opt = 0;
					if (hashi.size() > hashj.size()) opt = 1;
					Cij.clear();
					for (int k=0;k<lenRf;k++)
					{
						int vt = Refine[k];
						int base = vt >> PACK_SHIFT;
						int stat = 1 << (vt & PACK_MASK);

						if (!opt)
						{
							auto it1 = hashi.find(base);
							if (it1 != hashi.end())
							{
								if (stat & it1->second)
								{
									auto it2 = hashj.find(base);
									if (it2 != hashj.end())
									{
										if (stat & it2->second)
										{
											Cij.push_back(vt);
										}
									}
								}
							}
						}
						else
						{
							auto it1 = hashj.find(base);
							if (it1 != hashj.end())
							{
								if (stat & it1->second)
								{
									auto it2 = hashi.find(base);
									if (it2 != hashi.end())
									{
										if (stat & it2->second)
										{
											Cij.push_back(vt);
										}
									}
								}
							}
						}
					}
					if (Cij.size()==0)continue;
					int Csize = fast_item_intersection(fast_set, graph, ui, uj);
					double add = gamma0 / (config.alpha + Csize);
					for (auto vt:Cij){
						swing[vt] += add;
					}
				}
			}
			else
			{
				unordered_map<pair<int,int>, int, PairHash> S;
				for (int c=1; c<=reRounds; c++) {
					uint i = rand_u1(RandomUtils::rng);
					uint j = rand_u2(RandomUtils::rng);
					if (j>=i) j++;
					if (i > j) swap(i ,j);
					S[{i,j}] += 1;
				}
				for (auto [up, c]: S)
				{
					auto [i, j] = up;
					int ui = U_star[i], uj = U_star[j];
					auto &hashi = graph.uedgeMap[ui];
					auto &hashj = graph.uedgeMap[uj];
					bool opt = 0;
					if (hashi.size() > hashj.size()) opt = 1;
					Cij.clear();
					for (int k=0;k<lenRf;k++)
					{
						int vt = Refine[k];
						int base = vt >> PACK_SHIFT;
						int stat = 1 << (vt & PACK_MASK);

						if (!opt)
						{
							auto it1 = hashi.find(base);
							if (it1 != hashi.end())
							{
								if (stat & it1->second)
								{
									auto it2 = hashj.find(base);
									if (it2 != hashj.end())
									{
										if (stat & it2->second)
										{
											Cij.push_back(vt);
										}
									}
								}
							}
						}
						else
						{
							auto it1 = hashj.find(base);
							if (it1 != hashj.end())
							{
								if (stat & it1->second)
								{
									auto it2 = hashi.find(base);
									if (it2 != hashi.end())
									{
										if (stat & it2->second)
										{
											Cij.push_back(vt);
										}
									}
								}
							}
						}
					}
					if (Cij.size()==0)continue;
					int Csize = fast_item_intersection(fast_set, graph, ui, uj);
					double add = gamma0 / (config.alpha + Csize);
					for (auto vt:Cij){
						swing[vt] += add;
					}
				}
			}
		}
		// Accumulator::add(5, Refine.size(),"Refine size");
	}
	// cout << "Refine: " << Refine.size() << " Exact Refine: "<< cnt << endl;
}
void FastKASC_2(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	const auto& U_star = graph.vSorted[vq];
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);

	double zeta = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	double lambda = config.Lambda * U_star.size() * (U_star.size() -1) / 2 / (config.alpha + 2);
	int total_rounds = 2.0 * zeta / lambda * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	int n_rounds = config.PostRefineSigma * total_rounds;
	int reRounds = (total_rounds - n_rounds) ;
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, total_rounds, "TotalRnd");
	Accumulator::add(2, n_rounds, "SampleRnd");
	Accumulator::add(3, reRounds, "RefineRnd");

	// if (dvq < 10000) n_rounds = min(n_rounds, dvq * (dvq - 1) / 2);
	// cout << "n_rounds: " << n_rounds << endl;
	int vqMap = graph.mapIndex[vq];
	double Deg = graph.getVNDeg(vq);
	if (U_star.size() * (U_star.size() -1) * config.AdaptRho  < 2 * total_rounds * (1 + U_star.size() / Deg))
	{
		double r = min(1.0, 2*(config.epsilon/3+1)*log(1/config.probf)/(config.epsilon*config.epsilon)/lambda/(config.alpha + 2));
		if (r > config.eps)
		{
			for (int i=0;i<U_star.size();i++)
			{
				uint ui = U_star[i];
				vector<uint> Cij;
				for (int j=0;j<i;j++)
				{
					uint uj = U_star[j];
					{
						Timer tm2(2, "intersection");
						Reorder_intersection(fast_set, graph, Cij, ui, uj);
					}
					double add = 1.0 / (config.alpha + Cij.size());
					for (auto vt: Cij)
					{
						swing[vt] += add;
					}
				}
			}
		}
		else
		{
			FastRNG rng;
			for (int i=0;i<U_star.size();i++)
			{
				for (int j=i+1;j<U_star.size();j++)
				{
					uint ui = U_star[i];
					uint uj = U_star[j];

					const UVertex& u = fast_set.graph[ui];
					const UVertex& v = fast_set.graph[uj];

					uint di = u.deg;
					uint dj = v.deg;
					if (min(di,dj) * r <= 2)
					{
						double p = r;
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(r * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(r * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						if (Cij.size() > 0)
						{
							int Csize = 0;
							{
								Timer tm2(2, "intersection");
								Csize = fast_item_intersection(fast_set, graph, ui, uj);
								// vector<uint> tempC;
								// Reorder_intersection(fast_set, graph, tempC, ui, uj);
								// Csize = tempC.size();
							}
							double add = 1.0 / (p * (config.alpha+Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
					else
					{
						int Csize = 0;
						{
							Timer tm2(2, "intersection");
							Csize = fast_item_intersection(fast_set, graph, ui, uj);
						}
						if (Csize == 0) continue;
						double p = r * (config.alpha+2) / (config.alpha + Csize);
						vector<uint> Cij;
						if (di < dj)
						{
							const auto& edges = fast_set.pool_base + u.start;
							const auto& state = fast_set.pool_state + u.start;
							auto& hash = graph.uedgeMap[uj];
							const int Isize = (int)std::ceil(p * di);
							p = (double)Isize / di;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, di);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}
						else
						{
							const auto& edges = fast_set.pool_base + v.start;
							const auto& state = fast_set.pool_state + v.start;
							auto& hash = graph.uedgeMap[ui];
							const int Isize = (int)std::ceil(p * dj);
							p = (double)Isize / dj;
							for (int k = 0; k < Isize; ++k)
							{
								// int x = rand_ui(RandomUtils::rng)
								// uint v = edges[];
								uint32_t rg = rng.next();
								uint32_t idx = fast_range(rg, dj);
								// cout << "idx: " << idx << " d: " << di << endl;
								int base = edges[idx];
								int stat = state[idx];
								// if (hash_j.find(v) != hash_j.end()) Cij.push_back(v);
								auto it = hash.find(base);
								if (it != hash.end())
								{
									stat &= it->second;
									base <<= PACK_SHIFT;
									while (stat)
									{
										int x = (base | __builtin_ctz(stat));
										Cij.push_back(x);
										stat &= (stat - 1);
									}
								}
							}
						}

						if (Cij.size() > 0)
						{

							double add = 1.0 / (p * (config.alpha + Csize));
							for (auto vt: Cij)
							{
								swing[vt] += add;
							}
						}
					}
				}
			}
		}
		return ;
	}
	if (reRounds < dvq)
	{
		std::uniform_int_distribution<int> rand_u1(0, dvq-1);
		std::uniform_int_distribution<int> rand_u2(0, dvq-2);
		double gamma0 = 0.5 * dvq * (dvq -1) / n_rounds;
		if (gamma0 > 10)
		{
			vector<uint> Cij;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
				}
				double gamma =  gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}else
		{
			unordered_map<pair<int,int>, int, PairHash> S;
			for (int c=1; c<=n_rounds; c++) {
				uint k1 = rand_u1(RandomUtils::rng);
				uint k2 = rand_u2(RandomUtils::rng);
				if (k2>=k1) k2++;
				if (k1 > k2) swap(k1 ,k2);
				S[{k1,k2}] += 1;
			}
			for (auto [up, c]: S)
			{
				auto [k1, k2] = up;
				vector<uint> Cij;
				{
					Timer tm2(2, "intersection");
					Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
				}
				double gamma =  c * gamma0 / (config.alpha + (double)Cij.size());
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
		return ;
	}
	unordered_set<uint> Act;
	vector<double> variance(swing.size());
	vector<int> exRound(swing.size());
	vector<int> hit(swing.size());
	// cout << "case:" << cas << "sample range: " << last_rounds << " " << n_rounds << endl;
	/* Sampling */
	double gamma0 = 0.5 * dvq * (dvq -1) / n_rounds;
	// cout << "n: " << n_rounds << " gamma: " << gamma0 << endl;
	if (gamma0 > 10)
	{
		vector<uint> Cij;
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			{
				Timer tm2(2, "intersection");
				Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			}
			double gamma =  0.5 * dvq * (dvq -1) / (config.alpha + (double)Cij.size());
			for (auto vt: Cij) {
				if (vt == vqMap) continue;
				Act.insert(vt);
				hit[vt] += 1;

				const int r_old = exRound[vt];
				exRound[vt] += 1;
				const int ex_r = exRound[vt];
				const double diff = gamma - swing[vt];
				swing[vt] += 1 * diff / ex_r;
				if (ex_r > 1)
				{
					// variance[vt] = variance[vt] * (ex_r-2) / (ex_r-1) + wt / (ex_r-1) * (delta_ij - swing[vt]);
					variance[vt] = variance[vt] + ((1.0 * r_old / ex_r * diff * diff - variance[vt]) * 1)/(ex_r-1);
					// variance[vt] = (variance[vt] * (r_old - 1)  + diff * diff / ex_r * r_old * c)/ (ex_r-1);
				}
			}
		}
	}
	else
	{
		unordered_map<pair<int,int>, int, PairHash> S;
		for (int r=0; r<n_rounds; r++)
		{
			uint k1 = rand_u1(RandomUtils::rng);
			uint k2 = rand_u2(RandomUtils::rng);
			if (k2>=k1) k2++;
			if (k1 > k2) swap(k1 ,k2);
			S[{k1,k2}] += 1;
		}
		for (auto [up, c]: S)
		{
			auto [k1, k2] = up;
			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");
				Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			}

			if (Cij.size() == 0) continue;
			double delta_ij =  0.5 * dvq * (dvq-1) / (config.alpha + (double)Cij.size());
			{
				for (auto vt: Cij) {
					if (vt == vqMap) continue;
					Act.insert(vt);
					hit[vt] += c;

					const int r_old = exRound[vt];
					exRound[vt] += c;
					const int ex_r = exRound[vt];
					const double diff = delta_ij - swing[vt];
					swing[vt] += c * diff / ex_r;
					if (ex_r > c)
					{
						// variance[vt] = variance[vt] * (ex_r-2) / (ex_r-1) + wt / (ex_r-1) * (delta_ij - swing[vt]);
						variance[vt] = variance[vt] + ((1.0 * r_old / ex_r * diff * diff - variance[vt]) * c)/(ex_r-1);
						// variance[vt] = (variance[vt] * (r_old - 1)  + diff * diff / ex_r * r_old * c)/ (ex_r-1);
					}
				}
			}
		}
	}

	/* Boundary/Candidate Identification */
	priority_queue<pair<double, uint>, vector<pair<double, uint>>, greater<pair<double, uint>>>Q;
	vector<uint>Refine;
	// cout << "Act: " << Act.size() << endl;
	int K = config.maxTopk;
	{
		Timer tm4(5, "Boundary Identification");
		for (auto vt:Act)
		{
			const int r = exRound[vt];
			swing[vt] *= 1.0 * r / n_rounds;
			variance[vt] = variance[vt] +  1.0 * (n_rounds - r) * (swing[vt] / r * n_rounds * swing[vt] - variance[vt]) / (n_rounds -1);
			exRound[vt] = n_rounds;
			if (Q.size() < K + config.ext_k || Q.top().first < swing[vt])
			{
				if (Q.size() >= K + config.ext_k) Q.pop();
				Q.push({swing[vt],vt});
			}
			// cout << "vt: " << vt << " sw: " << swing[vt] << " var: " << variance[vt] << endl;
		}
		if (config.PostRefineType == "VarBound")//VarBound
		{
			struct node
			{
				unsigned int id;
				double l,r;
			};
			int Bq_len = Q.size();
			vector<node>Bq(Bq_len);
			double beta_kmin = zeta;
			double beta_kmax = 0;
			while (!Q.empty())
			{
				auto [sw, x] = Q.top();
				double lambda = EmpiricalFun(n_rounds, variance[x], zeta, config.probf);
				// cout << "vt: " << graph.oriIndex[x] << " sw: " << sw << " var: " << variance[x] << " hit: " << hit[x] << " LB: "<< sw-lambda << " RB: " << sw+lambda << endl;
				Bq[--Bq_len] = {x, sw-lambda, sw+lambda};
				Q.pop();
			}
			if (Bq.size()>K)
			{// k+1 k+ext_k
				beta_kmax = max(beta_kmax, Bq[K].r);
			}
			{// 1 k
				beta_kmin = min(beta_kmin, Bq[K-1].l);
			}
			// cout << "min: " << beta_kmin << " max: " << beta_kmax << endl;
			for (int i=0; i < K && i < Bq.size();i++)
			{
				if (Bq[i].l < beta_kmax)
				{
					Refine.push_back(Bq[i].id);
				}
			}
			for (int i=K ;i < Bq.size(); i++)
			{
				if (Bq[i].r > beta_kmin)
				{
					Refine.push_back(Bq[i].id);
					// if (Refine.size() > config.ext_k * 2) break;
				}
			}
		}
		else if (config.PostRefineType == "Naive")
		{
			while (!Q.empty() && Refine.size() < config.ext_k * 2)
			{
				auto [sw, x] = Q.top();
				Refine.push_back(x);
				Q.pop();
			}
		}
		else if (config.PostRefineType == "Threshold")
		{
			vector<pair<double, uint>> Bq(Q.size());
			int Bq_len = Q.size();
			while (!Q.empty())
			{
				Bq[--Bq_len] = Q.top();
				Q.pop();
			}
			if (Bq.size() > K)
			{
				for (int i = K-1; i >= 0 ; i--)
				{
					if (Bq[i].first - Bq[K].first < Bq[K].first * config.epsilon) Refine.push_back(Bq[i].second);
				}
			}
			for (int i=K; i < Bq.size(); i++)
			{
				if (Bq[K-1].first - Bq[i].first < Bq[i].first * config.epsilon) Refine.push_back(Bq[i].second);
			}
		}
	}
	if (config.NoRefine)return;
	/* Adaptive Pairwise Refinement*/
	// Refine.clear();
	// int cnt = Refine.size();
	{
		Accumulator::add(4, Refine.size(), "RefineSize");
		Timer tm_ref(6,"Refine");
		// reRounds = reRounds / max((double)Refine.size(), 1.0);
		int total_budget = reRounds;
		unordered_set<uint>Sq(U_star.begin(), U_star.end());
		int lpt=max(0,(int)Refine.size() - config.ext_k*2), rpt=(int)Refine.size() - 1 , st=0;
		int reSize = (rpt-lpt+1);
		// cout << "l: " << lpt << " r: " << rpt << endl;
		while (lpt<=rpt)
		{
			int vt = -1;
			if (st==0) vt = Refine[lpt++];
			else vt = Refine[rpt--];
			st^=1;

			int bgtRounds = (total_budget + reSize - 1) / reSize;
			reSize -= 1;
			if (bgtRounds < hit[vt])
			{
				Accumulator::add(6, 1, "RefineSkip");
				continue;
			}
			int intersection_cost = min((int)graph.getOriVDeg(vt), dvq);
			if (bgtRounds < intersection_cost)
			{
				Accumulator::add(6, 1, "RefineSkip");
				continue;
			};
			total_budget -= bgtRounds;
			vector<uint> common_users;
			double Deg_tmp = 0;
			{
				Timer tm_ref(7,"ComUser");
				if (graph.getOriVDeg(vt) < dvq)
				{
					int oriv = vt;
					if (graph.getReo()) oriv = graph.oriIndex[vt];
					for (auto u: graph.m_vedges[oriv])
					{
						if (Sq.count(u))
						{
							common_users.push_back(u);
							Deg_tmp += graph.getUDeg(u);
						}
					}
				}else
				{
					int base = vt >> 5;
					int stat = 1<< (vt & 31);
					for (auto u: U_star)
					{
						const auto &hash = graph.uedgeMap[u];
						auto it = hash.find(base);
						if (it != hash.end())
						{
							if (it->second & stat)
							{
								common_users.push_back(u);
								Deg_tmp += graph.getUDeg(u);
							}
						}
					}
				}
			}

			cout << "vt: " << graph.oriIndex[vt] << " commom: " << common_users.size() << " hit: " << hit[vt] << " budget: " << bgtRounds << " BFsw: " <<  swing[vt] << endl;
			if (common_users.size() < 2)continue;
			const int common_size = common_users.size();
			if (config.AdaptRho * common_size * (common_size-1) < 2 * bgtRounds * (1 + common_users.size()/Deg_tmp))
			{
				swing[vt] = 0;
				for (int i = 0; i < common_size ;i++)
				{
					for (int j=i+1; j < common_size;j++)
					{
						uint ui = common_users[i], uj = common_users[j];
						int Csize = fast_item_intersection(fast_set, graph, ui, uj);
						swing[vt] += 1.0 / (config.alpha + Csize);
					}
				}
			}
			else
			{
				swing[vt] = 0;
				std::uniform_int_distribution<int> rd1(0, common_size-1);
				std::uniform_int_distribution<int> rd2(0, common_size-2);
				double delta_ij = 0.5 * common_size * (common_size-1) / bgtRounds ;
				for (int _=1; _<bgtRounds; _++)
				{
					uint i = rd1(RandomUtils::rng);
					uint j = rd2(RandomUtils::rng);
					if (i>=j) j++;
					uint ui = common_users[i], uj = common_users[j];
					int Csize = fast_item_intersection(fast_set, graph, ui, uj);
					swing[vt] += delta_ij / (config.alpha + Csize);
				}
			}
			// cout << "Refine: " << graph.oriIndex[vt] << " sw: "<< swing[vt] << " size: " << common_size << endl;
		}
		// Accumulator::add(5, Refine.size(),"Refine size");
	}
	// cout << "Refine: " << Refine.size() << " Exact Refine: "<< cnt << endl;
}
void FastASC_2(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set){
	if (graph.getVDeg(vq) < 2) return;

	vector<uint>U_star;
	vector<unordered_set<uint>> Iu_sets;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			U_star.push_back(u);
		}
	}
	int dvq = U_star.size();
	if (dvq < 2) return;
	// set random
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	double delta = 1.0 / (graph.getM()/graph.getNu() + config.alpha) * dvq;
	double limit_r = 0.5 * dvq * (dvq -1) / (config.alpha + 2);
	long long total_rounds = config.SampleRatio * 2.0 * min(limit_r/delta, 60.0) * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / config.probf);
	Accumulator::add(0, 0.5 * dvq * (dvq-1), "BruteCnt");
	Accumulator::add(1, total_rounds, "SampleRnd");

	long long n_rounds = total_rounds / 2;
	// if (dvq < 10000) n_rounds = min(n_rounds, dvq * (dvq - 1) / 2);
	// cout << "n_rounds: " << n_rounds << endl;
	if (U_star.size() * (U_star.size()-1) / 2 < total_rounds)
	{
		for (int i=0; i<U_star.size(); i++){
			for (int j=0; j<i; j++) {
				vector<uint> Cij;
				{
					Timer tm(2, "intersection");
					// fast_item_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
					Reorder_intersection(fast_set,graph,Cij,U_star[i],U_star[j]);
				}
				double gamma = 1.0 / (config.alpha + (double)Cij.size());
				{
					Timer tm(3, "addition");
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
		return ;
	}

	priority_queue<pair<double, uint>, vector<pair<double, uint>>, greater<pair<double, uint>>>Q;
	vector<double> variance(swing.size());
	vector<int> exRound(swing.size());
	vector<int> hit(swing.size());
	// cout << "case:" << cas << "sample range: " << last_rounds << " " << n_rounds << endl;
	/* Sampling */
	unordered_map<pair<int,int>, int, PairHash> S;
	for (int r=0; r<n_rounds; r++)
	{
		uint k1 = rand_u1(RandomUtils::rng);
		uint k2 = rand_u2(RandomUtils::rng);
		if (k2>=k1) k2++;
		if (k1 > k2) swap(k1 ,k2);
		S[{k1,k2}] += 1;
	}
	double temp = 0.5 * dvq * (dvq-1) / n_rounds;
	set<int>Act;
	int K = config.maxTopk;
	for (auto [up, c]: S)
	{
		auto [k1, k2] = up;
		vector<uint> Cij;
		{
			Timer tm2(2, "intersection");
			// intersection(Iu_sets[k1], Iu_sets[k2], Cij);
			Reorder_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
		}

		if (Cij.size() == 0) continue;
		double delta_ij =  temp / (config.alpha + (double)Cij.size());
		{
			Timer tm3(3, "addition");
			for (auto vt: Cij) {
				if (vt == vq) continue;
				hit[vt] += c;
				Act.insert(vt);
				const int r_old = exRound[vt];
				exRound[vt] += c;
				const int ex_r = exRound[vt];
				const double sw_old = r_old == 0? 0 : swing[vt] * n_rounds / r_old;
				swing[vt] += c * delta_ij;
				const double sw = swing[vt] * n_rounds / ex_r;
				variance[vt] += (- r_old / n_rounds * sw_old * sw_old * (n_rounds-r_old)
					+ ex_r / n_rounds * sw * sw * (n_rounds-ex_r)
					+ (delta_ij * n_rounds  - sw_old) * (delta_ij * n_rounds - sw) * c) / (n_rounds-1);

				// if (Q.size() < K + config.ext_k || Q.top().first < swing[vt])
				// {
				// 	if (Act.find(vt) == Act.end())
				// 	{
				// 		if (Q.size() >= K + config.ext_k)
				// 		{
				// 			do
				// 			{
				// 				auto [sw,v] = Q.top();
				// 				if (swing[v] != sw)
				// 				{
				// 					Q.pop();
				// 					Q.push({swing[v],v});
				// 				}
				// 				else break;
				// 			}while (1);
				// 			if (Q.top().first < swing[vt])
				// 			{
				// 				Act.erase(Q.top().second);
				// 				Q.pop();
				// 				Act.insert(vt);
				// 				Q.push({swing[vt],vt});
				// 			}
				// 		}else
				// 		{
				// 			Act.insert(vt);
				// 			Q.push({swing[vt],vt});
				// 		}
				// 	}
				// }
			}
		}
	}

	/* Boundary/Candidate Identification */
	vector<uint>Refine;
	// cout << "Act: " << Act.size() << endl;
	{
		Timer tm4(5, "Boundary Identification");
		for (auto vt:Act)
		{
			const int r = exRound[vt];
			// swing[vt] *= 1.0 * r / n_rounds;
			// variance[vt] = variance[vt] * (1.0 * (r-1) / (n_rounds-1)) + swing[vt] / (n_rounds -1) * swing[vt] * ( 1.0 * n_rounds/r - 1) * (n_rounds);
			// exRound[vt] = n_rounds;
			if (Q.size() < K + config.ext_k || Q.top().first < swing[vt])
			{
				if (Q.size() >= K + config.ext_k) Q.pop();
				Q.push({swing[vt],vt});
			}
		}
		if (config.PostRefineType == "VarBound")//VarBound
		{
			struct node
			{
				unsigned int id;
				double l,r;
			};
			int Bq_len = Q.size();
			vector<node>Bq(Bq_len);
			double beta_kmin = limit_r;
			double beta_kmax = 0;
			while (!Q.empty())
			{
				auto [sw, x] = Q.top();
				double lambda = EmpiricalFun(n_rounds, variance[x], limit_r, config.probf);
				if (Bq_len>K)
				{// k+1 k+ext_k
					beta_kmax = max(beta_kmax, sw + lambda);
				}else
				{// 1 k
					beta_kmin = min(beta_kmin, sw - lambda);
				}
				Bq[--Bq_len] = {x, sw-lambda, sw+lambda};
				Q.pop();
			}
			for (int i=0; i < K && i < Bq.size();i++)
			{
				if (Bq[i].l < beta_kmax)
				{
					Refine.push_back(Bq[i].id);
				}
			}
			for (int i=K ;i < Bq.size(); i++)
			{
				if (Bq[i].r > beta_kmin)
				{
					Refine.push_back(Bq[i].id);
				}
			}
		}else if (config.PostRefineType == "Naive")
		{
			while (!Q.empty() && Refine.size() < config.ext_k * 2)
			{
				auto [sw, x] = Q.top();
				Refine.push_back(x);
				Q.pop();
			}
		}
		else if (config.PostRefineType == "Threshold")
		{
			vector<pair<double, uint>> Bq(Q.size());
			int Bq_len = Q.size();
			while (!Q.empty())
			{
				Bq[--Bq_len] = Q.top();
				Q.pop();
			}
			if (Bq.size() > K)
			{
				for (int i = K-1; i >= 0 ; i--)
				{
					if (Bq[i].first - Bq[K].first < Bq[K].first * config.epsilon) Refine.push_back(Bq[i].second);
				}
			}
			for (int i=K; i < Bq.size(); i++)
			{
				if (Bq[K-1].first - Bq[i].first < Bq[i].first * config.epsilon) Refine.push_back(Bq[i].second);
			}
		}
	}

	/* Adaptive Pairwise Refinement*/
	{
		Timer tm_ref(6,"Refine");
		int reRounds = n_rounds / max((int)Refine.size(), 1);
		unordered_set<uint>Sq(U_star.begin(), U_star.end());
		for (auto vt:Refine)
		{
			double est_comUserNum2 = 0.5 * dvq * (dvq - 1) / n_rounds * hit[vt];
			if (graph.getVDeg(vt) + est_comUserNum2 < reRounds)
			{
				vector<uint> common_users;
				{
					Timer tm_ref(7,"ComUser");
					unordered_set<uint>St(graph.m_vedges[vt].begin(), graph.m_vedges[vt].end());
					if (Sq.size() < St.size())
					{
						for (auto u: Sq)
						{
							if (!St.count(u))continue;
							common_users.push_back(u);
						}
					}else
					{
						for (auto u: St)
						{
							if (!Sq.count(u))continue;
							common_users.push_back(u);
						}
					}
				}
				{
					swing[vt] = 0;
					for (int i = 0; i < common_users.size() ;i++)
					{
						for (int j=i+1; j < common_users.size();j++)
						{
							uint ui = common_users[i], uj = common_users[j];
							int Csize = fast_item_intersection(fast_set, graph, ui, uj);
							swing[vt] += 1.0 / (config.alpha + Csize);
						}
					}
				}
			}else if (limit_r / est_comUserNum2 * (config.alpha + 2) < Refine.size())
			{
				if (graph.getVDeg(vt) > 3 * Refine.size())
				{
					// no action
				}else
				{
					vector<uint> common_users;
					{
						Timer tm_ref(7,"ComUser");
						unordered_set<uint>St(graph.m_vedges[vt].begin(), graph.m_vedges[vt].end());
						if (Sq.size() < St.size())
						{
							for (auto u: Sq)
							{
								if (!St.count(u))continue;
								common_users.push_back(u);
							}
						}else
						{
							for (auto u: St)
							{
								if (!Sq.count(u))continue;
								common_users.push_back(u);
							}
						}
					}
					if (common_users.size() >1)
					{
						std::uniform_int_distribution<int> rd1(0, common_users.size()-1);
						std::uniform_int_distribution<int> rd2(0, common_users.size()-2);
						double delta_ij = common_users.size() * (common_users.size()-1) / 2.0 / reRounds ;
						for (int _=1; _<reRounds; _++)
						{
							uint i = rd1(RandomUtils::rng);
							uint j = rd2(RandomUtils::rng);
							uint ui = common_users[i], uj = common_users[j];
							int Csize = fast_item_intersection(fast_set, graph, ui, uj);
							swing[vt] += delta_ij / (config.alpha + Csize);
						}
					}
				}
			}

			// cout << "Refine: " << vt << ' '<< swing[vt] << ' ' << "Set size: " << result.size() << endl;
		}
		Accumulator::add(5, Refine.size(),"Refine size");
	}
}
void FastAdapSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng){
	// vector<uint> Sample_cnt;
	{
		Timer tm(4, "Preprocess");
		std::fill(swing.begin(), swing.end(), 0.0);
		// Sample_cnt.resize(swing.size());
	}

	if (graph.getVDeg(vq) < 2)
	{
		// cout << vq << " degree < 2, return" << endl;
		return ;
	}

	vector<uint>U_star;
	int dvq = 0;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			// U_star[u]=dvq++;
			dvq++;
			U_star.push_back(u);
		}
	}
	/* set random */
	if (dvq < 2) return ;
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	/* set parameter */
	double delta = 1.0 * graph.getNu() / graph.getM() * config.SampleRatio * std::pow(graph.getVDeg(vq), config.SamplePower) ;
	// double delta = 1.0 * graph.getNu() / graph.getM() * graph.getVDeg(vq);
	double probf = 1.0 / graph.getNv();
	double limit_r = 1.0 * dvq * (dvq -1) / (config.alpha + 2);
	long long n_rounds = 2.0 * limit_r * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon * delta) * log(1 / probf);
	double gamma0 = 1.0 / n_rounds * dvq * (dvq -1) / 2;
	long long full_w = 1ll * dvq * (dvq-1) / 2;
	Accumulator::add(0, full_w, "square cnt");
	Accumulator::add(1, n_rounds, "sample cnt");
	Accumulator::add(2, min(n_rounds, full_w), "Adaptive cnt");
	/* Test */
	// cout << "detal: " << delta << ", probf: " << probf << ", limit_r: " << limit_r << ", n_rounds: " << n_rounds << endl;
	// cout << "vq: " << vq << ", dvq: " << dvq << ", n_rounds: " << n_rounds << ", gamma0: " << gamma0 << endl;
	if (full_w <= n_rounds)
	{
		for (int i = 0; i < U_star.size(); i++)
		{
			for (int j=0; j<i; j++)
			{
				vector<uint> Cij;
				{
					Timer tm2(2, "intersection");
					fast_item_intersection(fast_set, graph, Cij, U_star[i], U_star[j]);
				}
				{
					Timer tm3(3, "addition");
					double gamma =  1.0 / (config.alpha + (double)Cij.size());
					for (auto vt: Cij) {
						swing[vt] += gamma;
					}
				}
			}
		}
	}else
	{
		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(rng);
			uint k2 = rand_u2(rng);
			if (k2>=k1) k2++;
			// uint ui = U_star[graph.m_vedges[vq][k1]];
			// uint uj = U_star[graph.m_vedges[vq][k2]];
			long cost = 0;
			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");
				fast_item_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
				cost = tm2.cost();
			}
			double gamma =  gamma0 / (config.alpha + (double)Cij.size());
			{
				Timer tm3(3, "addition");
				for (auto vt: Cij) {
					swing[vt] += gamma;
					// Sample_cnt[vt] += 1;
				}
			}
		}
		// {
		// 	Timer tm(5, "Last process");
		// 	for (int i=0;i < swing.size();i++)
		// 	{
		// 		if (swing[i]>0) swing[i] = swing[i] / n_rounds * Sample_cnt[i];
		// 	}
		// }
	}
	return ;
}
void TopKSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, const BPMaximalClique& fast_set, std::mt19937& rng, int K){
	// auto time_begin = std::chrono::steady_clock::now();
	std::fill(swing.begin(), swing.end(), 0.0);
	vector<double> variance(swing.size());
	vector<int> exRound(swing.size());
	vector<int> hit(swing.size());

	if (graph.getVDeg(vq) < 2)
	{
		// cout << vq << " degree < 2, return" << endl;
		return;
	}

	vector<uint>U_star;
	vector<unordered_set<uint>> Iu_sets;
	for (auto u: graph.m_vedges[vq]) {
		if (graph.m_udeg[u] >= 2){
			U_star.push_back(u);
			{
				Timer tm(4, "prepare Iu sets");
				unordered_set<uint> Iui(graph.m_uedges[u].begin(),graph.m_uedges[u].end());
				Iu_sets.push_back(Iui);
			}
		}
	}
	int dvq = U_star.size();
	// set random
	if (dvq < 2) return;
	std::uniform_int_distribution<int> rand_u1(0, dvq-1);
	std::uniform_int_distribution<int> rand_u2(0, dvq-2);
	// double delta = 1.0 * graph.getNu() / graph.getM() * graph.getVDeg(vq) * (graph.getVDeg(vq) -1) ;
	double delta = 1.0 * graph.getNu() / graph.getM() * config.SampleRatio * std::pow(graph.getVDeg(vq), config.SamplePower) ;
	double probf = 1.0 / graph.getNv();
	double limit_r = 1.0 * dvq * (dvq-1) / (config.alpha + 2);
	// long long n_rounds = 2.0 * (limit_r / delta) * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / probf);
	// long long n_rounds = dvq * (dvq-1) / 2.0;
	// double gamma0 = 1.0 / n_rounds * graph.getVDeg(vq) * (graph.getVDeg(vq) -1) / 2;
	// cout << "detal: " << delta << ", probf: " << probf << ", limit_r: " << limit_r << ", n_rounds: " << n_rounds << endl;
	// cout << "vq: " << vq << ", dvq: " << dvq << ", n_rounds: " << n_rounds << ", gamma0: " << gamma0 << endl;
	// for (int cas=1; cas <= T; cas*=2)
	Accumulator::add(0, dvq * (dvq-1) / 2.0, "Total cnt");

	unordered_set<uint> Act;
	int n_rounds = 2.0 * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon) * log(1 / probf);
	int last_rounds = 0;
	for (int cas=1; cas<=13; cas++)
	{
		// cout << "case:" << cas << "sample range: " << last_rounds << " " << n_rounds << endl;
		Accumulator::add(1, n_rounds - last_rounds, "Sample cnt");
		Accumulator::add(2,1, "Run Case");
		for (int r=last_rounds; r<n_rounds; r++) {
			uint k1 = rand_u1(rng);
			uint k2 = rand_u2(rng);
			if (k2>=k1) k2++;

			vector<uint> Cij;
			{
				Timer tm2(2, "intersection");
				// set_intersection(Iui.begin(), Iui.end(), Iuj.begin(), Iuj.end(), inserter(Cij, Cij.begin()));
				intersection(Iu_sets[k1], Iu_sets[k2], Cij);
				// fast_item_intersection(fast_set, graph, Cij, U_star[k1], U_star[k2]);
			}
			if (Cij.size() == 0) continue;
			double delta_ij =  dvq * (dvq-1) / 2.0 / (config.alpha + (double)Cij.size());
			{
				Timer tm3(3, "addition");
				for (auto vt: Cij) {
					if (vt == vq) continue;
					Act.insert(vt);
					if (hit[vt] == 0) exRound[vt] = last_rounds;
					hit[vt] += 1;
					exRound[vt] += 1;
					int ex_r = exRound[vt];
					double wt = delta_ij - swing[vt];
					swing[vt] += wt / ex_r;
					if (ex_r > 1)
					{
						variance[vt] = variance[vt] * (ex_r-2) / (ex_r-1) + wt / (ex_r-1) * (delta_ij - swing[vt]);
					}
				}
				// set<int> Sc(Cij.begin(), Cij.end());
				// for (auto vt :Act)
				// {
				// 	if (vt == vq || Sc.count(vt))continue;
				// 	double wt = 0 - swing[vt];
				// 	swing[vt] += wt / r;
				// 	variance[vt] = variance[vt] * (r-2) / (r-1) + wt / (r-1) * (delta_ij - swing[vt]);
				// 	last_round[vt] = r;
				// 	// H.push(vt, swing[vt] * r);
				// 	if(EmpiricalFun(r, variance[vt], limit_r, probf) >= probf)app_con++;
				// }
			}
		}
		/* Approximation Condition */
		// if (Act.size() <= K) return;
		// cout << Act.size() << endl;
		bool ReFlag = 0;
		priority_queue<pair<double, uint>, vector<pair<double, uint>>, greater<pair<double, uint>>>Q;
		double beta_del = 0;
		double beta_kmin = limit_r;
		double beta_kmax = 0;
		{
			Timer tm4(5, "Approximation");
			double Max_err = 0;
			for (auto vt:Act)
			{
				int r = exRound[vt];
				swing[vt] = swing[vt] * (1.0 * r / n_rounds);
				variance[vt] = variance[vt] * (1.0 * (r-1) / (n_rounds-1)) + swing[vt] / (n_rounds -1) * swing[vt] * ( 1.0 * n_rounds/r - 1) * (n_rounds);
				exRound[vt] = n_rounds;
				double lambda = EmpiricalFun(n_rounds, variance[vt], limit_r, config.probf);
				if (swing[vt] < lambda) Max_err = 10000;
				else Max_err = max(Max_err, 2 * lambda / (swing[vt] - lambda));
				Q.push({swing[vt],vt});
				if (Q.size() > K+K)
				{
					uint v = Q.top().second;
					beta_del = max(beta_del, swing[v] + EmpiricalFun(n_rounds, variance[v], limit_r, config.probf));
					Q.pop();
				}
			}
			if (Max_err < config.epsilon)
			{
				// cout << "Max_err: " << Max_err << endl;
				// for (auto vt:Act)
				// {
				// 	double lambda = EmpiricalFun(n_rounds, variance[vt], limit_r, config.probf);
				// 	cout << "vt: " << vt << " LB: " << swing[vt] - lambda<< " sw: " << swing[vt] << " RB: " << swing[vt] + lambda<< endl;
				// }
				Accumulator::add(4, 1, "approximation condition gain");
				return ;
			}
		}
		/* Separation Condition*/
		{
			Timer tm4(6, "condition");
			// auto [sw, vk] = H.topk(r);
			// auto [swk1,vk1] = H.topk1(r);
			// if (vk1 != -1)
			// {
			// 	double beta_k = swing[vk] - EmpiricalFun(r, variance[vk], H.top(r).first, 0.001);
			// 	double beta_k1 = swing[vk1] + EmpiricalFun(r, variance[vk1], H.topk(r).first, 0.001);
			// 	if (beta_k > beta_k1) break;
			// }
			vector<pair<uint, double>> Topk_Candidate;
			while (!Q.empty())
			{
				auto [sw,vt] = Q.top();
				Q.pop();
				double fv = EmpiricalFun(n_rounds, variance[vt], limit_r, config.probf);
				Topk_Candidate.push_back({vt, fv});
				if (Topk_Candidate.size() <= K)
				{// k+1 k+k
					beta_kmax = max(beta_kmax, sw + fv);
				}else
				{// 1 k
					beta_kmin = min(beta_kmin, sw - fv);
				}
				// cout<< vt << " sw: " << sw << " var: " << variance[vt] << " hit: " << hit[vt] << " fv: " << fv << " bound: " << sw - fv << ' ' << sw + fv << endl;
				// cout<< " sw: " << sw << " hitRatio: " << 1.0 * hit[vt]/n_rounds << " lim: " << limit_r / n_rounds * hit[vt] << ' ' << limit_r << endl;
			}
			if (beta_kmin < beta_del)
			{
				// the candidate is deleted ! Re-running
				// cout << "the candidate is deleted ! Re-running is need !" << endl;
				// Accumulator::add(5, 200, "Re-running");
				// continue;
				// assert(0);
				ReFlag = true;
			} else if (beta_kmin < beta_kmax)
			{
				Timer tm_ref(7,"Refine");
				vector<uint>Refine;
				for (int i = 0; i < K ;i++)
				{
					auto [v, fv] = Topk_Candidate[i];
					if (swing[v] + fv > beta_kmin)
					{
						Refine.push_back(v);
					}
				}
				for (int i = K; i < K+K; i++)
				{
					auto [v, fv] = Topk_Candidate[i];
					if (swing[v] - fv < beta_kmax)
					{
						Refine.push_back(v);
					}
				}
				if (Refine.size() > 2)
				{
					// cout << "Refine size is too large : "<< Refine.size() << endl;
					// Accumulator::add(6, 200, "Refine costly");
					// assert(0);
					ReFlag = true;
				}else
				{
					for (auto vt:Refine)
					{
						unordered_set<uint>St(graph.m_vedges[vt].begin(), graph.m_vedges[vt].end());
						vector<uint> result;
						result.clear();

						swing[vt] = 0;
						for (int i = 0; i < U_star.size() ;i++)
						{
							if (!St.count(U_star[i]))continue;
							vector<uint>Cij;
							for (auto j: result)
							{
								intersection(Iu_sets[i], Iu_sets[j], Cij);
								swing[vt] += 1.0 / (config.alpha + Cij.size());
							}
							result.push_back(i);
						}
						// cout << "Refine: " << vt << ' '<< swing[vt] << ' ' << "Set size: " << result.size() << endl;
					}
					Accumulator::add(5, Refine.size(),"Refine size");
					return ;
				}
			}else
			{
				Accumulator::add(6, 1, "Separation condition gain");
				return;
			}
		}
		last_rounds = n_rounds;
		n_rounds <<= 1;
	}
}
void TestSwing(int vq, std::vector<double>& swing, const Graph& graph, const Config& config, std::mt19937& rng){
	std::fill(swing.begin(), swing.end(), 0.0);
	vector<uint> U_star;
	vector<unordered_set<uint>> Iu_sets;
	{
		Timer tm(2, "prepare Iu sets");
		for (auto u: graph.m_vedges[vq]) {
			if (graph.m_udeg[u] >= 2){
				// U_star[u]=dvq++;
				U_star.push_back(u);
				{
					unordered_set<uint> Iui(graph.m_uedges[u].begin(),graph.m_uedges[u].end());
					Iu_sets.push_back(Iui);
				}
			}
		}
	}
	int dvq = U_star.size();
	if (dvq < 2) return;
	vector<uint> copy(U_star.size());
	for (uint i=0; i<copy.size(); i++){copy[i]=i;}
	std::shuffle(copy.begin(), copy.end(), rng);

	double delta = 1.0 * graph.getNu() / graph.getM() * config.SampleRatio * std::pow(graph.getVDeg(vq), config.SamplePower);
	// double delta = 1.0 * graph.getNu() / graph.getM() * graph.getVDeg(vq);
	double probf = 1.0 / graph.getNv();
	double limit_r = 1.0 * graph.getVDeg(vq) * (graph.getVDeg(vq) -1) / (config.alpha + 2);

	long long tot_rounds = 2.0 * limit_r * (config.epsilon / 3 + 1) / (config.epsilon * config.epsilon * delta) * log(1 / probf);
	// long long tot_rounds = 2 * config.maxUserPerItem * config.maxUserPerItem;
	tot_rounds = 2 * max(tot_rounds, 2ll * config.maxUserPerItem * config.maxUserPerItem);
	long long n_rounds = tot_rounds / 2;
	// int sample_num = min((int)sqrt(n_rounds) , (int)U_star.size());
	int sample_num = 0;
	// {
	// 	Timer tm(3,"Exact part");
	// 	for (int i=0; i<sample_num; i++){
	// 		for (int j=0; j<i; j++) {
	// 			unordered_set<uint> &Iui = Iu_sets[copy[i]];
	// 			unordered_set<uint> &Iuj = Iu_sets[copy[j]];
	// 			vector<uint> Cij;
	// 			{
	// 				Timer tm2(5, "Exact intersection");
	// 				// set_intersection(Iui.begin(), Iui.end(), Iuj.begin(), Iuj.end(), inserter(Cij, Cij.begin()));
	// 				intersection(Iui, Iuj, Cij);
	// 			}
	// 			// statistic the counts
	// 			// Accumulator::add(4, (Cij.size()>=5), "5 valid pairs");
	// 			// Accumulator::add(5, (Cij.size()>=10), "10 valid pairs");
	// 			// Accumulator::add(6, (Cij.size()>=100), "100 valid pairs");
	//
	// 			double gamma = 1.0 / (config.alpha + (double)Cij.size());
	// 			{
	// 				for (auto vt: Cij) {
	// 					swing[vt] += gamma;
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	{
		if (sample_num >= dvq) return;
		Timer tm(4, "MC part");

		// double gamma0 = 1.0 / n_rounds * (dvq - sample_num) * (dvq -1) / 2;
		// std::uniform_int_distribution<int> rand_u1(sample_num, dvq - 1);
		// std::uniform_int_distribution<int> rand_u2(0, dvq-2);
		double gamma0 = 1.0 / n_rounds * (dvq - sample_num) * (dvq) / 2;
		std::uniform_int_distribution<int> rand_u1(sample_num, dvq-1);
		std::uniform_int_distribution<int> rand_u2(0, dvq-1);

		for (int c=1; c<=n_rounds; c++) {
			uint k1 = rand_u1(rng);
			uint k2 = rand_u2(rng);
			// uint ui = U_star[graph.m_vedges[vq][k1]];
			// uint uj = U_star[graph.m_vedges[vq][k2]];
			if (k1 == k2) continue;
			// if (k2 == k1){k2 = dvq-1;}
			unordered_set<uint> &Iui = Iu_sets[copy[k1]];
			unordered_set<uint> &Iuj = Iu_sets[copy[k2]];
			vector<uint> Cij;
			{
				Timer tm2(6, "MC intersection");
				// set_intersection(Iui.begin(), Iui.end(), Iuj.begin(), Iuj.end(), inserter(Cij, Cij.begin()));
				intersection(Iui, Iuj, Cij);
			}

			double gamma =  gamma0 / (config.alpha + (double)Cij.size());
			{
				for (auto vt: Cij) {
					swing[vt] += gamma;
				}
			}
		}
	}

}
