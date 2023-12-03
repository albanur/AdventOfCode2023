#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<vector>
#include<string>
#include<map>

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

bool isSymbol(char c)
{
    return !isdigit(c) && c != '.' && c!= '\0' && c != '\r' && c != '\n';
}

bool isValidPoint(vector<string>& symbols, int r, int c)
{
    return r >= 0 && r < (int)symbols.size() && c >= 0 && c < (int)symbols[r].size();
}

bool isAdjecentToSymbol(vector<string>& symbols, int r, int c)
{
    return (isValidPoint(symbols, r-1, c-1) && isSymbol(symbols[r-1][c-1]))
        || (isValidPoint(symbols, r-1, c) && isSymbol(symbols[r-1][c]))
        || (isValidPoint(symbols, r-1, c+1) && isSymbol(symbols[r-1][c+1]))
        || (isValidPoint(symbols, r, c-1) && isSymbol(symbols[r][c-1]))
        || (isValidPoint(symbols, r, c+1) && isSymbol(symbols[r][c+1]))
        || (isValidPoint(symbols, r+1, c-1) && isSymbol(symbols[r+1][c-1]))
        || (isValidPoint(symbols, r+1, c) && isSymbol(symbols[r+1][c]))
        || (isValidPoint(symbols, r+1, c+1) && isSymbol(symbols[r+1][c+1]));
}

bool isAdjecentToStar(vector<string>& symbols, int r, int c, int& x, int& y)
{
    if(isValidPoint(symbols, r-1, c-1) && symbols[r-1][c-1] == '*')
    {
        x = r-1; y = c-1; return true;
    }
    if(isValidPoint(symbols, r-1, c) && symbols[r-1][c] == '*')
    {
        x = r-1; y = c; return true;
    }
    if(isValidPoint(symbols, r-1, c+1) && symbols[r-1][c+1] == '*')
    {
        x = r-1; y = c+1; return true;
    }
    if(isValidPoint(symbols, r, c-1) && symbols[r][c-1] == '*')
    {
        x = r; y = c-1; return true;
    }
    if(isValidPoint(symbols, r, c+1) && symbols[r][c+1] == '*')
    {
        x = r; y = c+1; return true;
    }
    if(isValidPoint(symbols, r+1, c-1) && symbols[r+1][c-1] == '*')
    {
        x = r+1; y = c-1; return true;
    }
    if(isValidPoint(symbols, r+1, c) && symbols[r+1][c] == '*')
    {
        x = r+1; y = c; return true;
    }
    if(isValidPoint(symbols, r+1, c+1) && symbols[r+1][c+1] == '*')
    {
        x = r+1; y = c+1; return true;
    }

    return false;
}

int main()
{
    int sum = 0;
    int gearSum = 0;
    char buffer[1024];
    vector<string> symbols;
    map<Point, int> gears;
    int v, s;
    bool a, g;
    int x, y;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        symbols.push_back(string(buffer));
    }

    for(size_t i = 0; i < symbols.size(); i++)
    {
        for(size_t j = 0; j < symbols[i].size(); j++)
        {
            if(isdigit(symbols[i][j]))
            {
                v = atoi(symbols[i].c_str() + j);
                a = g = false;
                s = j;
                x = y = 0;

                while(isdigit(symbols[i][s]))
                {
                    a |= isAdjecentToSymbol(symbols, i, s);
                    g |= isAdjecentToStar(symbols, i, s, x, y);
                    s++;
                }

                if(a)
                {
                    sum += v;

                    if(g)
                    {
                        map<Point, int>::iterator it;
                        if((it = gears.find(Point(x,y))) != gears.end())
                        {
                            gearSum += it->second * v;
                        }
                        else
                        {
                            gears[Point(x,y)] = v;
                        }
                    }
                }
                j = s;
            }
        }
    }

    printf("%i\n", sum);
    printf("%i\n", gearSum);

    return 0;
}