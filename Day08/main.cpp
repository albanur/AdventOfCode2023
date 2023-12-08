#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>
#include<map>
#include<vector>
#include<cstdint>

using namespace std;

int64_t GCD(int64_t a, int64_t b)
{
    int64_t x;
    
	while(b!=0LL)
    {
		x = b;
		b = a%b;
		a = x;	
	}
	
    return a;
}

int64_t LCM(int64_t a, int64_t b)
{
    int64_t gcd = GCD(a,b);
    return (a / gcd) * b;
}

struct Node
{
    string Name;
    string Left;
    string Right;

    Node(const char* line)
    {
        Name = string(line, 3);
        Left = string(line + 7, 3);
        Right = string(line + 12, 3);
    }

    Node()
    {
        Name = "";
        Left = "";
        Right = "";
    }
};

bool IsFrequencyKnown(vector<int64_t>& frequency)
{
    for(int i = 0; i < frequency.size(); i++)
    {
        if(frequency[i] == 0LL) { return false; }
    }

    return true;
}

int main()
{
    int steps = 0;
    int64_t ghostSteps = 0LL;
    char buffer[1024];
    vector<char> commands;
    map<string, Node> nodes;
    int i;
    string current;
    vector<string> ghostCurrent;
    vector<int64_t> frequency;

    fgets(buffer, 1024, stdin);
    i = 0;
    while(buffer[i] == 'R' || buffer[i] == 'L')
    {
        commands.push_back(buffer[i]);
        i++;
    }

    fgets(buffer, 1024, stdin);

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) > 15)
        {
            Node n = Node(buffer);
            nodes[n.Name] = n;

            if(n.Name[2] == 'A')
            {
                ghostCurrent.push_back(n.Name);
                frequency.push_back(0LL);
            }
        }
    }

    i = 0;
    current = "AAA";

    while(current != "ZZZ")
    {
        steps++;

        Node n = nodes[current];
        if(commands[i] == 'R')
        {
            current = n.Right;
        }
        else
        {
            current = n.Left;
        }

        i++;
        if(i == (int)commands.size()) { i = 0; }
    }

    i = 0;
    while(!IsFrequencyKnown(frequency))
    {
        ghostSteps++;

        for(int j = 0; j < ghostCurrent.size(); j++)
        {
            Node n = nodes[ghostCurrent[j]];
            if(commands[i] == 'R')
            {
                ghostCurrent[j] = n.Right;
            }
            else
            {
                ghostCurrent[j] = n.Left;
            }
            if(ghostCurrent[j][2] == 'Z' && frequency[j] == 0LL)
            {
                frequency[j] = ghostSteps;
            }
        }

        i++;
        if(i == (int)commands.size()) { i = 0; }
    }
    
    ghostSteps = 1LL;
    for(int j = 0; j < frequency.size(); j++)
    {
        ghostSteps = LCM(ghostSteps, frequency[j]);
    }

    printf("%i\n", steps);
    printf("%lld\n", ghostSteps);

    return 0;
}