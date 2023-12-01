#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<cctype>
#include<map>

using namespace std;

int match(map<string, int>& digitMap, char* text)
{
    string t = string(text);
    for (const auto& [key, value] : digitMap)
    {
        if(t.length() >= key.length())
        {
            string d = t.substr(0, key.length());
            if(key.compare(d) == 0)
            {
                return value;
            }
        }
    }

    return -1;
}

int main()
{
    int sum = 0;
    char buffer[256];
    int first, last;
    int i, v;

    map<string, int> digitMap = {
        { "1", 1 },
        { "2", 2 },
        { "3", 3 },
        { "4", 4 },
        { "5", 5 },
        { "6", 6 },
        { "7", 7 },
        { "8", 8 },
        { "9", 9 },
        { "one", 1 },
        { "two", 2 },
        { "three", 3 },
        { "four", 4 },
        { "five", 5 },
        { "six", 6 },
        { "seven", 7 },
        { "eight", 8 },
        { "nine", 9 }
    };

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) > 1)
        {
            i = 0;
            while((v = match(digitMap, buffer+i)) == -1)
            {
                i++;
            }
            first = v;

            while(buffer[i] != 0)
            {
                if((v = match(digitMap, buffer+i)) != -1)
                {
                    last = v;
                }
                i++;
            }

            sum += 10 * first + last;
        }
    }

    printf("%i\n", sum);

    return 0;
}