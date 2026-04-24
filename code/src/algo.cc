#include "algo.h"
#include <math.h>
#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace std;

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
		else
		{
			assert(0);
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
