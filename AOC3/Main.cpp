
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct IDRange
{
    uint64_t Min;
    uint64_t Max;
};

std::vector<IDRange> ReadInput(std::string const& filename)
{
    std::vector<IDRange> result;

    std::ifstream fstream(filename);
    std::string rangeString;
    while (std::getline(fstream, rangeString, ','))
    {
        std::istringstream rStream(rangeString);
        IDRange& range = result.emplace_back();

        rStream >> range.Min;
        assert(rStream.get() == '-');
        rStream >> range.Max;
    }

    return result;
}

bool IsInvalid(uint64_t id)
{
    std::string string = std::to_string(id);
    // string has odd number of digits
    if (string.size() & 0x1) return false;

    size_t left = 0;
    size_t right = string.size() / 2;
    for (; right < string.size(); right++, left++)
    {
        char leftChar = string.at(left);
        char rightChar = string.at(right);
        if (leftChar != rightChar)
        {
            return false;
        }
    }

    return true;
}

int main(int argc, char** argv)
{
    std::cout << "Begin Checking Invalid IDs\n\n";
    std::vector<IDRange> ranges = ReadInput("input.txt");
    uint64_t invalidIDsum = 0;

    for (IDRange const& range : ranges)
    {
        std::cout << "Range(" << range.Min << ", " << range.Max << ")\n";

        for (uint64_t id = range.Min; id <= range.Max; id++)
        {
            if (IsInvalid(id))
            {
                std::cout << "\tInvalid ID " << id << " found\n"; 
                invalidIDsum += id;
            }
        }
    }
    std::cout << "Answer : " << invalidIDsum << "\n";
    return 0;
}