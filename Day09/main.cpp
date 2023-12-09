#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<algorithm>

using namespace std;

bool AllZeros(vector<int>& analysis, int size)
{
    for(int i = 0; i < size; i++)
    {
        if(analysis[i] != 0) { return false; }
    }
    return true;
}

void DownStep(vector<int>& analysis, int size)
{
    for(int i = 0; i < (size-1); i++)
    {
        analysis[i] = analysis[i+1] - analysis[i];
    }
}

int LastValue(vector<int>& analysis, int size)
{
    int sum = 0;
    for(int i = size; i < analysis.size(); i++)
    {
        sum += analysis[i];
    }
    return sum;
}

int ExtrapolateNext(vector<int>& analysis)
{
    int size = analysis.size();

    while(!AllZeros(analysis, size))
    {
        DownStep(analysis, size);
        size--;
    }

    return LastValue(analysis, size);
}

int ExtrapolatePrevious(vector<int>& analysis)
{
    int size = analysis.size();
    reverse(analysis.begin(), analysis.end());

    while(!AllZeros(analysis, size))
    {
        DownStep(analysis, size);
        size--;
    }

    return LastValue(analysis, size);
}

int main()
{
    int sum = 0;
    int sumPrev = 0;
    char buffer[1024];
    vector<int> analysis;
    vector<int> analysisPrev;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) <= 1)
        {
            continue;
        }

        analysis.clear();
        analysisPrev.clear();
        char* token = strtok(buffer, " ");
        while (token != NULL)
        {
            int value = atoi(token);
            analysis.push_back(value);
            analysisPrev.push_back(value);
            token = strtok(NULL, " ");
        }

        sum += ExtrapolateNext(analysis);
        sumPrev += ExtrapolatePrevious(analysisPrev);
    }

    printf("%i\n", sum);
    printf("%i\n", sumPrev);

    return 0;
}