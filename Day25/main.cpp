#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<queue>
#include<map>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>

using namespace std;

class SetUnion
{
private:
    unordered_map<string, int> mapping;
    int freeIdx;
    int* parent;
    int* rank;
    int size;
    int sets;

public:
    SetUnion(int size)
    {
        this->size = size+1;
        
        parent = new int[this->size];
        rank = new int[this->size];

        MakeSet();
    }

    ~SetUnion()
    {
        delete[] parent;
        delete[] rank;
    }

    void MakeSet()
    {
        freeIdx = 0;
        sets = 0;
        mapping.clear();

        for(int i = 0; i < this->size; i++)
        {
            parent[i] = 0;
            rank[i] = 0;
        }
    }

    int GetSets()
    {
        return sets;
    }

    void AddSet(const string& label)
    {
        if(mapping.find(label) == mapping.end())
        {
            freeIdx++;
            mapping[label] = freeIdx;
        }

        AddSet(mapping[label]);
    }

    void AddSet(int i)
    {
        if (parent[i] == 0)
        {
            parent[i] = i;
            sets++;
        }
    }

    int FindSet(const string& label)
    {
        if(mapping.find(label) == mapping.end())
        {
            freeIdx++;
            mapping[label] = freeIdx;
        }

        return FindSet(mapping[label]);
    }

    int FindSet(int i)
    {
        if (i == parent[i])
        {
            return i;
        }

        return parent[i] = FindSet(parent[i]);
    }

    void UnionSets(const string& label1, const string& label2)
    {
        if(mapping.find(label1) == mapping.end())
        {
            freeIdx++;
            mapping[label1] = freeIdx;
        }

        if(mapping.find(label2) == mapping.end())
        {
            freeIdx++;
            mapping[label2] = freeIdx;
        }

        UnionSets(mapping[label1], mapping[label2]);
    }

    void UnionSets(int i, int j)
    {
        if (parent[i] == 0 || parent[j] == 0)
        {
            return;
        }

        i = FindSet(i);
        j = FindSet(j);

        if (i != j)
        {
            if (rank[i] < rank[j])
            {
                swap(i,j);
            }

            parent[j] = i;
            sets--;

            if (rank[i] == rank[j])
            {
                rank[i]++;
            }
        }
    }
};

class Graph
{
private:
    unordered_set<string> nodes;
    unordered_multimap<string, string> edges;

    string startNode;
    multimap<int, string> nodeByDistance;

public:
    void ParseLine(char* line)
    {
        string from = ReadLabel(line);

        char* pointer = strchr(line, ':');
        char* token = strtok(pointer+2, " ");

        while (token != NULL)
        {
            string to = ReadLabel(token);
            AddEdge(from, to);
            token = strtok(NULL, " ");
        }
    }

    int FindPartition()
    {
        int limit = 3;
        string start = *nodes.begin();
        CalculateDistanceFromStart(start);

        unordered_multimap<string, string> usedEdges;
        for(auto it = nodeByDistance.rbegin(); it != nodeByDistance.rend(); it++)
        {
            string end = it->second;
            int flow = MaximalFlow(start, end, usedEdges, limit);
            if(flow > limit) { continue; }

            SetUnion setUnion(nodes.size());
            for(auto& node : nodes)
            {
                setUnion.AddSet(node);
            }
            for(auto& [from, to] : edges)
            {
                auto edge = make_pair(from, to);
                auto reverse = make_pair(to, from);
                if(ContainsEdge(usedEdges, edge) || ContainsEdge(usedEdges, reverse)) { continue; }
                else { setUnion.UnionSets(from, to); }
            }

            unordered_map<int,int> partitions;
            for(auto& node : nodes)
            {
                int id = setUnion.FindSet(node);
                if(partitions.find(id) == partitions.end()) { partitions[id] = 1; }
                else { partitions[id]++; }
            }

            int startId = setUnion.FindSet(start);
            int endId = setUnion.FindSet(end);

            if(partitions.size() == 2)
            {
                return partitions[startId] * partitions[endId];
            }
        }

        return 0;
    }

private:
    string ReadLabel(char* pointer)
    {
        string label;
        while(isalpha(*pointer))
        {
            label.push_back(*pointer);
            ++pointer;
        }
        return label;
    }

