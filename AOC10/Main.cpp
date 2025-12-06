
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

class Span
{
public:
    Span(uint64_t min, uint64_t max)
        : m_Min(min), m_Max(max)
    {
    }

    uint64_t GetMin() const { return m_Min; }
    uint64_t GetMax() const { return m_Max; }

    bool IsFresh(uint64_t id) const
    {
        return id >= m_Min && id <= m_Max;
    }

    uint64_t GetOverlapSpanSize(Span const& other) const
    {
        uint64_t min = std::max(m_Min, other.m_Min);
        uint64_t max = std::min(m_Max, other.m_Max);
        return (min <= max) ? (max - min) + 1 : 0;
    }

    uint64_t GetSpanSize() const
    {
        // if m_Min == m_Max we still have 1 available value
        return 1 + (m_Max - m_Min);
    }

private:
    uint64_t m_Min;
    uint64_t m_Max;
};

std::optional<Span> Merge(Span const& left, Span const& right)
{
    if (left.GetOverlapSpanSize(right) == 0) return std::nullopt;
    uint64_t min = std::min(left.GetMin(), right.GetMin());
    uint64_t max = std::max(left.GetMax(), right.GetMax());
    return Span(min, max);
}

bool HasAnyOverlaps(std::vector<Span> const& spans)
{
    for (size_t left = 0; left < spans.size(); left++)
    {
        for (size_t right = 0; right < spans.size(); right++)
        {
            if (left == right) continue;

            Span const& lspan = spans.at(left);
            Span const& rspan = spans.at(right);

            if (lspan.GetOverlapSpanSize(rspan) > 0) return true;
        }
    }

    return false;
}

void MergeAll(std::vector<Span>& spans)
{
    while (HasAnyOverlaps(spans))
    {
        std::vector<Span> queue = spans;
        spans.clear();

        for (Span const& current : queue)
        {
            bool currentWasMerged = false;
            for (Span& mergeable : spans)
            {
                auto result = Merge(mergeable, current);
                if (result.has_value())
                {
                    mergeable = result.value();
                    currentWasMerged = true;
                    break;
                }
            }

            if (!currentWasMerged)
            {
                spans.push_back(current);
            }
        }
    }
}

class Database
{
public:
    void AddSpan(uint64_t min, uint64_t max)
    {
        m_Spans.emplace_back(min, max);
    }

    bool IsFresh(uint64_t id) const
    {
        for (auto const& range : m_Spans)
        {
            if (range.IsFresh(id))
            {
                return true;
            }
        }

        return false;
    }

    uint64_t ComputeFreshIDCount() const
    {

        std::vector<Span> copy = m_Spans;
        MergeAll(copy);
        uint64_t count = 0;
        for (Span const& span : copy)
        {
            count += span.GetSpanSize();
        }

        return count;
    }

    Span GetBoundingSpan() const
    {
        uint64_t min = std::numeric_limits<uint64_t>::max();
        uint64_t max = std::numeric_limits<uint64_t>::min();
        for (Span const& span : m_Spans)
        {
            min = std::min(min, span.GetMin());
            max = std::max(max, span.GetMax());
        }

        return Span(min, max);
    }

private:
    std::vector<Span> m_Spans;
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
        db.AddSpan(min, max);
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

    Database db;
    std::vector<uint64_t> ids;
    ReadInput("input.txt", db, ids);

    std::cout << "Theoretical Max Span : " << db.GetBoundingSpan().GetSpanSize() << "\n";
    std::cout << "Total Fresh Possible : " << db.ComputeFreshIDCount() << "\n";

    return 0;
}