
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

class FreshIDRange
{
public:
    FreshIDRange(uint64_t min, uint64_t max) : m_Min(min), m_Max(max)
    {
    }

    bool IsFresh(uint64_t id) const
    {
        return id >= m_Min && id <= m_Max;
    }

private:
    uint64_t m_Min;
    uint64_t m_Max;
};

class Database
{
public:
    void AddFreshIDRange(uint64_t min, uint64_t max)
    {
        m_FreshIDRanges.emplace_back(min, max);
    }

    bool IsFresh(uint64_t id) const
    {
        for (auto const& range : m_FreshIDRanges)
        {
            if (range.IsFresh(id))
            {
                return true;
            }
        }

        return false;
    }

private:
    std::vector<FreshIDRange> m_FreshIDRanges;
};

void ReadInput(std::string const& filename, Database& db, std::vector<uint64_t>& ids)
{
    std::ifstream stream(filename);
    std::string line;

    // Load Database
    while (std::getline(stream, line))
    {
        if (line.empty()) break;

        std::istringstream lstream(line);
        uint64_t min, max;
        lstream >> min;
        assert(lstream.get() == '-');
        lstream >> max;
        db.AddFreshIDRange(min, max);
    }

    // Load Available IDs
    while (std::getline(stream, line))
    {
        ids.push_back(std::stoull(line));
    }
}

int main(int argc, char** argv)
{
    std::cout << "Begin Inventory\n\n";
    std::ifstream stream("input.txt");

    Database db;
    std::vector<uint64_t> ids;
    ReadInput("input.txt", db, ids);

    uint64_t freshCount = 0;
    for (uint64_t id : ids)
    {
        bool fresh = db.IsFresh(id);
        std::cout << "ID " << id << " : " << (fresh ? "Fresh" : "Spoiled") << "\n";

        if (fresh)
        {
            freshCount++;
        }
    }

    std::cout << "Fresh Count : " << freshCount << "\n";

    return 0;
}