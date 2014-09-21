/**
 * @file algorithms.tcc
 * @author Sean Massung
 */

#include <random>
#include <unordered_set>

namespace meta
{
namespace graph
{
namespace algorithms
{
template <class UndirectedGraph>
double clustering_coefficient(const UndirectedGraph& graph, node_id id)
{
    auto adj = graph.adjacent(id);
    if (adj.empty())
        return 0.0;

    if (adj.size() == 1)
        return 1.0;

    double numerator = 0.0;
    for (size_t i = 0; i < adj.size(); ++i)
    {
        for (size_t j = i + 1; j < adj.size(); ++j)
        {
            // count if two neighbors are connected
            if (graph.edge(adj[i].first, adj[j].first))
                ++numerator;
        }
    }

    return (2.0 * numerator) / (adj.size() * (adj.size() - 1));
}

template <class UndirectedGraph>
double clustering_coefficient(const UndirectedGraph& graph)
{
    double total = 0.0;
    for(auto& n: graph)
        total += clustering_coefficient(graph, n.id);

    return total / graph.size();
}

template <class UndirectedGraph>
double neighborhood_overlap(const UndirectedGraph& graph, node_id src,
                            node_id dest)
{
    if (!graph.edge(src, dest))
        throw graph_algorithm_exception{
            "neighborhood_overlap must be called on neighboring nodes"};

    double num_shared = 0.0;
    std::unordered_set<node_id> total;
    for (auto& p : graph.adjacent(src))
    {
        total.insert(p.first);
        if (graph.edge(dest, p.first))
            ++num_shared;
    }

    if (num_shared == 0.0)
        return 0.0;

    for (auto& p : graph.adjacent(dest))
        total.insert(p.first);

    // minus 2 so src doesn't count dest and vice versa
    return num_shared / (total.size() - 2);
}

template <class UndirectedGraph>
void random_graph(UndirectedGraph& g, uint64_t num_nodes, uint64_t num_edges)
{
    uint64_t start_id = g.size();
    for (uint64_t i = start_id; i < start_id + num_nodes; ++i)
        g.emplace(std::to_string(i));

    uint64_t possible = g.size() * (g.size() - 1) - g.num_edges();
    if (num_edges > possible)
        throw undirected_graph_exception{
            "impossible to add required number of edges to graph"};

    std::default_random_engine gen;
    std::uniform_int_distribution<uint64_t> dist(0, g.size() - 1);
    uint64_t edges_added = 0;
    while (edges_added != num_edges)
    {
        node_id src{dist(gen)};
        node_id dest{dist(gen)};

        if (src == dest || g.edge(src, dest))
            continue;

        g.add_edge(src, dest);
        ++edges_added;
    }
}

template <class UndirectedGraph>
void watts_strogatz(UndirectedGraph& g, uint64_t num_nodes,
                    uint64_t num_neighbors, uint64_t num_random_edges)
{
    if (g.size() != 0)
        throw undirected_graph_exception{
            "watts-strogatz graph generation must be called on an empty graph"};

    if (num_neighbors % 2 != 0)
        throw undirected_graph_exception{
            "num_neighbors for watts-strogatz graph model must be even"};

    for (uint64_t i = 0; i < num_nodes; ++i)
        g.emplace(std::to_string(i));

    for (uint64_t i = 0; i < num_nodes; ++i)
        for(uint64_t j = 1; j <= num_neighbors / 2; ++j)
            g.add_edge(node_id{i}, node_id{(i + j) % g.size()});

    random_graph(g, 0, num_random_edges);
}
}
}
}
