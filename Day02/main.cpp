#include<cstdio>
#include<cstdlib>
#include<cstring>

using namespace std;

int main()
{
    int sum = 0;
    int powerSum = 0;
    char buffer[1024];
    char* pointer;
    int l, id, v;
    int r, g, b;
    int maxR, maxG, maxB;

    const int LimitR = 12;
    const int LimitG = 13;
    const int LimitB = 14;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        l = strlen(buffer);
        maxR = maxG = maxB = 0;

        if(l <= 5)
        {
            continue;
        }

        id = atoi(strchr(buffer, ' ')+1);

        pointer = strchr(buffer, ':');

        while(pointer < buffer + l)
        {
            r = g = b = 0;
            while(true)
            {
                v = atoi(pointer+2);
                pointer = strchr(pointer+3, ' ');
                switch(*(pointer+1))
                {
                    case 'r':
                        r = v;
                        pointer += 4;
                        break;
                    case 'g':
                        g = v;
                        pointer += 6;
                        break;
                    case 'b':
                        b = v;
                        pointer += 5;
                        break;
                }

                if(*pointer != ',')
                {
                    break;
                }
            }

            maxR = r > maxR ? r : maxR;
            maxG = g > maxG ? g : maxG;
            maxB = b > maxB ? b : maxB;

            if(*pointer != ';')
            {
                break;
            }
        }

        if(maxR <= LimitR && maxG <= LimitG && maxB <= LimitB)
        {
            sum += id;
        }

        powerSum += maxR * maxG * maxB;
    }

    printf("%i\n", sum);
    printf("%i\n", powerSum);

    return 0;
}