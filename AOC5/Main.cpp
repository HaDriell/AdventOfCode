
#include <cassert>
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

uint64_t GetBestJoltableInBank(std::string_view bank)
{
    std::string result = std::string(bank.substr(0, 2));

    for (size_t index = 2; index < bank.size(); index++)
    {
        bool last = index + 1 == bank.size();

        if (result.at(0) < bank.at(index) && !last)
        {
            result.at(0) = bank.at(index);
            result.at(1) = bank.at(index + 1);
            continue;
        }

        if (result.at(1) < bank.at(index))
        {
            result.at(1) = bank.at(index);
        }
    }

    return std::stoull(result);
}

int main(int argc, char** argv)
{
    std::cout << "Begining Bank checking\n\n";

    uint64_t totalJoltage = 0;

    auto banks = ReadInput("input.txt");
    for (auto const& bank : banks)
    {
        auto joltage = GetBestJoltableInBank(bank);
        std::cout << "Best Joltable in " << bank << " : " << joltage << "\n"; 
        totalJoltage += joltage;
    }

    std::cout << "Total Joltage : " << totalJoltage << "\n";
    return 0;
}