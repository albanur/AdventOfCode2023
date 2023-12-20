#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<vector>
#include<map>
#include<string>
#include<stack>

using namespace std;

struct Part
{
    int x;
    int m;
    int a;
    int s;

    Part(int x, int m, int a, int s): x(x), m(m), a(a), s(s) { }

    Part(const char* line)
    {
        char* pointer = strchr(line, '=');;
        while(pointer != NULL)
        {
            Parse(pointer-1);
            pointer = strchr(pointer+1, '=');
        }
    }

    int64_t Sum()
    {
        return x + m + a + s;
    }

private:
    void Parse(char* pointer)
    {
        int v = atoi(pointer+2);
        switch(pointer[0])
        {
            case 'x': x = v; break;
            case 'm': m = v; break;
            case 'a': a = v; break;
            case 's': s = v; break;
        }
    }
};

struct Range
{
    short from;
    short to;

    Range() : from(1), to(4000) { }

    int Count() { return to - from + 1; }

    void Normalize()
    {
        if(to < from)
        {
            to = from-1;
        }
    }
};

struct PartRange
{
    string Label;
    Range x;
    Range m;
    Range a;
    Range s;

    PartRange() : Label("in") { }

    int64_t Count()
    {
        return (int64_t)x.Count() * (int64_t)m.Count() * (int64_t)a.Count() * (int64_t)s.Count();
    }

    void Normalize()
    {
        x.Normalize();
        m.Normalize();
        a.Normalize();
        s.Normalize();
    }
};

class Rule
{
public:
    virtual ~Rule() {}
    virtual bool Matches(Part& part) = 0;
    virtual string MatchLabel() = 0;
    virtual void Process(PartRange& range, PartRange& rest) = 0;
};

class LessRule : public Rule
{
private:
    char parameter;
    int value;
    string label;

public:
    LessRule(char parameter, int value, string label)
        : parameter(parameter), value(value), label(label) { };
    
    bool Matches(Part& part)
    {
        int v = GetValue(part);
        return v < value;
    }

    string MatchLabel()
    {
        return label;
    }

    void Process(PartRange& range, PartRange& rest)
    {
        rest = range;
        Truncate(range, rest);
        range.Normalize();
        rest.Normalize();
        range.Label = label;
    }

private:
    int GetValue(Part& part)
    {
        switch (parameter)
        {
            case 'x': return part.x;
            case 'm': return part.m;
            case 'a': return part.a;
            case 's': return part.s;
            default: return 0;
        }
    }

    void Truncate(PartRange& range, PartRange& rest)
    {
        switch (parameter)
        {
            case 'x':
                range.x.to = value-1;
                rest.x.from = value;
                break;
            case 'm':
                range.m.to = value-1;
                rest.m.from = value;
                break;
            case 'a':
                range.a.to = value-1;
                rest.a.from = value;
                break;
            case 's':
                range.s.to = value-1;
                rest.s.from = value;
                break;
        }
    }
};

class MoreRule : public Rule
{
private:
    char parameter;
    int value;
    string label;

public:
    MoreRule(char parameter, int value, string label)
        : parameter(parameter), value(value), label(label) { };
    
    bool Matches(Part& part)
    {
        int v = GetValue(part);
        return v > value;
    }

    string MatchLabel()
    {
        return label;
    }

    void Process(PartRange& range, PartRange& rest)
    {
        rest = range;
        Truncate(range, rest);
        range.Normalize();
        rest.Normalize();
        range.Label = label;
    }

private:
    int GetValue(Part& part)
    {
        switch (parameter)
        {
            case 'x': return part.x;
            case 'm': return part.m;
            case 'a': return part.a;
            case 's': return part.s;
            default: return 0;
        }
    }

    void Truncate(PartRange& range, PartRange& rest)
    {
        switch (parameter)
        {
            case 'x':
                range.x.from = value+1;
                rest.x.to = value;
                break;
            case 'm':
                range.m.from = value+1;
                rest.m.to = value;
                break;
            case 'a':
                range.a.from = value+1;
                rest.a.to = value;
                break;
            case 's':
                range.s.from = value+1;
                rest.s.to = value;
                break;
        }
    }
};

class UltimateRule : public Rule
{
private:
    string label;

public:
    UltimateRule(string label) : label(label) { };
    
