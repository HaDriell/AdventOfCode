
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

struct ButtonData
{
    std::vector<uint64_t> Bindings;
};

struct MachineData
{
    std::vector<bool> Lights;
    std::vector<ButtonData> Buttons;
    std::vector<uint64_t> Joltages;


    void Parse(std::istream& stream)
    {
        Lights.clear();
        Buttons.clear();
        Joltages.clear();

        while (!stream.eof())
        {
            switch (stream.get())
            {
                case '[': ParseLightData(stream); break;
                case '(': ParseButton(stream); break;
                case '{': ParseJoltage(stream); break;

                default: break;
            }
        }
    }

    void ParseLightData(std::istream& stream)
    {
        std::string data;
        std::getline(stream, data, ']');

        Lights.reserve(data.size());
        for (char c : data)
        {
            Lights.push_back(c == '#');
        }
    }

    void ParseButton(std::istream& stream)
    {
        std::string data;
        std::getline(stream, data, ')');

        ButtonData& newButton = Buttons.emplace_back();

        std::istringstream substream(data);
        while (!substream.eof())
        {
            uint64_t binding;
            substream >> binding;
            newButton.Bindings.push_back(binding);

            if (!substream.eof())
            {
                assert(substream.get() == ',');
            }
        }
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

std::ostream& operator<<(std::ostream& stream, MachineData const& machine)
{
    stream << '[';
    for (bool light : machine.Lights) stream << (light ? '#' : '.');
    stream << "] ";

    for (auto const& button : machine.Buttons)
    {
        stream << '(';
        bool first = true;
        for (uint64_t binding : button.Bindings)
        {
            if (first) first = false;
            else stream << ',';
            stream << binding;
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

std::vector<MachineData> ReadInput(std::string const& filename)
{
    std::vector<MachineData> result;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        MachineData& machine = result.emplace_back();
        std::istringstream lstream(line);
        machine.Parse(lstream);
    }

    return result;
}

int main(int argc, char** argv)
{
    auto machines = ReadInput("input.txt");
    for (auto const& machine : machines)
    {
        std::cout << machine;
    }
    return 0;
}