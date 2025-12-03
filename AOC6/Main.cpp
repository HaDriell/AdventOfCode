
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

std::vector<std::string> ReadInput(std::string_view filename)
{
    std::vector<std::string> result;
    std::ifstream stream(filename.data());
    std::string line;

    while (std::getline(stream, line))
    {
        result.push_back(line);
    }

    return result;
}

size_t FindBestBatteryIndex(std::string_view bank)
{
    size_t best = 0;
    for (size_t current = 1; current < bank.size(); current++)
    {
        if (bank[best] < bank[current])
        {
            best = current;
        }
    }

    return best;
}

uint64_t GetBestJoltableInBank(std::string_view bank, size_t batteryCount)
{
    std::string selection;
    selection.reserve( batteryCount);

    size_t begin = 0;
    while (selection.size() < batteryCount)
    {
        // We need space to find other battery indices
        size_t reservedSpace = batteryCount - (selection.size() + 1);
        size_t end = bank.size() - reservedSpace;
        std::string_view searchArea = bank.substr(begin, end - begin);
        size_t bestBatteryIndex = FindBestBatteryIndex(searchArea);

        selection.push_back(searchArea.at(bestBatteryIndex));
        begin += bestBatteryIndex + 1;
    }

    return std::stoull(selection);
}

int main(int argc, char** argv)
{
    std::cout << "Begining Bank checking\n\n";

    uint64_t totalJoltage = 0;

    auto banks = ReadInput("input.txt");
    for (auto const& bank : banks)
    {
        std::cout << "Bank : " << bank << "\n";
        auto joltage = GetBestJoltableInBank(bank, 12);
        std::cout << "Best Batteries : " << joltage << "\n"; 
        totalJoltage += joltage;
    }

    std::cout << "Total Joltage : " << totalJoltage << "\n";
    return 0;
}