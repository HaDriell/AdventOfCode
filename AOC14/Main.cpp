
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <tuple>
#include <utility>
#include <vector>

struct Point2D
{
    uint64_t x;
    uint64_t y;

    Point2D(uint64_t x, uint64_t y)
        : x(x)
        , y(y)
    {
    }

    Point2D(Point2D const& copy) = default;
    Point2D& operator=(Point2D const& copy) = default;

    bool operator==(Point2D const& other) const
    {
        return x == other.y
            && x == other.y;
    }

    bool operator!=(Point2D const& other) const
    {
        return !(*this == other);
    }

    bool operator<(Point2D const& other) const
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    Point2D operator+(Point2D const& other) const { return Point2D(x + other.x, y + other.y); }
    Point2D operator-(Point2D const& other) const { return Point2D(x - other.x, y - other.y); }

    static Point2D X;
    static Point2D Y;
};

Point2D Point2D::X(1, 0);
Point2D Point2D::Y(0, 1);

class Ray
{
public:
    Ray(Point2D origin, Point2D end, bool split)
        : m_Origin(origin)
        , m_End(end)
        , m_Split(split)
    {
        assert(m_Origin.x == m_End.x); // Don't support non-vertical stuff
    }

    Point2D const& GetOrigin() const { return m_Origin; }
    Point2D const& GetEnd() const { return m_End; }

    bool Contains(Point2D const& point) const
    {
        return point.x == m_Origin.x
            && point.y >= m_Origin.y 
            && point.y <= m_End.y;
    }

    bool Intersects(Ray const& ray) const
    {
        if (m_Origin.x != ray.m_Origin.x) return false;
        uint64_t origin = std::max(m_Origin.y, ray.m_Origin.y);
        uint64_t end = std::min(m_End.y, ray.m_End.y);

        return end >= origin;
    }

    bool IsSplit() const 
    {
        return m_Split;
    }

    operator bool() const 
    {
        return m_Split;
    }

private:
    Point2D m_Origin;
    Point2D m_End;
    bool m_Split = false;
};

class Matrix
{
public:
    Matrix(std::string const& data, uint64_t  width, uint64_t height)
        : m_Data(data)
        , m_Width(width)
        , m_Height(height)
    {
    }

    uint64_t GetWidth() const { return m_Width; }
    uint64_t GetHeight() const { return m_Height; }

    char At(uint64_t x, uint64_t y) const
    {
        return m_Data.at(x + y * m_Width);
    }

    bool Find(char c, uint64_t& px, uint64_t& py) const
    {
        for (uint64_t y = 0; y < m_Height; y++)
        {
            for (uint64_t x = 0; x < m_Width; x++)
            {
                if (c == At(x, y))
                {
                    px = x;
                    py = y;
                    return true;
                }
            }
        }

        return false;
    }

private:
    std::string m_Data;
    uint64_t  m_Width;
    uint64_t m_Height;
};

Ray RayCast(Matrix const& matrix, Point2D const& origin)
{
    for (uint64_t dy = origin.y;  dy < matrix.GetHeight(); dy++)
    {
        if (matrix.At(origin.x, dy) == '^')
        {
            return Ray(origin, Point2D(origin.x, dy), true);
        }
    }

    return Ray(origin, Point2D(origin.x, matrix.GetHeight()), false);
}

class SplitterTree
{
    struct Splitter
    {
        Point2D Origin;
        uint64_t Possibilities = 0;
        Splitter* Left;
        Splitter* Right;

        Splitter(Point2D const& origin)
            : Origin(origin)
        {
        }

        void ComputePossibilities()
        {
            if (Possibilities == 0)
            {
                if (Left)
                {
                    Left->ComputePossibilities();
                    Possibilities += Left->Possibilities;
                }
                else
                {
                    Possibilities += 1;
                }

                if (Right)
                {
                    Right->ComputePossibilities();
                    Possibilities += Right->Possibilities;
                }
                else
                {
                    Possibilities += 1;
                }
            }
        }
    };

public:
    SplitterTree(Matrix const& matrix, Point2D const& origin)
        : m_FirstSplitterPosition(std::numeric_limits<uint64_t>::max(),std::numeric_limits<uint64_t>::max())
    {
        std::vector<Point2D> positionQueue;

        if (Ray initialRay = RayCast(matrix, origin))
        {
            m_FirstSplitterPosition = initialRay.GetEnd();
            positionQueue.push_back(initialRay.GetEnd());
        } 

        for (size_t index = 0; index < positionQueue.size(); index++)
        {
            Point2D const& position = positionQueue.at(index);
            Splitter* current = GetOrCreateSplitter(position);

            if (current->Left == nullptr)
            {
                if (auto left = RayCast(matrix, current->Origin - Point2D::X))
                {
                    current->Left = GetOrCreateSplitter(left.GetEnd());

                    positionQueue.push_back(left.GetEnd());
                }
            }

            if (current->Right == nullptr)
            {
                if (auto right = RayCast(matrix,current->Origin + Point2D::X))
                {
                    current->Right = GetOrCreateSplitter(right.GetEnd());
                    positionQueue.push_back(right.GetEnd());
                }
            }
        }
    }

    uint64_t GetPossibilityCount() const
    {
        auto it = m_Splitters.find(m_FirstSplitterPosition);

        if (it == m_Splitters.end())
        {
            return 0;
        }

        Splitter* firstSplitter = it->second.get();

        firstSplitter->ComputePossibilities();
        return firstSplitter->Possibilities;
    }

    Splitter* GetOrCreateSplitter(Point2D const& point)
    {
        auto it = m_Splitters.find(point);

        if (it == m_Splitters.end())
        {
            auto ptr = std::make_unique<Splitter>(point);
            ptr->Left = nullptr;
            ptr->Right = nullptr;
            auto result = m_Splitters.insert_or_assign(point, std::move(ptr));
            return result.first->second.get();
        }

        return it->second.get();
    }

private:
    Point2D m_FirstSplitterPosition;
    std::map<Point2D, std::unique_ptr<Splitter>> m_Splitters;
};

Matrix ReadInput(std::string const& filename)
{
    uint64_t width = 0;
    uint64_t height = 0;
    std::string data;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        data += line;
        height++;
        width = line.size();
    }

    return Matrix(data, width, height);
}

int main(int argc, char** argv)
{
    std::cout << "Begin Tachyon Bombardment\n\n";

    Matrix matrix = ReadInput("input.txt");

    Point2D origin(0, 0);
    if (matrix.Find('S', origin.x, origin.y))
    {
        SplitterTree tree(matrix, origin);
        std::cout << "All Possibilities count : " << tree.GetPossibilityCount() << "\n";
    }
    
    return 0;
}