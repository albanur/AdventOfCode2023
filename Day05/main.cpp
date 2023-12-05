#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<vector>
#include<stack>
#include<cctype>

using namespace std;

struct Range
{
    int64_t start;
    int64_t length;

    Range(int64_t s = 0, int64_t l = 0) : start(s), length(l) { }
};

class MappingRange
{
    int64_t destination;
    int64_t source;
    int64_t length;

public:
    MappingRange(int64_t d = 0, int64_t s = 0, int64_t l = 0) : destination(d), source(s), length(l) { }

    bool IsInMappingRange(int64_t v)
    {
        return v >= source && v < source+length;
    }

    int64_t Convert(int64_t v)
    {
        return v + destination - source;
    }

    bool Intersect(Range r, Range& prefix, Range& intersect, Range& suffix)
    {
        if(r.start < source)
        {
            if(r.start + r.length > source)
            {
                prefix = Range(r.start, source - r.start);
                if(r.start + r.length > source + length)
                {
                    intersect = Range(source, length);
                    suffix = Range(source + length, r.start + r.length - source - length);
                }
                else
                {
                    intersect = Range(source, r.start + r.length - source);
                    suffix = Range();
                }
                return true;
            }
            return false;
        }
        if (r.start < source + length)
        {
            prefix = Range();
            if(r.start + r.length > source + length)
            {
                intersect = Range(r.start, source + length - r.start);
                suffix = Range(source + length, r.start + r.length - source - length);
            }
            else
            {
                intersect = Range(r.start, r.length);
                suffix = Range();
            }
            return true;
        }
        return false;
    }
};

class Mapping
{
private:
    vector<MappingRange> MappingRanges;

public:
    Mapping() {}
    void AddMappingRange(MappingRange r) { MappingRanges.push_back(r); }

    int64_t Convert(int64_t v)
    {
        for(auto r : MappingRanges)
        {
            if(r.IsInMappingRange(v))
            {
                return r.Convert(v);
            }
        }
        return v;
    }

    void Convert(vector<Range>& input)
    {
        vector<Range> current, next;
        Range prefix, intersect, suffix;

        Move(input, next);

        for(auto m : MappingRanges)
        {
            current.clear();
            Move(next, current);

            for(auto r : current)
            {
                if(m.Intersect(r, prefix, intersect, suffix))
                {
                    input.push_back(Range(m.Convert(intersect.start), intersect.length));
                    if (prefix.length > 0)
                    {
                        next.push_back(prefix);
                    }
                    if (suffix.length > 0)
                    {
                        next.push_back(suffix);
                    }
                }
                else
                {
                    next.push_back(r);
                }
            }
        }

        for(auto r : next)
        {
            input.push_back(r);
        }
    }

private:
    void Move(vector<Range>& source, vector<Range>& destination)
    {
        destination.clear();
        for(auto r : source)
        {
            destination.push_back(r);
        }
        source.clear();
    }
};

class MappingParser
{
private:
    const static int BufferSize = 256;
    char buffer[BufferSize];

public:
    MappingParser() {}

    bool FindMapping()
    {
        while(ReadLine())
        {
            if (IsFirstMappingLine())
            {
                return true;
            }
        }
        return false;
    }

    Mapping ReadMapping()
    {
        Mapping mapping;
        while(ReadLine() && IsMappingRangeLine())
        {
            mapping.AddMappingRange(ParseMappingRange());
        }

        return mapping;
    }

private:
    bool ReadLine()
    {
        return fgets(buffer, BufferSize, stdin) != NULL;
    }

    bool IsFirstMappingLine()
    {
        return isalpha(buffer[0]);
    }

    bool IsMappingRangeLine()
    {
        return isdigit(buffer[0]);
    }

    MappingRange ParseMappingRange()
    {
        int64_t d, s, l;
        sscanf(buffer, "%lld %lld %lld", &d, &s, &l);
        return MappingRange(d, s, l);
    }
};

int main(int argc, char *argv[])
{
    if(argc > 1)
    {
        freopen(argv[1], "r", stdin);
    }

    int64_t min = INT64_MAX;
    int64_t minRange = INT64_MAX;
    char buffer[1024];
    vector<int64_t> seeds;
    vector<Range> seedRanges;
    vector<Mapping> mappings;
    MappingParser parser;

    fgets(buffer, 1024, stdin);

    char* token = strtok(buffer, " ");
    token = strtok(NULL, " ");

    while (token != NULL)
    {
        int64_t item = atoll(token);
        seeds.push_back(item);
        token = strtok(NULL, " ");
    }

    for(size_t i = 0; i < seeds.size(); i += 2)
    {
        seedRanges.push_back(Range(seeds[i], seeds[i+1]));
    }

    while(parser.FindMapping())
    {
        Mapping mapping = parser.ReadMapping();

        mappings.push_back(mapping);
    }

    for(auto s : seeds)
    {
        int64_t v = s;
        for(auto m : mappings)
        {
            v = m.Convert(v);
        }

        if(v < min)
        {
            min = v;
        }
    }

    for(auto m : mappings)
    {
        m.Convert(seedRanges);
    }

    for(auto r : seedRanges)
    {
        if(r.start < minRange)
        {
            minRange = r.start;
        }
    }

    printf("%lld\n", min);
    printf("%lld\n", minRange);

    if(argc > 1)
    {
        fclose(stdin);
    }

    return 0;
}