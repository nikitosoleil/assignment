#include <iostream>
#include <vector>
#include <queue>
#include <bitset>

using namespace std;

namespace graphs
{
	struct edge
	{
		int to, capacity, reverse, flow, cost;
		edge (int a = 0, int b = 0, int c = 0, int d = 0): to(a), capacity(b), reverse(c), flow(0), cost(d) {}
	};
	
	typedef vector < vector < edge > > graph;
}

using namespace graphs;

namespace flows
{
	const int INF = 1e9;
	
	class min_cost_max_flow_increasing_paths
	{
	public:
		graph g;
		int source, sink = 0;
		pair < int, int > ans = {0, 0};
		min_cost_max_flow_increasing_paths (const graph &g, int source, int sink): g(g), source(source), sink(sink) {}
		pair < int, int > evaluate ()
		{
			if (ans == make_pair(0, 0))
			{
				int tmp;
				do
				{
					vector < int > dist(g.size(), INF), mn(g.size(), 0), p(g.size(), 0);
					dist[source] = 0;
					mn[source] = INF;
					bool changed;
					do
					{
						changed = false;
						for (int v = 1; v < g.size(); ++v)
							for (auto &it: g[v])
								if (it.flow < it.capacity && dist[v]+it.cost < dist[it.to])
								{
									dist[it.to] = dist[v]+it.cost;
									mn[it.to] = min(mn[v], it.capacity-it.flow);
									p[it.to] = it.reverse;
									changed = true;
								}
					} while (changed);
					tmp = mn[sink];
					if (tmp != 0)
					{
						int v = sink;
						while (v != source)
						{
							g[v][p[v]].flow -= tmp;
							auto edge = g[v][p[v]];
							g[edge.to][edge.reverse].flow += tmp;
							v = edge.to;
						}
						ans.first += tmp;
						ans.second += dist[sink]*tmp;
					}
				} while (tmp != 0);
			}
			return ans;
		}
	};
	
	class min_cost_max_flow_cycles_removal
	{
	public:
		static const int N = 1e3;
		graph g;
		int source, sink = 0;
		pair < int, int > ans = {0, 0};
		min_cost_max_flow_cycles_removal (const graph &g, int source, int sink): g(g), source(source), sink(sink) {}
		pair < int, int > evaluate ()
		{
			if (ans.first == 0 && ans.second == 0)
			{
				int tmp;
				do
				{
					vector < int > dist(g.size(), INF), mn(g.size(), 0), p(g.size(), 0);
					dist[source] = 0;
					mn[source] = INF;
					queue < int > que;
					que.push(source);
					while (!que.empty())
					{
						int v = que.front();
						que.pop();
						for (auto &it: g[v])
							if (mn[it.to] == 0 && it.flow < it.capacity)
							{
								dist[it.to] = dist[v]+it.cost;
								mn[it.to] = min(mn[v], it.capacity-it.flow);
								p[it.to] = it.reverse;
								que.push(it.to);
							}
					}
					tmp = mn[sink];
					if (tmp != 0)
					{
						int v = sink;
						while (v != source)
						{
							g[v][p[v]].flow -= tmp;
							auto edge = g[v][p[v]];
							g[edge.to][edge.reverse].flow += tmp;
							v = edge.to;
						}
					}
					ans.first += tmp;
					ans.second += dist[sink]*tmp;
				} while (tmp != 0);
				bool changed = false;
				do
				{
					int last;
					vector < int > dist(g.size(), INF), mn(g.size(), 0), p(g.size(), 0);
					for (int i = 1; i < g.size(); ++i)
					{
						dist[source] = 0;
						mn[source] = INF;
						changed = false;
						for (int v = 1; v < g.size(); ++v)
							for (auto &it: g[v])
								if (it.flow < it.capacity && dist[v]+it.cost < dist[it.to])
								{
									dist[it.to] = dist[v]+it.cost;
									mn[it.to] = min(mn[v], it.capacity-it.flow);
									p[it.to] = it.reverse;
									changed = true;
									last = v;
								}
					}
					if (changed)
					{
						int flow = INF;
						int cost = 0;
						int v = last;
						bitset < N > vis;
						do
						{
							auto edge = g[v][p[v]];
							flow = min(flow, g[edge.to][edge.reverse].capacity-g[edge.to][edge.reverse].flow);
							vis[v] = true;
							v = edge.to;
						} while (!vis[v]);
						last = v;
						do
						{
							g[v][p[v]].flow -= flow;
							auto edge = g[v][p[v]];
							cost += g[edge.to][edge.reverse].cost;
							g[edge.to][edge.reverse].flow += flow;
							v = edge.to;
						} while (v != last);
						ans.second += cost*flow;
					}
				} while (changed);
			}
			return ans;
		}
	};
};

using namespace flows;

int main ()
{
	int n;
	cin >> n;
	graph g;
	g.resize(2*n+3);
	for (int i = 1; i <= n; ++i)
	{
		for (int j = 1; j <= n; ++j)
		{
			int cost;
			cin >> cost;
			g[i+1].push_back(edge(n+j+1, 1, g[n+j+1].size(), cost));
			g[n+j+1].push_back(edge(i+1, 0, g[i+1].size()-1, -cost));
		}
	}
	for (int i = 1; i <= n; ++i)
	{
		g[1].push_back(edge(i+1, 1, g[i+1].size(), 0));
		g[i+1].push_back(edge(1, 0, g[1].size()-1, 0));
		g[n+i+1].push_back(edge(2*n+2, 1, g[2*n+2].size(), 0));
		g[2*n+2].push_back(edge(n+i+1, 0, g[n+i+1].size()-1, 0));
	}
	min_cost_max_flow_increasing_paths mcmf(g, 1, 2*n+2);
	cout << mcmf.evaluate().second << endl;
	for (int i = 2; i <= n+1; ++i)
		for (int j = 0; j < n; ++j)
			if (mcmf.g[i][j].flow != 0)
				cout << j+1 << ' ';
	return 0;
}