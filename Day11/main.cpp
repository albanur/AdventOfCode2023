#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<map>
#include<set>
#include<vector>
#include<cstdint>

using namespace std;

struct Point
{
    int x;
    int y;

    Point(int x = 0, int y = 0) : x(x), y(y) { }
};

bool operator < (const Point& p1, const Point& p2)
{
    return (p1.x < p2.x) || (p1.x == p2.x && p1.y < p2.y);
}

int ExpantionSum(const vector<int>& expantions, int start, int end)
{
    int s, e;

    if(start < end)
    {
        s = start+1;
        e = end-1;
    }
    else
    {
        s = end+1;
        e = start-1;
    }

    int result = 0;
    for(int i = s; i <= e; i++)
    {
        if(expantions[i] == 0) result++;
    }
    return result;
}

int Abs(int i)
{
    return i < 0 ? -i : i;
}

int main()
{
    int64_t sum1 = 0LL;
    int64_t sum2 = 0LL;
    int64_t multiplier = 1000000LL;
    char buffer[1024];
    set<Point> galaxies;
    vector<int> verticalCount;
    vector<int> horizontalCount;

    int row = 0;
    while(fgets(buffer, 1024, stdin) != NULL)
    {
        int length = strlen(buffer);
        if(length <= 1) { continue; }
        if(row == 0)
        {
            horizontalCount.assign(length, 0);
        }
        verticalCount.push_back(0);

        for(int i = 0; i < length; i++)
        {
            if(buffer[i] == '#')
            {
                galaxies.insert(Point(i, row));
                horizontalCount[i]++;
                verticalCount[row]++;
            }
        }

        row++;
    }

    for(auto p1 : galaxies)
    {
        for(auto p2: galaxies)
        {
            int64_t distance1 = (int64_t)Abs(p1.x - p2.x) + (int64_t)ExpantionSum(horizontalCount, p1.x, p2.x)
                + (int64_t)Abs(p1.y - p2.y) + (int64_t)ExpantionSum(verticalCount, p1.y, p2.y);
            sum1 += distance1;

            int64_t distance2 = (int64_t)Abs(p1.x - p2.x) + (multiplier-1) * (int64_t)ExpantionSum(horizontalCount, p1.x, p2.x)
                + (int64_t)Abs(p1.y - p2.y) + (multiplier-1) * (int64_t)ExpantionSum(verticalCount, p1.y, p2.y);
            sum2 += distance2;
        }
    }

    printf("%lld\n", sum1/2);
    printf("%lld\n", sum2/2);

    return 0;
}