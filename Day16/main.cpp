#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<vector>
#include<unordered_set>
#include<stack>

using namespace std;

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
        return point.x << 7 | point.y;
   }
};

enum GridDevice
{
    Empty = '.',
    TopLeftMirror = '\\',
    TopRightMirror = '/',
    HorizontalSplitter = '-',
    VerticalSplitter = '|'
};

struct Beam
{
    Point Position;
    DirectionType Direction;

    Beam(Point position, DirectionType direction) : Position(position), Direction(direction) { }

    Beam (Beam& beam, DirectionType nextDirection) : Position(beam.Position, nextDirection), Direction(nextDirection) { }
};

bool operator < (const Beam& b1, const Beam& b2)
{
    return (b1.Position < b2.Position)
        || (b1.Position.x == b2.Position.x && b1.Position.y == b2.Position.y && b1.Direction < b2.Direction);
}

bool operator == (const Beam& b1, const Beam& b2)
{
    return b1.Position == b2.Position && b1.Direction == b2.Direction;
}

struct BeamHash {
   size_t operator()(const Beam &beam) const
   {
        return beam.Direction << 14 | beam.Position.x << 7 | beam.Position.y;
   }
};

class Contraption
{
private:
    vector<vector<GridDevice>> grid;
    

public:
    Contraption() {}

    bool AddLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<GridDevice> row;
        for(int i = 0; i < length; i++)
        {
            switch(line[i])
            {
                case '.': row.push_back(Empty); break;
                case '\\': row.push_back(TopLeftMirror); break;
                case '/': row.push_back(TopRightMirror); break;
                case '-': row.push_back(HorizontalSplitter); break;
                case '|': row.push_back(VerticalSplitter); break;
                default: break;
            }
        }

        grid.push_back(row);

        return true;
    }

    int CountEnergized(Beam startBeam)
    {
        unordered_set<Point, PointHash> energized;
        unordered_set<Beam, BeamHash> analyzed;

        stack<Beam> queue;
        queue.push(startBeam);

        while(!queue.empty())
        {
            Beam beam = queue.top();
            queue.pop();

            if(analyzed.find(beam) != analyzed.end()) { continue; }

            energized.insert(beam.Position);
            analyzed.insert(beam);
            pair<DirectionType, DirectionType> next = ProcessBeam(beam);

            if(next.first != None)
            {
                Beam nextBeam(beam, next.first);
                if(IsOnGrid(nextBeam.Position) && analyzed.find(nextBeam) == analyzed.end())
                {
                    queue.push(nextBeam);
                }
            }

            if(next.second != None)
            {
                Beam nextBeam(beam, next.second);
                if(IsOnGrid(nextBeam.Position) && analyzed.find(nextBeam) == analyzed.end())
                {
                    queue.push(nextBeam);
                }
            }
        }
        
        return (int)energized.size();
    }

    int GetRows() { return (int)grid.size(); }
    int GetColumns() { return (int)grid[0].size(); }

private:
    bool IsOnGrid(Point& position)
    {
        return position.y >= 0 && position.y < (int)grid.size()
            && position.x >= 0 && position.x < (int)grid[position.y].size();
    }

    GridDevice GetDevice(Point& position)
    {
        return grid[position.y][position.x];
    }

    pair<DirectionType, DirectionType> ProcessBeam(Beam& beam)
    {
        GridDevice device = GetDevice(beam.Position);

        switch(device)
        {
            case Empty:
                return make_pair(beam.Direction, None);
            case TopLeftMirror:
                return make_pair(ProcessTopLeftMirror(beam.Direction), None);
            case TopRightMirror:
                return make_pair(ProcessTopRightMirror(beam.Direction), None);
            case HorizontalSplitter:
                return ProcessHorizontalSplitter(beam.Direction);
            case VerticalSplitter:
                return ProcessVerticalSplitter(beam.Direction);
            default:
                return make_pair(None, None);
        }
    }

    DirectionType ProcessTopLeftMirror(DirectionType direction)
    {
        switch (direction)
        {
        case Up: return Left;
        case Down: return Right;
        case Left: return Up;
        case Right: return Down;
        default: return None;
        }
    }

    DirectionType ProcessTopRightMirror(DirectionType direction)
    {
        switch (direction)
        {
        case Up: return Right;
        case Down: return Left;
        case Left: return Down;
        case Right: return Up;
        default: return None;
        }
    }

    pair<DirectionType, DirectionType> ProcessHorizontalSplitter(DirectionType direction)
    {
        switch (direction)
        {
        case Up:
        case Down:
            return make_pair(Left, Right);
        case Left:
        case Right:
            return make_pair(direction, None);
        default:
            return make_pair(None, None);
        }
    }

    pair<DirectionType, DirectionType> ProcessVerticalSplitter(DirectionType direction)
    {
        switch (direction)
        {
        case Up:
        case Down:
            return make_pair(direction, None);
        case Left:
        case Right:
            return make_pair(Up, Down);
        default:
            return make_pair(None, None);
        }
    }
};

int main()
{
    int result = 0LL;
    char buffer[1024];
    Contraption contraption;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        contraption.AddLine(buffer);
    }

    printf("%i\n", contraption.CountEnergized(Beam(Point(0,0), Right)));

    int rows = contraption.GetRows();
    int columns = contraption.GetColumns();

    for(int i = 0; i < rows; i++)
    {
        int value = contraption.CountEnergized(Beam(Point(0,i), Right));
        result = max(result, value);

        value = contraption.CountEnergized(Beam(Point(columns-1,i), Left));
        result = max(result, value);
    }

    for(int i = 0; i < columns; i++)
    {
        int value = contraption.CountEnergized(Beam(Point(i,0), Down));
        result = max(result, value);

        value = contraption.CountEnergized(Beam(Point(i, rows-1), Up));
        result = max(result, value);
    }

    printf("%i\n", result);

    return 0;
}