    bool Matches(Part& part)
    {
        return true;
    }

    string MatchLabel()
    {
        return label;
    }

    void Process(PartRange& range, PartRange& rest)
    {
        rest = range;
        range.Label = label;
        rest.x.from = 1;
        rest.x.to = 0;
    }
};

class Workflow
{
private:
    string label;
    vector<Rule*> rules;

public:
    Workflow() : label("") { }

    Workflow(string label) : label(label) { }

    void AddRule(Rule* rule)
    {
        rules.push_back(rule);
    }

    void ClearRules()
    {
        while(!rules.empty())
        {
            delete rules.back();
            rules.pop_back();
        }
    }

    string Process(Part& part)
    {
        for(auto r : rules)
        {
            if(r->Matches(part))
            {
                return r->MatchLabel();
            }
        }

        return "?";
    }

    void Process(PartRange& range, stack<PartRange>& buffer)
    {
        PartRange match = range;
        PartRange rest;

        for(auto r : rules)
        {
            if(match.Count() == 0LL) { break; }

            r->Process(match, rest);

            if(match.Count() > 0LL)
            {
                buffer.push(match);
            }
            match = rest;
        }
    }

    string& GetLabel()
    {
        return label;
    }
};

class WorkFlowParser
{
private:
    Workflow workflow;

public:
    WorkFlowParser() : workflow("") {}

    bool Parse(char* line)
    {
        if(strlen(line)<= 1 || line[0] == '{') { return false; }

        string label = ReadLabel(line);
        workflow = Workflow(label);

        char* pointer = strchr(line, '{');
        char* token = strtok(pointer+1, ",");

        while (token != NULL)
        {
            ParseRule(token);
            token = strtok(NULL, ",");
        }

        return true;
    }

    Workflow GetWorkflow()
    {
        return workflow;
    }

private:
    string ReadLabel(char* pointer)
    {
        string label;
        while(isalpha(*pointer))
        {
            label.push_back(*pointer);
            ++pointer;
        }
        return label;
    }

    void ParseRule(char* pointer)
    {
        if(pointer[1] == '<') { ParseLessRule(pointer); }
        else if(pointer[1] == '>') { ParseMoreRule(pointer); }
        else { ParseUltimateRule(pointer); }
    }

    void ParseLessRule(char* pointer)
    {
        char parameter = pointer[0];
        int value = atoi(pointer + 2);
        string label = ReadLabel(strchr(pointer, ':')+1);

        workflow.AddRule(new LessRule(parameter, value, label));
    }

    void ParseMoreRule(char* pointer)
    {
        char parameter = pointer[0];
        int value = atoi(pointer + 2);
        string label = ReadLabel(strchr(pointer, ':')+1);

        workflow.AddRule(new MoreRule(parameter, value, label));
    }

    void ParseUltimateRule(char* pointer)
    {
        string label = ReadLabel(pointer);
        workflow.AddRule(new UltimateRule(label));
    }
};

int main()
{
    int64_t sum = 0;
    char buffer[1024];

    map<string, Workflow> workflows;
    WorkFlowParser parser;
    vector<Part> parts;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        bool suceess = parser.Parse(buffer);

        if(suceess)
        {
            workflows[parser.GetWorkflow().GetLabel()] = parser.GetWorkflow();
        }
        else
        {
            break;
        }
    }

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        if(strlen(buffer) <= 1) { continue; }
        parts.push_back(Part(buffer));
    }

    for(auto p : parts)
    {
        string label = "in";

        while(workflows.find(label) != workflows.end())
        {
            label = workflows[label].Process(p);
        }

        if(label.compare("A") == 0)
        {
            sum += p.Sum();
        }
    }

    printf("%lld\n", sum);

    PartRange range;
    stack<PartRange> stack;
    stack.push(range);
    int64_t count = 0LL;

    while(!stack.empty())
    {
        PartRange range = stack.top();
        stack.pop();

        if(range.Label.compare("A") == 0)
        {
            count += range.Count();
            continue;
        }

        if(workflows.find(range.Label) != workflows.end())
        {
            workflows[range.Label].Process(range, stack);
        }
    }

    printf("%lld\n", count);

    for(auto [l,w] : workflows)
    {
        w.ClearRules();
    }

    return 0;
}