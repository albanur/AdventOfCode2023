#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<map>
#include<algorithm>

using namespace std;

struct IntPoint3D
{
    int64_t x;
    int64_t y;
    int64_t z;

    IntPoint3D(int64_t x = 0, int64_t y = 0, int64_t z=0) : x(x), y(y), z(z) { }
};

struct FloatPoint3D
{
    long double x;
    long double y;
    long double z;

    FloatPoint3D(long double x = 0, long double y = 0, long double z=0) : x(x), y(y), z(z) { }
    FloatPoint3D(IntPoint3D& point) : x((long double)point.x), y((long double)point.y), z((long double)point.z) {}
};

struct Hailstone
{
    IntPoint3D Position;
    IntPoint3D Velocity;

    Hailstone() : Position(), Velocity() { }
    Hailstone(IntPoint3D& position, IntPoint3D& velocity) : Position(position), Velocity(velocity) { }
};

class HailstoneParser
{
public:
    Hailstone Parse(char* line)
    {
        if(strlen(line) <= 1) { return Hailstone(); }

        int64_t px, py, pz;
        int64_t vx, vy, vz;

        sscanf(line, "%lld, %lld, %lld @ %lld, %lld, %lld", &px, &py, &pz, &vx, &vy, &vz);

        IntPoint3D position(px, py, pz);
        IntPoint3D velocity(vx, vy, vz);

        return Hailstone(position, velocity);
    }
};

class HailstoneSolver
{
private:
    const long double Epsilon = 1e-15L;
    long double rangeBegin;
    long double rangeEnd;
    bool hasSolution;
    FloatPoint3D position1;
    FloatPoint3D position2;
    long double time1;
    long double time2;

public:
    HailstoneSolver()
    {
        hasSolution = false;
    }

    void SetRange(long double rangeBegin, long double rangeEnd)
    {
        this->rangeBegin = rangeBegin;
        this->rangeEnd = rangeEnd;
    }

    bool Solve2D(const Hailstone& hailstone1, const Hailstone& hailstone2)
    {
        hasSolution = false;

        long double bx = hailstone2.Position.x - hailstone1.Position.x;
        long double by = hailstone2.Position.y - hailstone1.Position.y;

        long double ax1 = hailstone1.Velocity.x;
        long double ax2 = -hailstone2.Velocity.x;
        long double ay1 = hailstone1.Velocity.y;
        long double ay2 = -hailstone2.Velocity.y;

        long double m = ax2 / ay2;
        ax1 -= m * ay1;
        ax2 -= m * ay2;
        bx -= m * by;

        if(abs(ax1) < Epsilon)
        {
            hasSolution = false;
            return false;
        }

        m = ax1;
        ax1 /= m;
        ax2 /= m;
        bx /= m;

        m = ay1;
        ay1 -= m * ax1;
        ay2 -= m * ax2;
        by -= m * bx;

        m = ay2;
        ay1 /= m;
        ay2 /= m;
        by /= m;

        time1 = bx;
        time2 = by;

        position1 = FloatPoint3D(
            hailstone1.Position.x + time1 * hailstone1.Velocity.x,
            hailstone1.Position.y + time1 * hailstone1.Velocity.y,
            hailstone1.Position.z + time1 * hailstone1.Velocity.z);

        position2 = FloatPoint3D(
            hailstone2.Position.x + time2 * hailstone2.Velocity.x,
            hailstone2.Position.y + time2 * hailstone2.Velocity.y,
            hailstone2.Position.z + time2 * hailstone2.Velocity.z);

        hasSolution = true;
        return true;
    }

    bool IsValid2D()
    {
        bool inRange = rangeBegin <= position1.x && position1.x <= rangeEnd
            && rangeBegin <= position1.y && position1.y <= rangeEnd;
        bool inTime = time1 >= 0.0L && time2 >= 0.0L;

        return inRange && inTime;
    }
};

int main()
{
    int result = 0;
    char buffer[1024];
    vector<Hailstone> hailstones;
    HailstoneParser parser;
    HailstoneSolver solver;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) <= 1) { continue; }
        hailstones.push_back(parser.Parse(buffer));
    }

    //solver.SetRange(7.0L, 27.0L);
    solver.SetRange(200000000000000.0L, 400000000000000.0L);

    for(int i = 0; i < (int)hailstones.size(); i++)
    {
        for(int j = i+1; j < (int)hailstones.size(); j++)
        {
            if(solver.Solve2D(hailstones[i], hailstones[j]))
            {
                if(solver.IsValid2D()) { result++; }
            }
        }
    }

    printf("%i\n", result);

    return 0;
}