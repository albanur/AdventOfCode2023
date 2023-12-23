#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<unordered_set>
#include<algorithm>

using namespace std;

enum TileType
{
    Path = '.',
    Forest = '#',
    UpSlope = '^',
    DownSlope = 'v',
    LeftSlope = '<',
    RightSlope = '>',
    UnknownType,
};

enum DirectionType
{
    Up,
    Down,
    Left,
    Right,
    None,
};

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

    Point(Point& point, DirectionType direction)
    {
        switch(direction)
        {
            case Up:
                x = point.x; y = point.y-1; break;
            case Down:
                x = point.x; y = point.y+1; break;
            case Left:
                x = point.x-1; y = point.y; break;
            case Right:
                x = point.x+1; y = point.y; break;
            default:
                x = point.x; y = point.y; break;
        }
    }

    size_t Hash() const
    {
        return ((size_t)x << 32) ^ (size_t)y;
    }
};

bool operator < (const Point& p1, const Point& p2)
{
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}

bool operator == (const Point& p1, const Point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

struct PointHash {
   size_t operator()(const Point &point) const
   {
        return point.Hash();
   }
};

struct Node;

struct Edge
{
    Node* NextNode;
    int Cost;

    Edge(Node* next, int cost) : NextNode(next), Cost(cost) { }
};

struct Node
{
    Point Position;
    vector<Edge> Edges;

    Node(Point position) : Position(position) { }
};

class HikingMap
{
private:
    vector<vector<TileType>> map;
    vector<Node> nodes;
    Node* start;
    Node* finish;

public:
    HikingMap(){}

    bool ParseLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<TileType> row;
        for(int i = 0; i < length; i++)
        {
            switch(line[i])
            {
                case '.':
                case '#':
                case '^':
                case 'v':
                case '<':
                case '>':
                    row.push_back((TileType)line[i]);
                    break;
                default:
                    break;
            }
        }

        map.push_back(row);

        return true;
    }

    void BuildDirectionalGraph()
    {
        ClearGraph();
        FindNodes();
        FindDirectionalEdges();
    }

    void BuildUndirectionalGraph()
    {
        ClearGraph();
        FindNodes();
        FindUndirectionalEdges();
    }

    int FindLongestPath()
    {
        int additionalCost = 0;
        Node* begin = start;
        Node* end = finish;
        unordered_set<Point, PointHash> visited;

        if(begin->Edges.size() == 1)
        {
            additionalCost += begin->Edges[0].Cost;
            begin = begin->Edges[0].NextNode;
        }

        if(end->Edges.size() == 1)
        {
            additionalCost += end->Edges[0].Cost;
            end = end->Edges[0].NextNode;
        }

        return additionalCost + FindLongestPath(begin, end, visited, 0);
    }

private:
    void ClearGraph()
    {
        start = NULL;
        finish = NULL;
        nodes.clear();
    }

    void FindNodes()
    {
        nodes.push_back(Node(GetStartPosition()));

        Point position;
        for(int y = 1; y < (int)map.size()-1; y++)
        {
            position.y = y;
            for(int x = 1; x < (int)map[y].size()-1; x++)
            {
                position.x = x;
                if(GetTile(position) != Forest && CountPossibleDirections(position) > 2)
                {
                    nodes.push_back(Node(position));
                }
            }
        }

        nodes.push_back(Node(GetFinishPosition()));
        
        start = &nodes.front();
        finish = &nodes.back();
    }

    int CountPossibleDirections(Point& position)
    {
        int result = 0;
        vector<DirectionType> directions = { Up, Down, Left, Right };

        Point next;
        for(DirectionType direction : directions)
        {
            next = Point(position, direction);
            if(IsOnMap(next) && GetTile(next) != Forest)
            {
                result++;
            }
        }

        return result;
    }

    Point GetStartPosition()
    {
        for(int x = 0; x < (int)map[0].size(); x++)
        {
            if(map[0][x] == Path) { return Point(x,0); }
        }

        return Point(1,0);
    }

    Point GetFinishPosition()
    {
        int lastRowId = (int)map.size() - 1;
        for(int x = 0; x < (int)map[lastRowId].size(); x++)
        {
            if(map[lastRowId][x] == Path) { return Point(x,lastRowId); }
        }

        return Point((int)map[lastRowId].size()-2, lastRowId);
    }

    void FindDirectionalEdges()
    {
        unordered_set<Point, PointHash> visited;
        vector<DirectionType> nodeMoves;
        vector<DirectionType> pathMoves;
        Point next;

        for(Node& node : nodes)
        {
            visited.clear();
            FindDirectionalPossibleMoves(node.Position, visited, nodeMoves);

            for(DirectionType nodeMove : nodeMoves)
            {
                next = node.Position;
                visited.clear();
                pathMoves = { nodeMove };
                
                do
                {
                    visited.insert(next);
                    next = Point(next, pathMoves[0]);
                    FindDirectionalPossibleMoves(next, visited, pathMoves);
                } while (pathMoves.size() == 1);

                Node* nextNode = FindNodeByPosition(next);

                if(nextNode != NULL)
                {
                    node.Edges.push_back(Edge(nextNode, (int)visited.size()));
                }
            }
        }
    }

    void FindDirectionalPossibleMoves(Point& position, unordered_set<Point, PointHash>& visited, vector<DirectionType>& moves)
    {
        moves.clear();
        TileType tile = GetTile(position);
        vector<DirectionType> legalMoves;

        switch(tile)
        {
            case Path:
                legalMoves.push_back(Up);
                legalMoves.push_back(Down);
                legalMoves.push_back(Left);
                legalMoves.push_back(Right);
                break;
            case UpSlope:
                legalMoves.push_back(Up); break;
            case DownSlope:
                legalMoves.push_back(Down); break;
            case LeftSlope:
                legalMoves.push_back(Left); break;
            case RightSlope:
                legalMoves.push_back(Right); break;
            default:
                break;
        }

        Point next;
        for(DirectionType move : legalMoves)
        {
            next = Point(position, move);
            if(IsOnMap(next) && GetTile(next) != Forest && visited.find(next) == visited.end())
            {
                moves.push_back(move);
            }
        }
    }

    void FindUndirectionalEdges()
    {
        unordered_set<Point, PointHash> visited;
        vector<DirectionType> nodeMoves;
        vector<DirectionType> pathMoves;
        Point next;

        for(Node& node : nodes)
        {
            visited.clear();
            FindUnidirectionalPossibleMoves(node.Position, visited, nodeMoves);

            for(DirectionType nodeMove : nodeMoves)
            {
                next = node.Position;
                visited.clear();
                pathMoves = { nodeMove };
                
                do
                {
                    visited.insert(next);
                    next = Point(next, pathMoves[0]);
                    FindUnidirectionalPossibleMoves(next, visited, pathMoves);
                } while (pathMoves.size() == 1);

                Node* nextNode = FindNodeByPosition(next);

                if(nextNode != NULL)
                {
                    node.Edges.push_back(Edge(nextNode, (int)visited.size()));
                }
            }
        }
    }

    void FindUnidirectionalPossibleMoves(Point& position, unordered_set<Point, PointHash>& visited, vector<DirectionType>& moves)
    {
        moves.clear();
        vector<DirectionType> legalMoves = { Up, Down , Left, Right };

        Point next;
        for(DirectionType move : legalMoves)
        {
            next = Point(position, move);
            if(IsOnMap(next) && GetTile(next) != Forest && visited.find(next) == visited.end())
            {
                moves.push_back(move);
            }
        }
    }

    Node* FindNodeByPosition (Point& position)
    {
        for(Node& node : nodes)
        {
            if(node.Position == position)
            {
                return &node;
            }
        }

        return NULL;
    }
   
    int FindLongestPath(Node* current, Node* end, unordered_set<Point, PointHash>& visited, int cost)
    {
        int result = 0;

        if(current->Position == end->Position) { return cost; }

        visited.insert(current->Position);
        for(Edge& e : current->Edges)
        {
            if(visited.find(e.NextNode->Position) == visited.end())
            {
                int value = FindLongestPath(e.NextNode, end, visited, cost + e.Cost);
                result = max(result, value);
            }
        }
        visited.erase(current->Position);

        return result;
    }

    bool IsOnMap(const Point& point)
    {
        return point.y >= 0 && point.y < (int)map.size()
            && point.x >= 0 && point.x < (int)map[point.y].size();
    }

    TileType GetTile(const Point& point)
    {
        return map[point.y][point.x];
    }
};

int main()
{
    int64_t result = 0;
    char buffer[1024];
    HikingMap map;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) <= 1) { break; }
        map.ParseLine(buffer);
    }

    map.BuildDirectionalGraph();
    result = map.FindLongestPath();
    printf("%lld\n", result);

    map.BuildUndirectionalGraph();
    result = map.FindLongestPath();
    printf("%lld\n", result);

    return 0;
}