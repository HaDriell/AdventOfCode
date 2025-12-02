
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
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

bool IsRepeating(std::string_view string, size_t stride)
{
    // Stride cannot repeat a whole number of times
    if (string.size() % stride) return false;

    std::string_view pattern = string.substr(0, stride);
    for (size_t offset = stride; offset + stride < string.size(); offset += stride)
    {
        std::string_view part = string.substr(offset, stride);
        if (pattern != part)
        {
            return false;
        }
    }

    return true;
}

bool IsInvalid(uint64_t id)
{
    std::string string = std::to_string(id);

    for (size_t stride = 1; stride <= string.size(); stride++)
    {
        if (IsRepeating(string, stride))
        {
            return true;
        }
    }

    return false;
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