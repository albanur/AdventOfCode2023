#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<unordered_map>
#include<queue>

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

enum TileType
{
    Plot = '.',
    Rock = '#',
    Start = 'S'
};

struct Step
{
    Point Position;
    int Steps;

    Step(Point& point, int steps) : Position(point), Steps(steps) { }
};

bool operator < (const Step& s1, const Step& s2)
{
    return (s1.Steps > s2.Steps) || (s1.Steps == s2.Steps && s1.Position < s2.Position);
}

class Garden
{
private:
    int width;
    int height;

    vector<vector<TileType>> garden;
    vector<vector<int>> distance;
    unordered_map<int,int> summary;

public:
    Garden(vector<vector<TileType>>& tiles)
    {
        garden = tiles;
        width = (int)garden[0].size();
        height = (int)garden.size();
    }

    void CalculateDistance(vector<Step>& entries)
    {
        distance.clear();
        summary.clear();
        distance.resize(height, vector<int>(width, -1));

        priority_queue<Step> processQueue;
        for(auto& entry : entries)
        {
            processQueue.push(entry);
        }

        while(!processQueue.empty())
        {
            Step step = processQueue.top();
            processQueue.pop();

            if(GetDistance(step.Position) >= 0) { continue; }

            SetDistance(step.Position, step.Steps);
            UpdateSummary(step.Steps);

            ConsiderMove(step, Up, processQueue);
            ConsiderMove(step, Down, processQueue);
            ConsiderMove(step, Left, processQueue);
            ConsiderMove(step, Right, processQueue);
        }
    }

    int64_t CountReachableTiles(int steps)
    {
        int64_t counter = 0LL;

        for(const auto& [key, value] : summary)
        {
            if((key&1) == (steps&1) && key <= steps)
            {
                counter += value;
            }
        }

        return counter;
    }

    int GetDistance(const Point& point)
    {
        return distance[point.y][point.x];
    }

    int GetWidth() { return width; }

    int GetHeight() { return height; }

private:
    bool IsInGarden(const Point& point)
    {
        return point.y >= 0 && point.y < (int)garden.size()
            && point.x >= 0 && point.x < (int)garden[point.y].size();
    }

    TileType GetTile(const Point& point)
    {
        return garden[point.y][point.x];
    }

    void SetDistance(const Point& point, int value)
    {
        distance[point.y][point.x] = value;
    }

    void UpdateSummary(int value)
    {
        if(summary.find(value) == summary.end())
        {
            summary[value] = 1;
        }
        else
        {
            summary[value]++;
        }
    }

    void ConsiderMove(Step& step, DirectionType direction, priority_queue<Step>& processQueue)
    {
        Point next(step.Position, direction);
        if(IsInGarden(next) && GetTile(next) == Plot)
        {
            processQueue.push(Step(next, step.Steps+1));
        }
    }
};

class GardenParser
{
private:
    vector<vector<TileType>> garden;
    Point start;

public:
    bool ParseLine(const char* line)
    {
        int length = strlen(line);
        if(length <= 1) { return false; }

        vector<TileType> row;
        for(int i = 0; i < length; i++)
        {
            if(line[i] == '.') { row.push_back(Plot); }
            else if(line[i] == '#') { row.push_back(Rock); }
            else if(line[i] == 'S')
            {
                row.push_back(Plot);
                start = Point(i, garden.size());
            }
            else { break; }
        }

        garden.push_back(row);

        return true;
    }

    Garden* CreateGarden()
    {
        return new Garden(garden);
    }

    Point& GetStartPoint() { return start; }
};

struct BigStep
{
    vector<Step> Entries;
};

class BigGarden
{
private:
    Garden* garden;

public:
    BigGarden(Garden* garden) : garden(garden) {}

    int64_t CountReachableTiles(Point& start, int steps)
    {
        int64_t counter = 0LL;

        BigStep center;
        center.Entries.push_back(Step(start, 0));

        garden->CalculateDistance(center.Entries);
        counter += garden->CountReachableTiles(steps);

        BigStep upStep, downStep, leftStep, rightStep;
        BigStep upLeftStep, upRightStep, downLeftStep, downRightStep;

        GetBigUpStep(upStep);
        GetBigDownStep(downStep);
        GetBigLeftStep(leftStep);
        GetBigRightStep(rightStep);

        GetBigUpLeftStep(upLeftStep);
        GetBigUpRightStep(upRightStep);
        GetBigDownLeftStep(downLeftStep);
        GetBigDownRightStep(downRightStep);

        counter += CountInAxisDirection(&BigGarden::GetBigUpStep, upStep, steps, garden->GetHeight());
        counter += CountInAxisDirection(&BigGarden::GetBigDownStep, downStep, steps, garden->GetHeight());
        counter += CountInAxisDirection(&BigGarden::GetBigLeftStep, leftStep, steps, garden->GetWidth());
        counter += CountInAxisDirection(&BigGarden::GetBigRightStep, rightStep, steps, garden->GetWidth());

        counter += CountInDiagonalDirection(&BigGarden::GetBigLeftStep, upLeftStep, steps, garden->GetWidth());
        counter += CountInDiagonalDirection(&BigGarden::GetBigRightStep, upRightStep, steps, garden->GetWidth());
        counter += CountInDiagonalDirection(&BigGarden::GetBigLeftStep, downLeftStep, steps, garden->GetWidth());
        counter += CountInDiagonalDirection(&BigGarden::GetBigRightStep, downRightStep, steps, garden->GetWidth());

        return counter;
    }

private:
    // precise version (longer, more general)
    /* int64_t CountInAxisDirection(void (BigGarden::*GetNextBigStep)(BigStep& step), BigStep next, int steps, int diff)
    {
        int64_t counter = 0LL;
        int64_t lastCounter = 0LL;

        do
        {
            garden->CalculateDistance(next.Entries);
            lastCounter = garden->CountReachableTiles(steps);
            counter += lastCounter;
            (*this.*GetNextBigStep)(next);
        } while(lastCounter > 0LL);

        return counter;
    } */

