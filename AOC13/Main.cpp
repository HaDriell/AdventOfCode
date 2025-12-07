
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
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
};

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

bool AddNoIntersect(std::vector<Ray>& rays, Ray const& ray)
{
    for (Ray const& other : rays)
    {
        if (other.Intersects(ray))
        {
            return false;
        }
    }

    rays.push_back(ray);
    return true;
}

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

void DebugDraw(Matrix const& matrix, std::vector<Ray> const& rays)
{
    std::cout << "Debug\n";
    for (uint64_t y = 0; y < matrix.GetHeight(); y++)
    {
        for (uint64_t x = 0; x < matrix.GetWidth(); x++)
        {
            char c  = matrix.At(x, y);
            if (c == '.')
            {
                Point2D p(x, y);
                for (Ray const& ray : rays)
                {
                    if (ray.Contains(p))
                    {
                        c = '|';
                        break;
                    }
                }
            }

            std::cout << c;
        }
        std::cout << '\n';
    }
}

int main(int argc, char** argv)
{
    std::cout << "Begin Tachyon Bombardment\n\n";

    Matrix matrix = ReadInput("input.txt");

    Point2D origin(0, 0);
    if (matrix.Find('S', origin.x, origin.y))
    {
        uint64_t splitCount = 0;
        std::vector<Ray> rays;
        rays.push_back(RayCast(matrix, origin));

        for (size_t index = 0; index < rays.size(); index++)
        {
            Ray ray = rays.at(index);

            if (ray.IsSplit())
            {
                splitCount++;

                //Split Left
                Point2D leftOrigin = ray.GetEnd();
                leftOrigin.x -= 1;
                Ray leftRay = RayCast(matrix, leftOrigin);
                AddNoIntersect(rays, leftRay);

                //Split Right
                Point2D rightOrigin = ray.GetEnd();
                rightOrigin.x += 1;
                Ray rightRay = RayCast(matrix, rightOrigin);
                AddNoIntersect(rays, rightRay);
            }
        }

        std::cout << " Split count : " << splitCount << "\n";

        DebugDraw(matrix, rays);
    }
    
    return 0;
}