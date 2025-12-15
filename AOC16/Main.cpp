
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <numeric>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

struct Position3D
{
    int64_t x, y, z;

    uint64_t GetLengthSquared() const
    {
        return x * x + y * y + z * z;
    }
};

std::istream& operator>>(std::istream& stream, Position3D& position)
{
    stream >> position.x;
    assert(stream.get() == ',');
    stream >> position.y;
    assert(stream.get() == ',');    
    stream >> position.z;

    return stream;
}

std::ostream& operator<<(std::ostream& stream, Position3D const& position)
{
    return stream << "Position(" << position.x << ", " << position.y << ", " << position.z << ")";
}

bool operator==(Position3D const& l, Position3D const& r) { return std::tie(l.x, l.y, l.z) == std::tie(r.x, r.y, r.z); };
bool operator!=(Position3D const& l, Position3D const& r) { return std::tie(l.x, l.y, l.z) != std::tie(r.x, r.y, r.z); };
bool operator<(Position3D const& l, Position3D const& r) { return std::tie(l.x, l.y, l.z) < std::tie(r.x, r.y, r.z); };
bool operator>(Position3D const& l, Position3D const& r) { return std::tie(l.x, l.y, l.z) > std::tie(r.x, r.y, r.z); };

Position3D operator+(Position3D const& l, Position3D const& r) { return {l.x + r.x, l.y + r.y, l.z + r.z }; }
Position3D operator-(Position3D const& l, Position3D const& r) { return {l.x - r.x, l.y - r.y, l.z - r.z }; }

struct PositionPairData
{
    size_t First;
    size_t Second;
    uint64_t DistanceSquared;
};

std::vector<Position3D> ReadInput(std::string const& filename)
{
    std::vector<Position3D> result;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream lstream(line);
        Position3D position;
        lstream >> position;
        result.push_back(position);
    }

    return result;
}

std::vector<PositionPairData> ComputeAllPairs(std::vector<Position3D> const& positions)
{
    std::vector<PositionPairData> result;

    for (size_t first = 0; first < positions.size(); first++)
    {
        for (size_t second = first + 1; second < positions.size(); second++)
        {
            Position3D firstPosition = positions.at(first);
            Position3D secondPosition = positions.at(second);

            PositionPairData& ppd = result.emplace_back(); 
            ppd.First = first;
            ppd.Second = second;
            ppd.DistanceSquared = (firstPosition - secondPosition).GetLengthSquared();
        }
    }

    std::sort(result.begin(), result.end(), [](PositionPairData const& l, PositionPairData const& r)
    {
        return l.DistanceSquared < r.DistanceSquared;
    });

    return result;
}

void ProcessPairIntoChains(std::vector<std::set<size_t>>& chains, size_t first, size_t second)
{
        auto const& fIt = std::find_if(chains.begin(), chains.end(), [first] (std::set<size_t> const& set)
        {
            return set.find(first) != set.end();
        });

        auto const& sIt = std::find_if(chains.begin(), chains.end(), [second] (std::set<size_t> const& set)
        {
            return set.find(second) != set.end();
        });
        
        if (fIt != chains.end() && sIt != chains.end())
        {
            if (fIt != sIt) // Only if Distinct (otherwise nothing to do)
            {
                // Merge into fIt
                std::for_each(sIt->begin(), sIt->end(), [&fIt] (size_t element) 
                {
                    fIt->insert(element);
                });
                chains.erase(sIt);
            }
        }
        else if (fIt != chains.end() && sIt == chains.end())
        {
            fIt->insert(second);
        }
        else if (fIt == chains.end() && sIt != chains.end())
        {
            sIt->insert(first);
        }
        else if (fIt == chains.end() && sIt == chains.end())
        {
            auto& chain = chains.emplace_back();
            chain.insert(first);
            chain.insert(second);
        }
}

std::vector<std::set<size_t>> ConnectClosestNPairs(std::vector<PositionPairData> const& pairs, size_t count)
{
    std::vector<std::set<size_t>> result;

    for (size_t index = 0; index < count; index++)
    {
        auto const& pair = pairs.at(index);
        ProcessPairIntoChains(result, pair.First, pair.Second);
    }

    std::sort(result.begin(), result.end(), [](std::set<size_t> const& l, std::set<size_t> const& r)
    {
        return l.size() > r.size();
    });

    return result;
}

void UnitTestThisShit()
{
    std::cout << "Unit tests...";

    auto positions = ReadInput("SmallInput.txt");
    auto pairs = ComputeAllPairs(positions);

    {   // First Connection was explicit
        auto chains = ConnectClosestNPairs(pairs, 1);
        assert(chains.size() == 1);
        auto const& chain = chains.at(0);
        assert(chain.find(0) != chain.end());
        assert(chain.find(19) != chain.end());
    }
    
    {   // Second Connection was explicit
        auto chains = ConnectClosestNPairs(pairs, 2);
        assert(chains.size() == 1);
        auto chain = chains.at(0);
        assert(chain.find(0) != chain.end());
        assert(chain.find(19) != chain.end());
        assert(chain.find(7) != chain.end());
    }
    
    {   // Third Connection was explicit
        auto chains = ConnectClosestNPairs(pairs, 3);
        assert(chains.size() == 2);

        auto const& chain = chains.at(0);
        assert(chain.find(0) != chain.end());
        assert(chain.find(19) != chain.end());
        assert(chain.find(7) != chain.end());

        auto const& chain2 = chains.at(1);
        assert(chain2.find(2) != chain2.end());
        assert(chain2.find(13) != chain2.end());
    }
    
    {   // Final Test
        auto chains = ConnectClosestNPairs(pairs, 10);
        assert(chains.size() == 4); // We ignore single junctionbox circuits in the Connection, 11 circuits - 7 "singles"
        assert(chains.at(0).size() == 5);
        assert(chains.at(1).size() == 4);
        assert(chains.at(2).size() == 2);
        assert(chains.at(3).size() == 2);
    }

    std::cout << "Passed!\n";
}

int main(int argc, char** argv)
{
    std::cout << "Analyzing Junction Boxes !!!\n\n";
    UnitTestThisShit();

    constexpr size_t K_CONNECTIONS_COUNT = 1000;
    constexpr size_t K_CIRCUITS_COUNT = 3;


    auto positions = ReadInput("input.txt");
    std::cout << "There are " << positions.size() << " distinct Positions\n";
    auto pairs = ComputeAllPairs(positions);
    std::cout << "There are " << pairs.size() << " distinct Pairs\n";
    auto chains = ConnectClosestNPairs(pairs, K_CONNECTIONS_COUNT);

    size_t total = std::accumulate(chains.begin(), chains.begin() + K_CIRCUITS_COUNT, 1, [](size_t const&  acc, auto const& chain)
    {
        return acc * chain.size();
    });
    std::cout << "Total : " << total << "\n"; 

    return 0;
}