#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cstdint>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<algorithm>

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

enum SignalType
{
    Low = 0,
    High = 1
};

struct Signal
{
    string Input;
    SignalType Type;
    string Output;

    Signal(const char* input, SignalType type, const char* output) : Input(input), Type(type), Output(output) {}
    Signal(string& input, SignalType type, string& output) : Input(input), Type(type), Output(output) {}
};

class Module
{
protected:
    string name;
    vector<string> outputs;

public:
    Module(string& name) : name(name) {}
    virtual ~Module() {}

    virtual void ProcessSignal(Signal& signal, queue<Signal>& step) = 0;

    void AddOutput(string& output) { outputs.push_back(output); }
    virtual void AddInput(const string& input) { }
    string& GetName() { return name; }
    vector<string>& GetOutputs() { return outputs; }
    virtual int CountInputs() { return 0; }
};

class Broadcaster : public Module
{
public:
    Broadcaster(string& name) : Module(name) { }

    void ProcessSignal(Signal& signal, queue<Signal>& step)
    {
        for(auto o : outputs)
        {
            step.push(Signal(name, signal.Type, o));
        }
    }
};

class FlipFlop : public Module
{
private:
    bool isOn;

public:
    FlipFlop(string& name) : Module(name)
    {
        isOn = false;
    }

    void ProcessSignal(Signal& signal, queue<Signal>& step)
    {
        if(signal.Type == Low)
        {
            isOn = !isOn;
            SignalType next = isOn ? High : Low;

            for(auto o : outputs)
            {
                step.push(Signal(name, next, o));
            }
        }
    }
};

class Conjunction : public Module
{
private:
    map<string, SignalType> inputs;

public:
    Conjunction(string& name) : Module(name) { }

    void ProcessSignal(Signal& signal, queue<Signal>& step)
    {
        UpdateInput(signal);

        SignalType next = AllHighInputs() ? Low : High;

        for(auto o : outputs)
        {
            step.push(Signal(name, next, o));
        }
    }

    void AddInput(const string& input)
    {
        inputs[input] = Low;
    }

    int CountInputs() { return (int)inputs.size(); }

private:
    bool AllHighInputs()
    {
        for(auto const& [key, value] : inputs)
        {
            if(value == Low) { return false; }
        }

        return true;
    }

    void UpdateInput(Signal& signal)
    {
        inputs[signal.Input] = signal.Type;
    }
};

class ModuleConfiguration
{
private:
    map<string, Module*> modules;
    int64_t lowCounter;
    int64_t highCounter;
    int64_t pressCounter;
    map<string, int64_t> inputMonitor;
    string monitoredModule;

public:
    ModuleConfiguration()
    {
        lowCounter = highCounter = 0LL;
        pressCounter = 0LL;
        monitoredModule = "";
    }

    ~ModuleConfiguration()
    {
        for(auto [key, value] : modules)
        {
            delete value;
        }
    }

    void AddModule(Module* module)
    {
        modules[module->GetName()] = module;
    }

    void UpdateModuleInputs()
    {
        for(auto& [name, module] : modules)
        {
            for(auto& outputName : module->GetOutputs())
            {
                auto it = modules.find(outputName);
                if(it != modules.end())
                {
                    it->second->AddInput(name);
                }
            }
        }
    }

    void SetMonitoredModule(const char* name)
    {
        monitoredModule = string(name);
        inputMonitor.clear();
    }

    void ProcessButtonPressed()
    {
        pressCounter++;

        queue<Signal> q1, q2;
        queue<Signal>* current = &q1;
        queue<Signal>* next = &q2;

        next->push(Signal("start", Low, "broadcaster"));

        while(!next->empty())
        {
            swap(current, next);

            while(!current->empty())
            {
                Signal& signal = current->front();

                if(signal.Type == Low) { lowCounter++; }
                else if(signal.Type == High) { highCounter++; }

                auto it = modules.find(signal.Output);
                if(it != modules.end())
                {
                    it->second->ProcessSignal(signal, *next);
                }

                if(signal.Type == High && signal.Output.compare(monitoredModule) == 0)
                {
                    if(inputMonitor.find(signal.Input) == inputMonitor.end())
                    {
                        inputMonitor[signal.Input] = pressCounter;
                    }
                }

                current->pop();
            }
        }
    }

    void ClearCounters() { lowCounter = highCounter = pressCounter = 0LL; }

    int64_t GetLowCounter() { return lowCounter; }

    int64_t GetHighCounter() { return highCounter; }

    bool IsAllInputsMonitored()
    {
        if(modules.find(monitoredModule) != modules.end())
        {
            int count = modules[monitoredModule]->CountInputs();
            int monitored = (int)inputMonitor.size();
            return count == monitored;
        }

        return false;
    }

    int64_t GetMinimumSteps()
    {
        int64_t result = 1LL;

        for(auto& [key, value] : inputMonitor)
        {
            result = LCM(result, value);
        }

        return result;
    }
};

class ConfigurationParser
{
public:
    ConfigurationParser(){}

    Module* ParseLine(char* line)
    {
        if(strlen(line) <= 1) { return NULL; }

        Module* result;

        if(line[0] == '%')
        {
            string name = ReadLabel(line+1);
            result = new FlipFlop(name);
        }
        else if(line[0] == '&')
        {
            string name = ReadLabel(line+1);
            result = new Conjunction(name);
        }
        else
        {
            string name = ReadLabel(line);
            result = new Broadcaster(name);
        }

        char* pointer = strchr(line, '>');
        char* token = strtok(pointer+1, ",");

        while (token != NULL)
        {
            string name = ReadLabel(token);
            result->AddOutput(name);
            token = strtok(NULL, ",");
        }

        return result;
    }

private:
    string ReadLabel(char* pointer)
    {
        string label;
        while(*pointer == ' ') { ++pointer; }
        while(isalpha(*pointer))
        {
            label.push_back(*pointer);
            ++pointer;
        }
        return label;
    }
};

int main()
{
    char buffer[1024];
    ConfigurationParser parser;
    ModuleConfiguration configuration;

    while(fgets(buffer, 1024, stdin) != NULL)
    {
        Module* module = parser.ParseLine(buffer);

        if(module != NULL)
        {
            configuration.AddModule(module);
        }
    }

    configuration.UpdateModuleInputs();
    configuration.ClearCounters();
    configuration.SetMonitoredModule("gf");

    for(int i = 1; i <= 1000; i++)
    {
        configuration.ProcessButtonPressed();
    }

    printf("%lld\n", configuration.GetLowCounter() * configuration.GetHighCounter());

    while(!configuration.IsAllInputsMonitored())
    {
        configuration.ProcessButtonPressed();
    }

    printf("%lld\n", configuration.GetMinimumSteps());

    return 0;
}