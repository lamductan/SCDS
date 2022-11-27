#include <algorithm>
#include <cassert>
#include <iostream>

#include "centralized/algorithms/GK96_cds/GK96_CDS.h"
#include "centralized/algorithms/checkers/cds_checker.h"
#include "centralized/algorithms/checkers/mis_checker.h"
#include "utils/union_find.h"
#include "utils/utils.h"

centralized::GK96CDSAlg::GK96CDSAlg() {}

centralized::GK96CDSAlg::GK96CDSAlg(std::map<int, centralized::Node *> nodes,
                                    std::set<centralized::Edge> edges)
{
    this->graph = new Graph(nodes, edges, CDS_GK96_NODE_TYPE);
}

std::set<int> centralized::GK96CDSAlg::find_cds_nodes_from_mis_nodes(
    const std::vector<int> &mis_nodes)
{
    std::cout << "centralized::GK96CDSAlg::find_cds_nodes_from_mis_nodes()\n";
    std::set<int> mis_nodes_set(mis_nodes.begin(), mis_nodes.end());
    std::set<int> cds_nodes_set(mis_nodes.begin(), mis_nodes.end());

    std::vector<Edge> cluster_edges = find_cluster_edges(mis_nodes_set);
    std::vector<Edge> mst_edges = build_mst(cluster_edges);

    for (Edge edge : mst_edges) {
        cds_nodes_set = combine_set<int>(cds_nodes_set, edge.nodes_on_edge);
    }
    return cds_nodes_set;
}

std::vector<centralized::Edge>
centralized::GK96CDSAlg::find_cluster_edges(std::set<int> &mis_nodes_set)
{
    std::cout << "centralized::GK96CDSAlg::find_cluster_edges()\n";
    std::map<std::pair<int, int>, Edge> cluster_edges_map;
    for (Edge edge : graph->edges) {
        int u = edge.u;
        int v = edge.v;
        int cluster_u = graph->nodes[u]->dominator;
        int cluster_v = graph->nodes[v]->dominator;
        if (cluster_u == cluster_v)
            continue;
        if (cluster_u > cluster_v) {
            std::swap(u, v);
            std::swap(cluster_u, cluster_v);
        }
        Edge cluster_edge(cluster_u, cluster_v, 2);

        if (u == cluster_u) {
            --cluster_edge.w;
        } else {
            cluster_edge.nodes_on_edge.push_back(u);
        }

        if (v == cluster_v) {
            --cluster_edge.w;
        } else {
            cluster_edge.nodes_on_edge.push_back(v);
        }

        // std::cout << '\t' << "u = " << u << ", v = " << v << ", cluster_edge
        // = " << cluster_edge.to_string() << '\n';
        if (cluster_edge.w == 0)
            continue;

        std::pair<int, int> p = { cluster_u, cluster_v };
        if (cluster_edges_map.find(p) == cluster_edges_map.end()) {
            cluster_edges_map[p] = cluster_edge;
        } else {
            if (cluster_edge.w == 2)
                continue;
            if (cluster_edges_map[p].w == 1)
                continue;
            else
                cluster_edges_map[p] = cluster_edge;
        }
    }
    for (auto it : cluster_edges_map) {
        std::pair<int, int> p = it.first;
        Edge e = it.second;
        // std::cout << "\tcluster_edges_map[" << p.first << "," << p.second <<
        // "] = " << e.to_string() << '\n';
    }

    return getMapValues<std::pair<int, int>, Edge>(cluster_edges_map);
}

std::vector<centralized::Edge>
centralized::GK96CDSAlg::build_mst(std::vector<centralized::Edge> &edges)
{
    int n = graph->nodes.size();
    UnionFind union_find(n);
    std::sort(edges.begin(), edges.end(),
              [](Edge a, Edge b) { return (a.w < b.w); });

    std::vector<Edge> mst_edges;

    for (Edge edge : edges) {
        if (mst_edges.size() < n - 1) {
            int fr = edge.u;
            int to = edge.v;
            if (union_find.find(fr) == union_find.find(to))
                continue;
            union_find.join(fr, to);
            mst_edges.push_back(edge);
        } else {
            break;
        }
    }
    return mst_edges;
}