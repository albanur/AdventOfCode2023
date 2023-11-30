#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>

using namespace std;

int main()
{
    vector<int> calories;
    int calorie;
    char buffer[256];
    int current = 0;

    while(fgets(buffer, 256, stdin) != NULL)
    {
        if(strlen(buffer) > 1)
        {
            calorie = atoi(buffer);
            current += calorie;
        }
        else
        {
            calorie = -1;
            current = 0;
        }

        calories.push_back(calorie);
    }

    printf("%i\n", 0);

    return 0;
}