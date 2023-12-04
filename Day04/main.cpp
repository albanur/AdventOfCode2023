#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<set>
#include<map>

using namespace std;

int readNumber(char* pointer)
{
    if(*pointer == ' ')
    {
        return pointer[1] - '0';
    }

    return 10*(pointer[0] - '0') + pointer[1] - '0';
}

bool contains(map<int, int>& map, int key)
{
    return map.find(key) != map.end();
}

void increase(map<int, int>& map, int key, int value)
{
    if(!contains(map, key))
    {
        map[key] = value;
    }
    else
    {
        map[key] += value;
    }
}

int main()
{
    int sum = 0;
    int cardSum = 0;
    char buffer[1024];
    set<int> winning;
    set<int> numbers;
    map<int, int> multiplier;
    int v, i, m, p;
    char* start;
    char* separator;
    char* end;
    char* current;

    i = 0;
    while(fgets(buffer, 1024, stdin) != NULL)
    {
        i++;
        winning.clear();
        numbers.clear();
        increase(multiplier, i, 1);
        
        start = strchr(buffer, ':');
        separator = strchr(buffer, '|');
        end = buffer + strlen(buffer);

        current = start + 2;
        while(current < separator)
        {
            v = readNumber(current);
            winning.insert(v);
            current += 3;
        }

        current = separator + 2;
        while(current < end)
        {
            v = readNumber(current);
            numbers.insert(v);
            current += 3;
        }

        v = 1;
        p = 0;
        m = multiplier[i];
        for(auto w : winning) {
            if(numbers.find(w) != numbers.end())
            {
                v <<= 1;
                p++;
            }
        }

        v >>= 1;
        sum+=v;

        for(int j = 1; j <= p; j++)
        {
            increase(multiplier, i+j, m);
        }
    }

    for (const auto& [key, value] : multiplier)
    {
        cardSum += value;
    }

    printf("%i\n", sum);
    printf("%i\n", cardSum);

    return 0;
}