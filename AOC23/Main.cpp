
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stack>
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

bool Contains(std::vector<Device const*> const& path, Device const* device)
{
    return std::find(path.begin(), path.end(), device) != path.end();
}

void ExploreAllPathsUntil(std::vector<std::vector<Device const*>>& paths, Device const* end)
{
    while (true)
    {
        // Find any explorable path
        auto it = std::find_if(paths.begin(), paths.end(), [end] (auto const& path) { return path.back() != end; });
        if (it == paths.end()) break; // All Paths found

        std::vector<Device const*> nexts;
        it->back()->GetOutputs(nexts);
        // Remove cycles
        auto _ = std::remove_if(nexts.begin(), nexts.end(), [it] (Device const* next) { return Contains(*it, next); });

        if (nexts.empty())
        {
            // No remaining nexts => this path is a dead-end
            paths.erase(it);
        }
        else
        {
            std::vector<Device const*> copy = *it;
            //small optimization => reuse the current path for the first iteration
            it->push_back(nexts.at(0));
            // Duplicate path for each Next
            std::for_each(nexts.begin() + 1, nexts.end(), [&copy, &paths] (Device const* next)
            {
                auto& nextPath = paths.emplace_back(copy);
                nextPath.push_back(next);
            });
        }
    }
}

std::vector<std::vector<Device const*>> GetAllPaths(Device const* begin, Device const* end)
{
    std::vector<std::vector<Device const*>> paths;
    paths.push_back({ begin });
    ExploreAllPathsUntil(paths, end);
    return paths;
}

bool IsConnected(Device const* begin, Device const* end)
{
    assert(begin != end);
    std::set<Device const*> visited;
    std::vector<Device const*> queue;
    queue.push_back(begin);

    while (true)
    {
        auto current = queue.back();
        queue.pop_back();
        visited.insert(current);

        for (auto output : current->GetOutputs())
        {
            if (output == end) return true;
            if (visited.find(output) != visited.end()) continue;
            queue.push_back(output);
        }
    }

    return false;
}

void CollectAllDevicesBetween(Device const* begin, Device const* end, std::set<Device const*>& devices)
{
    devices.insert(begin);
    devices.insert(end);
    for (auto output : begin->GetOutputs())
    {
        if (devices.find(output) != devices.end()) continue; // already found
        if (!IsConnected(output, end)) continue; // not connected

        devices.insert(output);
        CollectAllDevicesBetween(output, end, devices);
    }
}

size_t CountAllPaths(Device const* begin, Device const* end, std::set<Device const*> const& whitelist)
{
    assert(begin != end);

    size_t count = 0;
    for (auto output : begin->GetOutputs())
    {
        if (whitelist.find(output) == whitelist.end()) continue; // skip
        if (output == end)
        {
            count++;
        }
        else
        {
            count += CountAllPaths(output, end, whitelist);
        }
    }

    return count;
}

int main(int argc, char** argv)
{
    std::cout << "Graphing Devices\n\n";

    auto db = ReadInput("input.txt");
    Device const* svr = db.Find("svr");
    Device const* out = db.Find("out");
    Device const* fft = db.Find("fft");
    Device const* dac = db.Find("dac");

    std::set<Device const*> devices, visited;
    CollectAllDevicesBetween(svr, out, devices);
    std::cout << "There are " << devices.size() << " devices between svr and out\n";

    std::cout << "Finding all paths\n";    
    visited.clear();
    std::cout << "There are " << CountAllPaths(db.Find("you"), db.Find("out"), devices, visited) << " paths\n";
    visited.clear();
    std::cout << "There are " << CountAllPaths(svr, out, devices, visited) << " paths\n";
    
    // size_t total = 0;
    // total += CountPaths(svr, fft) * CountPaths(fft, dac) * CountPaths(dac, out);
    // total += CountPaths(svr, dac) * CountPaths(dac, fft) * CountPaths(fft, out);
    // std::cout << "Total Paths svr -> out passing by fft & dac in any order " << total << "\n";

    // total = 0;
    // auto svr_to_fft = GetPathsBetween(svr, fft);
    // auto fft_to_dac = GetPathsBetween(fft, dac);
    // auto dac_to_out = GetPathsBetween(dac, out);
    // total += svr_to_fft.size() * fft_to_dac.size() * dac_to_out.size();

    // auto svr_to_dac = GetPathsBetween(svr,dac);
    // auto dac_to_fft = GetPathsBetween(dac, fft);
    // auto fft_to_out = GetPathsBetween(fft,out);
    // total += svr_to_dac.size() * dac_to_fft.size() * fft_to_out.size();

    // std::cout << "Total Paths svr -> out passing by fft & dac in any order " << total << "\n";

    return 0;
}