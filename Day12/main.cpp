#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<cstdint>
#include<map>

using namespace std;

struct Index
{
    int Start;
    int End;
    int GroupId;

    Index(int start, int end, int groupId) : Start(start), End(end), GroupId(groupId) { }
};

bool operator < (const Index& i1, const Index& i2)
{
    return (i1.Start < i2.Start)
        || (i1.Start == i2.Start && i1.End < i2.End)
        || (i1.Start == i2.Start && i1.End == i2.End && i1.GroupId < i2.GroupId);
}

class Cache
{
private:
    map<Index, int64_t> cache;

public:
    Cache(){}

    bool Contains(Index& index)
    {
        return cache.find(index) != cache.end();
    }

    int64_t Get(Index& index)
    {
        return cache[index];
    }

    void Update(Index& index, int64_t value)
    {
        cache[index] = value;
    }
};

enum Spring
{
    Operational = '.',
    Damaged = '#',
    Unknown = '?'
};

class Record
{
private:
    vector<Spring> springs;
    vector<int> groups;
    Cache cache;

public:
    Record(char* line)
    {
        Parse(line);
    }

    Record Unfold(int multiplier)
    {
        Record result;

        for(int i = 0; i < multiplier; i++)
        {
            if(i != 0)
            {
                result.springs.push_back(Unknown);
            }

            for(auto s : springs) { result.springs.push_back(s); }
            for(auto g : groups) { result.groups.push_back(g); }
        }

        return result;
    }

    int64_t CountArrangements()
    {
        return Calculate(0, springs.size(), 0);
    }

private:
    Record() { }

    int64_t Calculate(int start, int end, int gPos)
    {
        Index id(start, end, gPos);

        if(cache.Contains(id)) { return cache.Get(id); }

        if(gPos == (int)groups.size())
        {
            for(int i = start; i < end; i++)
            {
                if(springs[i] == Damaged)
                {
                    cache.Update(id, 0LL);
                    return 0LL;
                }
            }

            cache.Update(id, 1LL);
            return 1LL;
        }

        int64_t result = 0LL;
        bool hasDamaged = false;
        int current = 0;
        int gSize = groups[gPos];

        for(int i = start; i < end; i++)
        {
            if(springs[i] == Operational)
            {
                if(hasDamaged) { break; }
                current = 0;
            }
            else
            {
                if(springs[i] == Damaged) { hasDamaged = true; }

                current++;
                if(current >= gSize)
                {
                    if(springs[i-gSize] == Damaged) { break; }
                    if(springs[i+1] == Damaged) { continue; }

                    result += Calculate(i+2, end, gPos+1);
                }
            }
        }

        cache.Update(id, result);
        return result;
    }

    void Parse(char* line)
    {
        ParseSprings(line);
        ParseGroups(strchr(line, ' ')+1);
    }

    bool IsSpring(char c)
    {
        return c == '.' || c == '#' || c == '?';
    }

    void ParseSprings(char* line)
    {
        springs.clear();

        int i = 0;
        while(IsSpring(line[i]))
        {
            springs.push_back((Spring)line[i]);
            i++;
        }
    }

    void ParseGroups(char* line)
    {
        char* token = strtok(line, ",");
        while (token != NULL)
        {
            int value = atoi(token);
            groups.push_back(value);
            token = strtok(NULL, ",");
        }
    }
};

int main()
{
    int64_t sum1 = 0;
    int64_t sum5 = 0;
    char buffer[1024];

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        Record record = Record(buffer);
        sum1 += record.CountArrangements();

        Record unfolded = record.Unfold(5);
        sum5 += unfolded.CountArrangements();
    }

    printf("%lld\n", sum1);
    printf("%lld\n", sum5);

    return 0;
}