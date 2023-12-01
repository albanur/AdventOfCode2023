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
    map<string, int> digitMap;

    digitMap["1"] = 1;
    digitMap["2"] = 2;
    digitMap["3"] = 3;
    digitMap["4"] = 4;
    digitMap["5"] = 5;
    digitMap["6"] = 6;
    digitMap["7"] = 7;
    digitMap["8"] = 8;
    digitMap["9"] = 9;
    digitMap["one"] = 1;
    digitMap["two"] = 2;
    digitMap["three"] = 3;
    digitMap["four"] = 4;
    digitMap["five"] = 5;
    digitMap["six"] = 6;
    digitMap["seven"] = 7;
    digitMap["eight"] = 8;
    digitMap["nine"] = 9;

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

            //printf("%i\n", 10 * first + last);
            sum += 10 * first + last;
        }
    }

    printf("%i\n", sum);

    return 0;
}