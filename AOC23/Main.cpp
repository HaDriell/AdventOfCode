
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Device
{
public:
    Device(std::string const& name)
        : m_Name(name)
    {
    }

    std::string const& GetName() const { return m_Name; }
    std::vector<Device const*> const& GetInputs() const { return m_Inputs; }
    std::vector<Device const*> const& GetOutputs() const { return m_Outputs; }

    void AddInput(Device const* device)
    {
        m_Inputs.push_back(device);
    }

    void AddOutput(Device const* device)
    {
        m_Outputs.push_back(device);
    }

    bool HasInput(Device const* device) const
    {
        for (Device const* input : m_Inputs)
        {
            if (device == input) return true;
        }
        return false;
    }
    
    bool HasOutput(Device const* device) const
    {
        for (Device const* output : m_Outputs)
        {
            if (device == output) return true;
        }
        return false;
    }

    void GetInputs(std::vector<Device const*> inputs) const
    {
        for (Device const* device : m_Inputs)
        {
            inputs.push_back(device);
        }
    }

    void GetOutputs(std::vector<Device const*>& outputs) const
    {
        for (Device const* device : m_Outputs)
        {
            outputs.push_back(device);
        }
    }

private:
    std::string m_Name;
    std::vector<Device const*> m_Inputs;
    std::vector<Device const*> m_Outputs;
};

class DeviceDatabase
{
public:
    Device* Find(std::string const& name)
    {
        auto it = std::find_if(m_Devices.begin(), m_Devices.end(), [&name](auto const& device) { return device->GetName() == name; });
        return it != m_Devices.end() ? it->get() : nullptr;
    }

    Device* New(std::string const& name)
    {
        return m_Devices.emplace_back(std::make_unique<Device>(name)).get();
    }
    
    Device* GetOrCreate(std::string const& name)
    {
        if (auto* device = Find(name))
        {
            return device;
        }

        return New(name);
    }

    std::vector<std::unique_ptr<Device>> const& GetDevices() const { return m_Devices; }
private:
    std::vector<std::unique_ptr<Device>> m_Devices;
};

DeviceDatabase ReadInput(std::string const& filename)
{
    DeviceDatabase db;
    
    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream lstream(line);
        std::string deviceName;

        std::getline(lstream, deviceName, ':');

        Device* device = db.GetOrCreate(deviceName);

        assert(lstream.get() == ' '); // skip first <whitespace>
        while (std::getline(lstream, deviceName, ' '))
        {
            Device* connectedDevice = db.GetOrCreate(deviceName);
            device->AddOutput(connectedDevice);
            connectedDevice->AddInput(device);
        }
    }

    return db;
}

size_t CountAllPathsRecursive(Device const* begin, Device const* end, std::map<Device const*, size_t>& cache)
{
    size_t count = 0;

    for (auto output : begin->GetOutputs())
    {
        if (cache.find(output) != cache.end())
            count += cache.at(output);
        else if (output == end)
            count++;
        else
            count += CountAllPathsRecursive(output, end, cache);        
    }

    cache.emplace(begin, count);
    return count;
}

size_t CountAllPathsRecursive(Device const* begin, Device const* end)
{
    std::map<Device const*, size_t> cache;
    return CountAllPathsRecursive(begin, end, cache);
}

int main(int argc, char** argv)
{
    std::cout << "Graphing Devices\n\n";

    auto db = ReadInput("input.txt");
    Device const* you = db.Find("you");
    Device const* svr = db.Find("svr");
    Device const* out = db.Find("out");
    Device const* fft = db.Find("fft");
    Device const* dac = db.Find("dac");

    std::cout << "Finding all paths\n";
    std::cout << "You -> Out = " << CountAllPathsRecursive(you, out) << " excpecting 670 from AOC22\n";

    size_t sd = CountAllPathsRecursive(svr, dac); std::cout << " sd = " << sd << "\n";
    size_t df = CountAllPathsRecursive(dac, fft); std::cout << " df = " << df << "\n";
    size_t fo = CountAllPathsRecursive(fft, out); std::cout << " fo = " << fo << "\n";

    size_t sf = CountAllPathsRecursive(svr, fft); std::cout << " sf = " << sf << "\n";
    size_t fd = CountAllPathsRecursive(fft, dac); std::cout << " fd = " << fd << "\n";
    size_t dO = CountAllPathsRecursive(dac, out); std::cout << " dO = " << dO << "\n";

    std::cout << " sdfo+sfdo = " << (sd * df * fo + sf * fd * dO) << "\n";
    return 0;
}