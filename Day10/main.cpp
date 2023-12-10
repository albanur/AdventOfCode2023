#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>

using namespace std;

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }
};

enum Direction
{
    DirectionN = 1,
    DirectionS = 2,
    DirectionW = 4,
    DirectionE = 8
};

enum Pipe
{
    PipeNS = DirectionN + DirectionS,
    PipeWE = DirectionW + DirectionE,
    PipeNW = DirectionN + DirectionW,
    PipeNE = DirectionN + DirectionE,
    PipeSW = DirectionS + DirectionW,
    PipeSE = DirectionS + DirectionE,
    Start = 16,
    MainLoop = 32,
    Empty = 0
};

Point GoTo(const Point& point, Direction direction)
{
    switch(direction)
    {
        case DirectionN: return Point(point.x, point.y - 1);
        case DirectionS: return Point(point.x, point.y + 1);
        case DirectionW: return Point(point.x - 1, point.y);
        case DirectionE: return Point(point.x + 1, point.y);
        default: return Point(point.x, point.y);
    }
}

bool IsOnBoard(const vector<vector<Pipe>>& board, const Point& point)
{
    return point.y >= 0 && point.y < (int)board.size()
        && point.x >= 0 && point.x < (int)board[point.y].size();
}

Pipe& GetTile(vector<vector<Pipe>>& board, const Point& point)
{
    return board[point.y][point.x];
}

bool HasDirection(Pipe pipe, Direction direction)
{
    return (pipe & direction) == direction;
}

Pipe DecodePipe(char c)
{
    switch(c)
    {
        case '|': return PipeNS;
        case '-': return PipeWE;
        case 'L': return PipeNE;
        case 'F': return PipeSE;
        case 'J': return PipeNW;
        case '7': return PipeSW;
        case 'S': return Start;
        case '.': return Empty;
        default: return Empty;
    }
}

void FindTypeOfStart(vector<vector<Pipe>>& board, const Point& startPoint)
{
    Pipe& startTile = GetTile(board, startPoint);

    Point point;
    
    point = GoTo(startPoint, DirectionN);
    if(IsOnBoard(board, point) && HasDirection(GetTile(board, point), DirectionS))
    {
        startTile = (Pipe)(startTile | DirectionN);
    }

    point = GoTo(startPoint, DirectionS);
    if(IsOnBoard(board, point) && HasDirection(GetTile(board, point), DirectionN))
    {
        startTile = (Pipe)(startTile | DirectionS);
    }

    point = GoTo(startPoint, DirectionW);
    if(IsOnBoard(board, point) && HasDirection(GetTile(board, point), DirectionE))
    {
        startTile = (Pipe)(startTile | DirectionW);
    }

    point = GoTo(startPoint, DirectionE);
    if(IsOnBoard(board, point) && HasDirection(GetTile(board, point), DirectionW))
    {
        startTile = (Pipe)(startTile | DirectionE);
    }
}

void MarkAsMainLoop(vector<vector<Pipe>>& board, const Point& point)
{
    Pipe& tile = GetTile(board, point);
    tile = (Pipe)(tile | MainLoop);
}

Direction NextDirection(vector<vector<Pipe>>& board, const Point& point, Direction lastDirection)
{
    Pipe tile = GetTile(board, point);

    if(lastDirection != DirectionS && HasDirection(tile, DirectionN)) return DirectionN;
    if(lastDirection != DirectionN && HasDirection(tile, DirectionS)) return DirectionS;
    if(lastDirection != DirectionW && HasDirection(tile, DirectionE)) return DirectionE;
    if(lastDirection != DirectionE && HasDirection(tile, DirectionW)) return DirectionW;

    return (Direction)0;
}

bool IsPipeOfType(vector<vector<Pipe>>& board, const Point& point, Pipe type)
{
    return (GetTile(board, point) & type) == type;
}

int CrossesNW(Pipe tile)
{
    if((tile & PipeSW) == PipeSW) return 2;
    if((tile & PipeNE) == PipeNE) return 2;
    if((tile & PipeNS) == PipeNS) return 1;
    if((tile & PipeWE) == PipeWE) return 1;
    if((tile & PipeSE) == PipeSE) return 1;
    if((tile & PipeNW) == PipeNW) return 1;
    return 0;
}

bool IsEnclosed(vector<vector<Pipe>>& board, const Point& point)
{
    if(IsPipeOfType(board, point, MainLoop))
    {
        return false;
    }

    int crosses = 0;
    Point current = Point(point.x-1, point.y-1);
    while(IsOnBoard(board, current))
    {
        if(IsPipeOfType(board, current, MainLoop))
        {
            crosses += CrossesNW(GetTile(board, current));
        }
        
        current.x--;
        current.y--;
    }

    return ((crosses & 1) == 1);
}

int main()
{
    char buffer[1024];
    vector<vector<Pipe>> board;
    Point startPoint;
    vector<Point> path;
    int enclosed = 0;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        vector<Pipe> line;
        int length = strlen(buffer);
        Pipe p;

        for(int i = 0; i < length; i++)
        {
            p = DecodePipe(buffer[i]);
            line.push_back(p);
            if(p == Start)
            {
                startPoint = Point(i, (int)board.size());
            }
        }

        board.push_back(line);
    }

    FindTypeOfStart(board, startPoint);

    Point current = startPoint;
    Direction lastDirection = DirectionN;
    Direction nextDirection;

    do
    {
        MarkAsMainLoop(board, current);
        path.push_back(current);
    
        nextDirection = NextDirection(board, current, lastDirection);
        current = GoTo(current, nextDirection);
        lastDirection = nextDirection;

    } while (current.x != startPoint.x || current.y != startPoint.y);
    
    printf("%i\n", (int)path.size()/2);

    for(int y = 0; y < (int)board.size(); y++)
    {
        for(int x = 0; x < (int)board[y].size(); x++)
        {
            if(IsEnclosed(board, Point(x, y)))
            {
                enclosed++;
            }
        }
    }

    printf("%i\n", enclosed);

    return 0;
}