    void AddNode(string& name)
    {
        if(nodes.find(name) == nodes.end())
        {
            nodes.insert(name);
        }
    }

    void AddEdge(string& from, string& to)
    {
        AddNode(from);
        AddNode(to);

        edges.insert(make_pair(from, to));
        edges.insert(make_pair(to, from));
    }

    int CountPartition(unordered_set<string> part)
    {
        int count = 0;
        for(auto& [from, to] : edges)
        {
            if (part.find(from) != part.end() && part.find(to) == part.end()) { count++; }
        }
        return count;
    }

    void CalculateDistanceFromStart(string& start)
    {
        startNode = start;
        nodeByDistance.clear();

        queue<string> toProcess;
        unordered_map<string, int> distanceFromStart;

        nodeByDistance.insert(make_pair(0, startNode));
        distanceFromStart[startNode] = 0;
        toProcess.push(startNode);

        while(!toProcess.empty())
        {
            string node = toProcess.front();
            toProcess.pop();

            int distance = distanceFromStart[node] + 1;

            const auto range = edges.equal_range(node);
            for(auto it = range.first; it != range.second; it++)
            {
                string next = it->second;
                if(distanceFromStart.find(next) == distanceFromStart.end())
                {
                    nodeByDistance.insert(make_pair(distance, next));
                    distanceFromStart[next] = distance;
                    toProcess.push(next);
                }
            }
        }
    }

    string GetFarthestNode()
    {
        return nodeByDistance.rbegin()->second;
    }

    int MaximalFlow(string& start, string& end, unordered_multimap<string, string>& usedEdges, int limit)
    {
        vector<pair<string, string>> path;

        int flow = 0;
        usedEdges.clear();

        while(FindPath(start, end, usedEdges, path) && flow <= limit)
        {
            for(auto& edge: path)
            {
                auto reverse = make_pair(edge.second, edge.first);
                auto used = FindEdge(usedEdges, reverse);
                if(used != usedEdges.end())
                {
                    usedEdges.erase(used);
                }
                else
                {
                    usedEdges.insert(edge);
                }
            }

            flow++;
        }

        return flow;
    }

    bool FindPath(string& start, string& end, unordered_multimap<string, string>& usedEdges, vector<pair<string, string>>& path)
    {
        path.clear();
        unordered_map<string, string> predecessors;
        unordered_set<string> visited;
        queue<string> toProcess;

        toProcess.push(start);
        visited.insert(start);

        while(!toProcess.empty())
        {
            string node = toProcess.front();
            toProcess.pop();

            const auto range = edges.equal_range(node);
            for(auto it = range.first; it != range.second; it++)
            {
                string next = it->second;
                auto edge = make_pair(node, next);
  
                if(visited.find(next) == visited.end() && !ContainsEdge(usedEdges, edge))
                {
                    visited.insert(next);
                    predecessors[next] = node;
                    
                    if(next.compare(end) == 0)
                    {
                        while(!toProcess.empty()) { toProcess.pop(); }
                        break;
                    }
                    else
                    {
                        toProcess.push(next);
                    }
                }
            }
        }

        if(predecessors.find(end) == predecessors.end())
        {
            return false;
        }

        string node = end;
        string prev;
        do
        {
            prev = predecessors[node];
            path.push_back(make_pair(prev, node));
            node = prev;
        } while (node.compare(start) != 0);
        
        return true;
    }

    bool ContainsEdge(unordered_multimap<string, string>& usedEdges, pair<string, string>& edge)
    {
        const auto range = usedEdges.equal_range(edge.first);
        for(auto it = range.first; it != range.second; it++)
        {
            if(it->second.compare(edge.second) == 0) { return true; }
        }
        return false;
    }

    unordered_multimap<string, string>::iterator FindEdge(unordered_multimap<string, string>& usedEdges, pair<string, string>& edge)
    {
        const auto range = usedEdges.equal_range(edge.first);
        for(auto it = range.first; it != range.second; it++)
        {
            if(it->second.compare(edge.second) == 0) { return it; }
        }

        return usedEdges.end();
    }
}; 

int main()
{
    int result = 0;
    char buffer[1024];
    Graph graph;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) <= 1) { continue; }
        graph.ParseLine(buffer);
    }

    result = graph.FindPartition();

    printf("%i\n", result);

    return 0;
}