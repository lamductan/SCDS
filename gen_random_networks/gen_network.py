# Usage:
# python gen_network.py

import os
import sys
import argparse
import random
import math
import tqdm

sys.setrecursionlimit(5000)

parser = argparse.ArgumentParser()
parser.add_argument('--t', type=int, help='number of networks', default=50)
parser.add_argument('--w', type=int, help='area width', default=1000)
parser.add_argument('--h', type=int, help='area height', default=1000)
parser.add_argument('--seed', type=int, help='seed', default=1234)
args = parser.parse_args()
random.seed(args.seed)

class DSU:
    def __init__(self, n):
        self.n = n
        self.par = list(range(n))
        self.rk = [0]*n

    def find(self, x):
        if x == self.par[x]: return x
        return self.find(self.par[x])
    
    def join(self, x, y):
        x = self.find(x)
        y = self.find(y)
        if x == y: return
        if self.rk[x] < self.rk[y]:
            x, y = y, x
        self.par[y] = x
        if self.rk[x] == self.rk[y]:
            self.rk[x] += 1

def distance(node1, node2):
    return math.sqrt((node1.x - node2.x)**2 + (node1.y - node2.y)**2)

class Node:
    def __init__(self, id, x, y):
        self.id = id
        self.x = x
        self.y = y
        self.edges = []

class Edge:
    def __init__(self, endpoint1, endpoint2):
        self.endpoint1 = endpoint1
        self.endpoint2 = endpoint2
        self.length = distance(self.endpoint1, self.endpoint2)

    def get_other_endpoint_id(self, u_id):
        return self.endpoint1.id + self.endpoint2.id - u_id

class Instance:
    def __init__(self, n, w, h, denseness):
        self.n = n
        self.w = w
        self.h = h
        self.denseness = denseness

        while True:
            self.nodes = self.gen_nodes()
            self.complete_edges = self.get_complete_edges()
            self.r, self.edges = self.get_selected_edges()
            self.m = len(self.edges)
            if self.is_connected(): break
            else:
                print("Network is not connected. Generate another one!")
                exit(1)

    def gen_nodes(self):
        nodes = []
        for i in range(self.n):
            x = random.randint(0, self.w)
            y = random.randint(0, self.h)
            node = Node(i, x, y)
            nodes.append(node)
        return nodes

    def get_complete_edges(self):
        complete_edges = []
        for i in range(self.n):
            for j in range(i):
                complete_edges.append(Edge(self.nodes[j], self.nodes[i]))
        return complete_edges

    def build_mst(self):
        dsu = DSU(self.n)
        cnt = 0
        for i in range(len(self.complete_edges)):
            edge = self.complete_edges[i]
            x = edge.endpoint1.id
            y = edge.endpoint2.id
            x = dsu.find(x)
            y = dsu.find(y)
            if x == y: continue
            dsu.join(x,y)
            cnt += 1
            if cnt == self.n - 1: return i
        return len(self.complete_edges) - 1

    def get_selected_edges(self):
        self.complete_edges = sorted(self.complete_edges, key=lambda e: e.length)
        longest_mst_edge_idx = self.build_mst()
        longest_edge_idx = -1
        a = max(self.n*4 - 1, longest_mst_edge_idx)
        if self.denseness == 'sparse':
            longest_edge_idx = a
        elif self.denseness == 'medium':
            longest_edge_idx = random.randint(a, self.n**2//4 - 1)
        else:
            longest_edge_idx = random.randint(self.n**2//4, self.n*(self.n - 1)/2 - 1)
        r = self.complete_edges[longest_edge_idx].length

        edges = []
        for i in range(longest_edge_idx + 1):
            edge = self.complete_edges[i]
            edges.append(edge)
            endpoint1 = edge.endpoint1
            endpoint2 = edge.endpoint2
            endpoint1.edges.append(edge)
            endpoint2.edges.append(edge)
        return r, edges
        
    def is_connected(self):
        def dfs(u, par, vis):
            if vis[u]: return
            vis[u] = True
            for edge in self.nodes[u].edges:
                v = edge.get_other_endpoint_id(u)
                if v == par: continue
                if vis[v]: continue
                dfs(v, u, vis)

        vis = [False]*self.n
        dfs(0, -1, vis)
        return all(vis)

    def save(self, file_path):
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, "w") as f:
            f.write('{} {} {} {}\n'.format(self.n, self.m, self.r, self.denseness))
            for node in self.nodes:
                f.write('{} {} {}\n'.format(node.id, node.x, node.y))
            for edge in self.edges:
                f.write('{} {}\n'.format(edge.endpoint1.id, edge.endpoint2.id))
            f.close()

def gen_network_and_save_file(network_id, n, w, h, denseness):
    file_path = os.path.join(str(n), denseness, '{:03d}.txt'.format(network_id))
    instance = Instance(n, w, h, denseness)
    instance.save(file_path)
    del instance

def gen(n, denseness):
    t = args.t
    w = args.w
    h = args.h

    for network_id in range(t):
        gen_network_and_save_file(network_id, n, w, h, denseness)

if __name__ == '__main__':
    for n in tqdm.tqdm([20, 50, 100] + list(range(200, 2001, 100))):
        print("n =", n)
        for denseness in ['sparse', 'medium', 'dense']:
            print(denseness)
            gen(n, denseness)
