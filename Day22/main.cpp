#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<unordered_set>
#include<stack>
#include<algorithm>

using namespace std;

struct Point3D
{
    int x;
    int y;
    int z;

    Point3D(int x = 0, int y = 0, int z=0) : x(x), y(y), z(z) { }
};

struct Brick
{
    Point3D start;
    Point3D end;

    Brick(char* line)
    {
        int x, y, z;

        char* pointer = line;
        sscanf(pointer, "%i,%i,%i", &x, &y, &z);
        start = Point3D(x, y, z);

        pointer = strchr(line, '~') + 1;
        sscanf(pointer, "%i,%i,%i", &x, &y, &z);
        end = Point3D(x, y, z);

        if(start.x > end.x) { swap(start.x, end.x); }
        if(start.y > end.y) { swap(start.y, end.y); }
        if(start.z > end.z) { swap(start.z, end.z); }
    }

    void MoveDown()
    {
        start.z--;
        end.z--;
    }
};

class SandPile
{
private:
    vector<Brick> bricks;
    vector<unordered_set<Brick*>> zStack;

public:
    SandPile() {}

    void AddBrick(Brick& brick)
    {
        bricks.push_back(brick);
    }

    void FallProcedure()
    {
        FillZStack();

        for(int z = 1; z < (int)zStack.size(); z++)
        {
            vector<Brick*> level(zStack[z].begin(), zStack[z].end());

            for(Brick* brick : level)
            {
                while(CanMoveDown(brick))
                {
                    MoveDown(brick);
                }
            }
        }
    }

    int CountSafeBricks()
    {
        int counter = 0;

        for(int i = 0; i < (int)bricks.size(); i++)
        {
            Brick* brick = &(bricks[i]);

            zStack[brick->end.z].erase(brick);

            for(Brick* topBrick : zStack[brick->end.z+1])
            {
                if(CanMoveDown(topBrick))
                {
                    counter++;
                    break;
                }
            }

            zStack[brick->end.z].insert(brick);
        }

        return bricks.size() - counter;
    }

    int CountImpactedBricks()
    {
        int counter = 0;
        stack<Brick*> removed;

        for(int i = 0; i < (int)bricks.size(); i++)
        {
            Brick* brick = &(bricks[i]);

            RemoveFromZStack(brick);
            removed.push(brick);

            for(int z = brick->end.z + 1; z < (int)zStack.size(); z++)
            {
                vector<Brick*> level(zStack[z].begin(), zStack[z].end());

                for(Brick* b : level)
                {
                    if(CanMoveDown(b))
                    {
                        counter++;
                        RemoveFromZStack(b);
                        removed.push(b);
                    }
                }
            }

            while(!removed.empty())
            {
                Brick* b = removed.top();
                removed.pop();
                AddToZStack(b);
            }
        }

        return counter;
    }

private:
    bool IsOccupied(Point3D& point)
    {
        for(Brick* brick : zStack[point.z])
        {
            if (brick->start.x <= point.x && point.x <= brick->end.x
                && brick->start.y <= point.y && point.y <= brick->end.y)
            {
                return true;
            }
        }

        return false;
    }

    bool CanMoveDown(Brick* brick)
    {
        if(brick->start.z <= 1) { return false; }

        Point3D point(0, 0, brick->start.z-1);

        for(int x = brick->start.x; x <= brick->end.x; x++)
        {
            point.x = x;
            for(int y = brick->start.y; y <= brick->end.y; y++)
            {
                point.y = y;
                if(IsOccupied(point)) { return false; }
            }
        }

        return true;
    }

    void MoveDown(Brick* brick)
    {
        zStack[brick->end.z].erase(brick);
        brick->MoveDown();
        zStack[brick->start.z].insert(brick);
    }

    void FillZStack()
    {
        zStack.clear();
        for(int i = 0; i < (int)bricks.size(); i++)
        {
            Brick* b = &(bricks[i]);
            AddToZStack(b);
        }
    }

    void AddToZStack(Brick* brick)
    {
        if(brick->end.z >= (int)zStack.size()) { zStack.resize(brick->end.z + 2); }

        for(int z = brick->start.z; z <= brick->end.z; z++)
        {
            zStack[z].insert(brick);
        }
    }

    void RemoveFromZStack(Brick* brick)
    {
        for(int z = brick->start.z; z <= brick->end.z; z++)
        {
            zStack[z].erase(brick);
        }
    }
};

int main()
{
    int64_t result = 0;
    char buffer[1024];
    SandPile sandPile;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) >= 1)
        {
            Brick brick(buffer);
            sandPile.AddBrick(brick);
        }
    }

    sandPile.FallProcedure();

    result = sandPile.CountSafeBricks();
    printf("%lld\n", result);

    result = sandPile.CountImpactedBricks();
    printf("%lld\n", result);

    return 0;
}