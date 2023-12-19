#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<algorithm>

using namespace std;

enum DirectionType
{
    Up,
    Down,
    Left,
    Right,
    None
};

struct DigStep
{
    DirectionType Direction;
    int Step;
    int Color;

    // DigStep(const char* line)
    // {
    //     switch(line[0])
    //     {
    //         case 'U': Direction = Up; break;
    //         case 'D': Direction = Down; break;
    //         case 'L': Direction = Left; break;
    //         case 'R': Direction = Right; break;
    //         default: Direction = None; break;
    //     }

    //     Step = atoi(line+2);
    //     Color = strtol(strchr(line, '#') + 1, NULL, 16);
    // }

    DigStep(const char* line)
    {
        char* code = strchr(line, '#') + 1;

        switch(code[5])
        {
            case '3': Direction = Up; break;
            case '1': Direction = Down; break;
            case '2': Direction = Left; break;
            case '0': Direction = Right; break;
            default: Direction = None; break;
        }

        code[5] = ' ';
        Step = strtol(code, NULL, 16);
        Color = 0;
    }
};

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }

    Point Move(DirectionType direction, int steps)
    {
        switch(direction)
        {
            case Up:
                return Point(x, y-steps);
            case Down:
                return Point(x, y+steps);
            case Left:
                return Point(x-steps, y);
            case Right:
                return Point(x+steps, y);
            default:
                return Point(x, y);
        }
    }
};

struct VerticalRange
{
    int x;
    int y1;
    int y2;

    VerticalRange(int x, int y1, int y2) : x(x), y1(y1), y2(y2) { }

    VerticalRange(Point& p, int dy)
    {
        x = p.x;

        if(dy >= 0) { y1 = p.y; y2 = p.y + dy; }
        else { y1 = p.y + dy; y2 = p.y; }
    }

    VerticalRange(Point& p1, Point& p2)
    {
        if(p1.x != p2.x) { x = y1 = y2 = 0; return; }

        x = p1.x;
        y1 = min(p1.y, p2.y);
        y2 = max(p1.y, p2.y);
    }
};

struct CompareByX
{
    bool operator() (const VerticalRange& r1, const VerticalRange& r2) const
    {
        return (r1.x < r2.x)
            || (r1.x == r2.x && r1.y1 < r2.y1)
            || (r1.x == r2.x && r1.y1 == r2.y1 && r1.y2 < r2.y2);
    }
};

struct ReverseCompareByY2
{
    bool operator() (const VerticalRange& r1, const VerticalRange& r2) const
    {
        return (r1.y2 > r2.y2)
            || (r1.y2 == r2.y2 && r1.x > r2.x)
            || (r1.y2 == r2.y2 && r1.x == r2.x && r1.y1 > r2.y1);
    }
};

enum VerticalRangeCross
{
    NoneCross = 0,
    Start = 1,
    End = 2,
    Middle = 3
};

class BigPlan
{
private:
    vector<DigStep> steps;

public:
    BigPlan() {}

    void AddStep(DigStep step) { steps.push_back(step); }

    int64_t CalculateArea()
    {
        multimap<int, VerticalRange> ranges;
        Point point(0,0);

        for(auto step : steps)
        {
            Point end = point.Move(step.Direction, step.Step);

            if(step.Direction == Left || step.Direction == Right) {point = end; continue; }

            VerticalRange range(point, end);
            ranges.insert(make_pair(range.y1, range));

            point = end;
        }

        vector<VerticalRange> active;
        priority_queue<VerticalRange, vector<VerticalRange>, ReverseCompareByY2> activeToRemove;
        
        auto rangeIt = ranges.begin();

        int64_t count = 0LL;
        int currentY = INT32_MIN;

        while(rangeIt != ranges.end() || !activeToRemove.empty())
        {
            int nextToInsertY = (rangeIt != ranges.end()) ? rangeIt->first : INT32_MAX;
            int nextToRemoveY = !activeToRemove.empty() ? activeToRemove.top().y2 : INT32_MAX;
            int nextY = min(nextToInsertY, nextToRemoveY);

            count += (int64_t)(nextY - currentY - 1) * CountRow(nextY - 1, active);

            while(rangeIt != ranges.end() && rangeIt->first == nextY)
            {
                active.push_back(rangeIt->second);
                activeToRemove.push(rangeIt->second);
                rangeIt++;
            }
            sort(active.begin(), active.end(), CompareByX());

            count += CountRow(nextY, active);

            if(!activeToRemove.empty() && activeToRemove.top().y2 == nextY)
            {
                for(auto r : active) { if(r.y2 == nextY){ r.x = INT32_MAX; } }
                sort(active.begin(), active.end(), CompareByX());
                while(active.size() > 0 && active.back().x == INT32_MAX) { active.pop_back(); }

                while(!activeToRemove.empty() && activeToRemove.top().y2 == nextY) { activeToRemove.pop(); }
            }

            currentY = nextY;
        }

        return count;
    }

private:
    int64_t CountRow(int row, vector<VerticalRange>& active)
    {
        int64_t count = 0LL;

        VerticalRangeCross cross = NoneCross;
        int startX;

        for(int j = 0; j < (int)active.size(); j++)
        {
            VerticalRangeCross current = GetCross(row, active[j]);
            if(cross == NoneCross)
            {
                cross = current;
                startX = active[j].x;
            }
            else if (cross == current)
            {
                cross = NoneCross;
                count += active[j].x - startX + 1;
            }
            else
            {
                cross = (VerticalRangeCross)((int)current ^ (int)cross);
            }
        }

        return count;
    }

    VerticalRangeCross GetCross(int y, VerticalRange& range)
    {
        if(y == range.y1) return Start;
        if(y == range.y2) return End;
        if(range.y1 < y && y < range.y2) return Middle;
        return NoneCross;
    }

};

int main()
{
    int64_t sum = 0;
    char buffer[256];
    BigPlan plan;

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) <= 1) { continue; }
        plan.AddStep(DigStep(buffer));
    }

    sum = plan.CalculateArea();

    printf("%lld\n", sum);

    return 0;
}