#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<queue>
#include<set>

using namespace std;

enum DirectionType
{
    Up = 1,
    Down = 3,
    Left = 2,
    Right = 4,
    None = 0,
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
};

bool operator < (const Point& p1, const Point& p2)
{
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}

bool operator == (const Point& p1, const Point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

struct State
{
    Point Position;
    int HeatLoss;
    DirectionType PreviousMove;
    int Multiplicity;

    State(Point position, int HeatLoss, DirectionType previousMove, int multiplicity) :
        Position(position), HeatLoss(HeatLoss), PreviousMove(previousMove), Multiplicity(multiplicity) { }

    State(State& state, int heatLoss, DirectionType direction)
    {
        Position = Point(state.Position, direction);
        HeatLoss = state.HeatLoss + heatLoss;
        PreviousMove = direction;
        Multiplicity = (state.PreviousMove == direction)
            ? state.Multiplicity + 1
            : 1;
    }

    State(State& state, int heatLoss)
        : Position(state.Position), HeatLoss(heatLoss), PreviousMove(state.PreviousMove), Multiplicity(state.Multiplicity) { }
};

bool operator < (const State& s1, const State& s2)
{
    return (s1.HeatLoss > s2.HeatLoss)
        || (s1.HeatLoss == s2.HeatLoss && s1.Multiplicity > s2.Multiplicity)
        || (s1.HeatLoss == s2.HeatLoss && s1.Multiplicity == s2.Multiplicity && (int)s1.PreviousMove > (int)s2.PreviousMove)
        || (s1.HeatLoss == s2.HeatLoss && s1.Multiplicity == s2.Multiplicity && (int)s1.PreviousMove == (int)s2.PreviousMove && s1.Position < s2.Position);
}

bool CanMove1(State& state, DirectionType direction)
{
    if(state.PreviousMove == None) { return true; }
    if(direction == None) { return false; }
    if(state.PreviousMove == direction) { return state.Multiplicity < 3; }

    return ((int)state.PreviousMove & 1) != ((int)direction & 1);
}

bool CanMove2(State& state, DirectionType direction)
{
    if(state.PreviousMove == None) { return true; }
    if(direction == None) { return false; }
    if(state.PreviousMove == direction) { return state.Multiplicity < 10; }

    if(state.Multiplicity < 4) return false;
    return ((int)state.PreviousMove & 1) != ((int)direction & 1);
}

class HeatLossMap
{
private:
    vector<vector<int>> heatLossMap;

public:
    HeatLossMap() {}

    bool AddLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<int> row;
        for(int i = 0; i < length; i++)
        {
            if(isdigit(line[i]))
            {
                row.push_back(line[i] - '0');
            }
        }

        heatLossMap.push_back(row);

        return true;
    }

    int MinimumHeatLoss(Point start, Point end, bool (*CanMove)(State&, DirectionType))
    {
        priority_queue<State> queue;
        set<State> analyzed;

        queue.push(State(start, 0, None, 0));

        while(!queue.empty())
        {
            State state = queue.top();
            queue.pop();

            if(state.Position == end) { return state.HeatLoss; }

            if(analyzed.find(State(state,0)) != analyzed.end()) { continue; }
            analyzed.insert(State(state,0));

            if(CanMove(state, Up))
            {
                Point nextPosition(state.Position, Up);
                if(IsOnMap(nextPosition))
                {
                    State next = State(state, GetHeatLoss(nextPosition), Up);
                    queue.push(next);
                }
            }

            if(CanMove(state,Down))
            {
                Point nextPosition(state.Position, Down);
                if(IsOnMap(nextPosition))
                {
                    State next = State(state, GetHeatLoss(nextPosition), Down);
                    queue.push(next);
                }
            }

            if(CanMove(state, Left))
            {
                Point nextPosition(state.Position, Left);
                if(IsOnMap(nextPosition))
                {
                    State next = State(state, GetHeatLoss(nextPosition), Left);
                    queue.push(next);
                }
            }

            if(CanMove(state, Right))
            {
                Point nextPosition(state.Position, Right);
                if(IsOnMap(nextPosition))
                {
                    State next = State(state, GetHeatLoss(nextPosition), Right);
                    queue.push(next);
                }
            }
        }

        return 0;
    }

    int GetRows() { return (int)heatLossMap.size(); }
    int GetColumns() { return (int)heatLossMap[0].size(); }
private:

    bool IsOnMap(Point& position)
    {
        return position.y >= 0 && position.y < (int)heatLossMap.size()
            && position.x >= 0 && position.x < (int)heatLossMap[position.y].size();
    }

    int GetHeatLoss(Point& position)
    {
        return heatLossMap[position.y][position.x];
    }
};

int main()
{
    int result = 0;
    char buffer[1024];
    HeatLossMap map;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        map.AddLine(buffer);
    }

    result = map.MinimumHeatLoss(Point(0,0), Point(map.GetColumns()-1, map.GetRows()-1), CanMove1);
    printf("%i\n", result);

    result = map.MinimumHeatLoss(Point(0,0), Point(map.GetColumns()-1, map.GetRows()-1), CanMove2);
    printf("%i\n", result);

    return 0;
}