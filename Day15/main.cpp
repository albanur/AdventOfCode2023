#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<cctype>
#include<vector>
#include<algorithm>

using namespace std;

void UpdateHash(int& hash, char c)
{
    hash += (int)c;
    hash = (hash*17) % 256;
};

enum OperationType
{
    AddReplace = '=',
    Remove = '-',
};

struct Operation
{
    string Label;
    OperationType Type;
    int Value;

    Operation() {}

    void Clear() { Label.clear(); Value = 0; }

    bool ParseChar(char c)
    {
        if(isalpha(c))
        {
            Label.push_back(c);
            return true;
        }
        else if(isdigit(c))
        {
            Value = c - '0';
            return true;
        }
        else if(c == '=' || c == '-')
        {
            Type = (OperationType)c;
            return true;
        }

        return false;
    }

    int CalculateHash()
    {
        int hash = 0;
        for(auto c : Label)
        {
            UpdateHash(hash, c);
        }
        return hash;
    }
};

struct Lens
{
    string Label;
    int Value;

    Lens() : Label(""), Value(0) { }
    Lens(string& label, int value) : Label(label), Value(value) {}
};

class Box
{
private:
    vector<Lens> lenses;

public:
    Box() {}

    void AddReplace(string& label, int value)
    {
        int ind = FindLens(label);
        if(ind == -1)
        {
            lenses.push_back(Lens(label, value));
        }
        else
        {
            lenses[ind].Value = value;
        }
    }

    void Remove(string& label)
    {
        int ind = FindLens(label);
        if(ind != -1)
        {
            lenses.erase(lenses.begin()+ind);
        }
    }

    int64_t CalculateFocusingPower(int boxId)
    {
        int64_t result = 0LL;

        for(int i = 0; i < (int)lenses.size(); i++)
        {
            result += boxId * (i+1) * lenses[i].Value;
        }

        return result;
    }

private:
    int FindLens(string& label)
    {
        for(int i = 0; i < (int)lenses.size(); i++)
        {
            if(lenses[i].Label.compare(label) == 0)
            {
                return i;
            }
        }

        return -1;
    }
};

class State
{
private:
    vector<Box> boxes;

public:
    State()
    {
        boxes = vector<Box>(256);
    }

    void PerformOperation(Operation& op)
    {
        int hash = op.CalculateHash();
        
        if(op.Type == AddReplace)
        {
            boxes[hash].AddReplace(op.Label, op.Value);
        }

        if(op.Type == Remove)
        {
            boxes[hash].Remove(op.Label);
        }
    }

    int64_t CalculateFocusingPower()
    {
        int64_t result = 0LL;

        for(int i = 0; i < (int)boxes.size(); i++)
        {
            result += boxes[i].CalculateFocusingPower(i+1);
        }

        return result;
    }
};

int main()
{
    int64_t sum = 0;
    int hash = 0;
    char c;
    Operation op;
    State state;
    
    do
    {
        c = getchar();
        if(c == ',' || c == '\n' || c == EOF)
        {
            sum += hash;
            hash = 0;
        }
        else
        {
            UpdateHash(hash, c);
        }

        if(!op.ParseChar(c))
        {
            state.PerformOperation(op);
            op.Clear();
        }
    } while (c != EOF && c != '\n');

    printf("%lld\n", sum);
    printf("%lld\n", state.CalculateFocusingPower());

    return 0;
}