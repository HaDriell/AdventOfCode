
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Flags
{
    template<typename Integer>
    Integer Set(Integer data, size_t index)
    {
        static_assert(std::is_integral_v<Integer>, "Must be Integral");
        assert(index < sizeof(Integer) * 8);
        Integer mask = 0x1 << index;

        return data | mask;
    }

    template<typename Integer>
    Integer Clear(Integer data, size_t index)
    {
        static_assert(std::is_integral_v<Integer>, "Must be Integral");
        assert(index < sizeof(Integer) * 8);
        Integer mask = 0x1 << index;

        return data & !mask;
    }

    template<typename Integer>
    Integer Toggle(Integer data, size_t index)
    {
        static_assert(std::is_integral_v<Integer>, "Must be Integral");
        assert(index < sizeof(Integer) * 8);
        Integer mask = 0x1 << index;

        return data ^ mask;
    }

    template<typename Integer>
    bool Get(Integer data, size_t index)
    {
        static_assert(std::is_integral_v<Integer>, "Must be Integral");
        assert(index < sizeof(Integer) * 8);
        Integer mask = 0x1 << index;
        
        return data & mask;
    }
}

struct Machine
{
    uint64_t LightState;
    size_t LightCount;

    std::vector<uint64_t> Buttons;

    std::vector<uint64_t> Joltages;

    void Parse(std::istream& stream)
    {
        Buttons.clear();
        Joltages.clear();

        while (!stream.eof())
        {
            switch (stream.get())
            {
                case '[': ParseLights(stream); break;
                case '(': ParseButton(stream); break;
                case '{': ParseJoltage(stream); break;

                default: break;
            }
        }
    }

    void ParseLights(std::istream& stream)
    {
        std::string data;
        std::getline(stream, data, ']');

        LightCount = data.size();
        LightState = 0;
        for (size_t index = 0; index < data.size(); index++)
        {
            if (data.at(index) == '#')
            {
                LightState = Flags::Set(LightState, index);
            }
        }
    }

    void ParseButton(std::istream& stream)
    {
        std::string data;
        std::getline(stream, data, ')');

        uint64_t bindings = 0;

        std::istringstream substream(data);
        while (!substream.eof())
        {
            size_t index;
            substream >> index;
            bindings = Flags::Set(bindings, index);

            assert(substream.eof() || substream.get() == ',');
        }

        Buttons.push_back(bindings);
    }

    void ParseJoltage(std::istream& stream)
    {
        std::string data;
        std::getline(stream, data, '}');

        Joltages.reserve(data.size());

        std::istringstream substream(data);
        while (!substream.eof())
        {
            uint64_t joltage;
            substream >> joltage;
            Joltages.push_back(joltage);
            
            if (!substream.eof())
            {
                assert(substream.get() == ',');
            }
        }
    }
};

std::ostream& PrintLightState(std::ostream& stream, uint64_t state, size_t count)
{
    stream << '[';
    for (size_t index = 0; index < count; index++)
    {
        std::cout << (Flags::Get(state,count - index - 1) ? '#' : '.');
    }
    stream << "] ";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Machine const& machine)
{
    PrintLightState(stream, machine.LightState, machine.LightCount);
    for (auto const& bindings : machine.Buttons)
    {
        stream << '(';
        bool first = true;
        for (size_t index = 0; index < machine.LightCount; index++)
        {
            if (!Flags::Get(bindings, index)) continue;

            if (first) first = false;
            else stream << ',';
            stream << index;
        }
        stream << ") ";
    }

    stream << '{';
    {
        bool first = true;
        for (uint64_t joltage : machine.Joltages)
        {
            if (first) first = false;
            else stream << ',';
            stream << joltage;
        }
    }
    stream << '}';
    stream << '\n';

    return stream;
}

std::vector<Machine> ReadInput(std::string const& filename)
{
    std::vector<Machine> result;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        Machine& machine = result.emplace_back();
        std::istringstream lstream(line);
        machine.Parse(lstream);
    }

    return result;
}

size_t Solve(Machine const& machine)
{
    std::unordered_set<uint64_t> states;
    states.insert(0); // initial state

    size_t iterations = 0;
    while (states.find(machine.LightState) == states.end())
    {
        std::unordered_set<uint64_t> nexts;
        for (uint64_t state : states)
        {
            for (uint64_t bindings : machine.Buttons)
            {
                uint64_t next = state ^ bindings;
                nexts.insert(next);
            }
        }

        for (uint64_t next :nexts) states.insert(next);
        iterations++;
    }

    return iterations;
}

int main(int argc, char** argv)
{
    auto machines = ReadInput("input.txt");

    uint64_t total = 0;
    for (auto const& machine : machines)
    {
        std::cout << "Solving Machine : " << machine << " : ";
        size_t iterations = Solve(machine);
        std::cout << iterations << "\n";
        
        total += iterations;
    }

    std::cout << "Total : " << total << "\n";
    return 0;
}