    // fast version, uses special properties of real input
    int64_t CountInAxisDirection(void (BigGarden::*GetNextBigStep)(BigStep& step), BigStep next, int steps, int diff)
    {
        int64_t counter = 0LL;
        int64_t lastCounter = 0LL;

        garden->CalculateDistance(next.Entries);
        do
        {
            lastCounter = garden->CountReachableTiles(steps);
            counter += lastCounter;
            steps -= diff;
        } while(lastCounter > 0LL);

        return counter;
    }

    // precise version (longer, more general)
    /* int64_t CountInDiagonalDirection(void (BigGarden::*GetNextBigStep)(BigStep& step), BigStep next, int steps, int diff)
    {
        int64_t counter = 0LL;
        int64_t multiplier = 1LL;
        int64_t lastCounter = 0LL;

        do
        {
            garden->CalculateDistance(next.Entries);
            lastCounter = garden->CountReachableTiles(steps);
            counter += multiplier * lastCounter;
            multiplier++;
            (*this.*GetNextBigStep)(next);
        } while(lastCounter > 0LL);

        return counter;
    } */

    // fast version, uses special properties of real input
    int64_t CountInDiagonalDirection(void (BigGarden::*GetNextBigStep)(BigStep& step), BigStep next, int steps, int diff)
    {
        int64_t counter = 0LL;
        int64_t multiplier = 1LL;
        int64_t lastCounter = 0LL;

        garden->CalculateDistance(next.Entries);
        do
        {
            lastCounter = garden->CountReachableTiles(steps);
            counter += multiplier * lastCounter;
            multiplier++;
            steps -= diff;
        } while(lastCounter > 0LL);

        return counter;
    }

    void GetBigUpStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(0, 0);
        Point destination(0, garden->GetHeight()-1);

        for(int x=0; x < garden->GetWidth(); x++)
        {
            source.x = destination.x = x;
            int dist = garden->GetDistance(source);
            if(dist >= 0)
            {
                step.Entries.push_back(Step(destination, dist+1));
            }
        }
    }

    void GetBigDownStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(0, garden->GetHeight()-1);
        Point destination(0, 0);

        for(int x=0; x < garden->GetWidth(); x++)
        {
            source.x = destination.x = x;
            int dist = garden->GetDistance(source);
            if(dist >= 0)
            {
                step.Entries.push_back(Step(destination, dist+1));
            }
        }
    }

    void GetBigLeftStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(0, 0);
        Point destination(garden->GetWidth()-1, 0);

        for(int y=0; y < garden->GetHeight(); y++)
        {
            source.y = destination.y = y;
            int dist = garden->GetDistance(source);
            if(dist >= 0)
            {
                step.Entries.push_back(Step(destination, dist+1));
            }
        }
    }

    void GetBigRightStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(garden->GetWidth()-1, 0);
        Point destination(0, 0);

        for(int y=0; y < garden->GetHeight(); y++)
        {
            source.y = destination.y = y;
            int dist = garden->GetDistance(source);
            if(dist >= 0)
            {
                step.Entries.push_back(Step(destination, dist+1));
            }
        }
    }

    void GetBigUpLeftStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(0, 0);
        Point destination(garden->GetWidth()-1, garden->GetHeight()-1);

        int dist = garden->GetDistance(source);
        if(dist >= 0)
        {
            step.Entries.push_back(Step(destination, dist+2));
        }
    }

    void GetBigUpRightStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(garden->GetWidth()-1, 0);
        Point destination(0, garden->GetHeight()-1);

        int dist = garden->GetDistance(source);
        if(dist >= 0)
        {
            step.Entries.push_back(Step(destination, dist+2));
        }
    }

    void GetBigDownLeftStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(0, garden->GetHeight()-1);
        Point destination(garden->GetWidth()-1, 0);

        int dist = garden->GetDistance(source);
        if(dist >= 0)
        {
            step.Entries.push_back(Step(destination, dist+2));
        }
    }

    void GetBigDownRightStep(BigStep& step)
    {
        step.Entries.clear();

        Point source(garden->GetWidth()-1, garden->GetHeight()-1);
        Point destination(0, 0);

        int dist = garden->GetDistance(source);
        if(dist >= 0)
        {
            step.Entries.push_back(Step(destination, dist+2));
        }
    }
};

int main()
{
    char buffer[1024];
    GardenParser parser;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        parser.ParseLine(buffer);
    }

    Garden* garden = parser.CreateGarden();
    vector<Step> entries = { Step(parser.GetStartPoint(), 0) };
    garden->CalculateDistance(entries);

    int64_t result = garden->CountReachableTiles(64);
    printf("%lld\n", result);

    BigGarden bigGarden(garden);
    result = bigGarden.CountReachableTiles(parser.GetStartPoint(), 26501365);
    printf("%lld\n", result);

    delete garden;

    return 0;
}