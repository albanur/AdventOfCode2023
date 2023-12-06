#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<cstdint>
#include<string>

using namespace std;

int64_t WaysToWin(int64_t time, int64_t dist)
{
    int64_t result = 0;
    for(int i = 1; i < time; i++)
    {
        if( i * (time-i) > dist)
        {
            result++;
        }
    }
    return result;
}

int main()
{
    int64_t result = 1;
    int64_t totalResult = 0;
    char buffer[256];
    string digits;
    char* token;
    vector<int64_t> time;
    vector<int64_t> dist;
    int64_t totalTime;
    int64_t totalDist;

    fgets(buffer, 256, stdin);
    token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    while (token != NULL)
    {
        while(*token == ' ')
        {
            token++;
        }
        int64_t item = atoll(token);
        time.push_back(item);
        token = strtok(NULL, " ");
    }

    digits.clear();
    for(int i = 0; i < 256 && buffer[i] != '\n'; i++)
    {
        if(isdigit(buffer[i]))
        {
            digits.push_back(buffer[i]);
        }
    }
    totalTime = atoll(digits.c_str());

    fgets(buffer, 256, stdin);
    token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    while (token != NULL)
    {
        while(*token == ' ')
        {
            token++;
        }
        int64_t item = atoll(token);
        dist.push_back(item);
        token = strtok(NULL, " ");
    }

    digits.clear();
    for(int i = 0; i < 256 && buffer[i] != '\n'; i++)
    {
        if(isdigit(buffer[i]))
        {
            digits.push_back(buffer[i]);
        }
    }
    totalDist = atoll(digits.c_str());

    for(int i = 0; i < time.size(); i++)
    {
        result *= WaysToWin(time[i], dist[i]);
    }

    totalResult = WaysToWin(totalTime, totalDist);

    printf("%lld\n", result);
    printf("%lld\n", totalResult);

    return 0;
}