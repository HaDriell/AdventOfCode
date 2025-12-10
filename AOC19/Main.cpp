
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <float.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Point2D
{
    int64_t x, y;

    static int64_t Cross(Point2D const& a, Point2D const& b)
    {
        return a.x * b.y - a.y * b.x;
    }

    static int64_t Dot(Point2D const& a, Point2D const& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    static Point2D RightNormal(Point2D const& v)
    {
        return { v.y, -v.x };
    }

    static int64_t Orientation(Point2D const& a, Point2D const& b, Point2D const& c)
    {
        Point2D ab = { b.x - a.x, b.y - a.y };
        Point2D ac = { c.x - a.x, c.y - a.y };
        return Cross(ab, ac);
    }

    static Point2D ZERO;
    static Point2D X;
    static Point2D Y;
};

Point2D Point2D::ZERO{0, 0};
Point2D Point2D::X{1, 0};
Point2D Point2D::Y{0, 1};

Point2D operator+(Point2D const& l, Point2D const& r) { return {l.x + r.x, l.y + r.y }; }
Point2D operator-(Point2D const& l, Point2D const& r) { return {l.x - r.x, l.y - r.y }; }
Point2D operator*(Point2D const& l, Point2D const& r) { return {l.x * r.x, l.y * r.y }; }
Point2D operator/(Point2D const& l, Point2D const& r) { return {l.x / r.x, l.y / r.y }; }

bool operator==(Point2D const& l, Point2D const& r) { return l.x == r.x && l.y == r.y; }
bool operator!=(Point2D const& l, Point2D const& r) { return !(l == r); }


bool Intersect(Point2D const& a, Point2D const& b, Point2D const& c, Point2D const& d)
{
    int64_t oa = Point2D::Orientation(c,d,a);
    int64_t ob = Point2D::Orientation(c,d,b);
    int64_t oc = Point2D::Orientation(a,b,c);
    int64_t od = Point2D::Orientation(a,b,d);      

    // Proper intersection exists iff opposite signs  
    return (oa * ob < 0 && oc * od < 0);
}

bool IsInside(std::vector<Point2D> const& shape, Point2D const& p)
{
    Point2D ORIGIN{ 0, 0 };
    size_t intersections = 0;
    for (size_t i = 1; i < shape.size(); i++)
    {
        Point2D const& a = shape.at(i - 1);
        Point2D const& b = shape.at(i);

        if (a == p || b == p)
        {
            return true; // We're on the line <=> we're inside
        }

        if (Intersect(a, b, p, ORIGIN))
        {
            intersections++;
        }
    }

    // shape is a cyclic array of segments
    if (Intersect(shape.back(), shape.front(), p, ORIGIN))
    {
        intersections++;
    }

    return (intersections % 2) > 0;
}

struct Rectangle
{
    Point2D A;
    Point2D B;

    uint64_t GetWidth() const
    {
        int64_t min = std::min(A.x, B.x);
        int64_t max = std::max(A.x, B.x);
        return 1 + (max - min);
    }

    uint64_t GetHeight() const
    {
        int64_t min = std::min(A.y, B.y);
        int64_t max = std::max(A.y, B.y);
        return 1 + (max - min);
    }

    uint64_t GetArea() const
    {
        return GetWidth() * GetHeight();
    }

    Point2D GetTL() const { return {std::min(A.x, B.x), std::min(A.y, B.y)}; }
    Point2D GetTR() const { return {std::max(A.x, B.x), std::min(A.y, B.y)}; }
    Point2D GetBL() const { return {std::min(A.x, B.x), std::max(A.y, B.y)}; }
    Point2D GetBR() const { return {std::max(A.x, B.x), std::max(A.y, B.y)}; }
};

class Matrix
{
public:
    Matrix(Rectangle const& region)
        : m_Data(region.GetArea(), '.')
        , m_Region(region)
        , m_Origin(region.GetTL())
    {
    }

    uint64_t GetWidth() const { return m_Region.GetWidth(); }
    uint64_t GetHeight() const { return m_Region.GetHeight(); }

    char At(Point2D const& position) const
    {
        Point2D coord = position - m_Origin;
        uint64_t width = m_Region.GetWidth();
        return m_Data.at(coord.x + coord.y * width);
    }

    void Set(Point2D const& position, char c)
    {
        Point2D coord = position - m_Origin;
        uint64_t width = m_Region.GetWidth();
        m_Data.at(coord.x + coord.y * width) = c;
    }

    bool FindAnyInRectangle(Rectangle const& rect, char c) const
    {
        Point2D begin = rect.GetTL() - m_Origin;
        Point2D end = rect.GetBR() - m_Origin;
        uint64_t width = m_Region.GetWidth();

        Point2D current = begin;
        while (true)
        {
            if (m_Data.at(current.x + current.y * width) == c)
            {
                return true;
            }

            // 2D Region search
            current.x++;
            if (current.x == end.x)
            {
                current.x = begin.x;
                current.y++;
            }

            if (current.y > end.y)
            {
                break;
            }
        }

        return false;
    }

    void DrawShape(std::vector<Point2D> const& shape, char c)
    {
        Point2D begin = m_Region.GetTL() - m_Origin;
        Point2D end = m_Region.GetBR() - m_Origin;
        uint64_t width = m_Region.GetWidth();

        Point2D current = begin;
        while (true)
        {
            if (IsInside(shape, current + m_Origin))
            {
                m_Data.at(current.x + current.y * width) = c;
            }
        
            // 2D Region search
            current.x++;
            if (current.x == end.x)
            {
                current.x = begin.x;
                current.y++;
            }

            if (current.y > end.y)
            {
                break;
            }
        }
    }

    void DebugDraw() const
    {
        Point2D end = m_Region.GetBR();
        size_t width = m_Region.GetWidth();
        for (int64_t y = 0; y <= end.y; y++)
        {
            for (int64_t x = 0; x <= end.x; x++)
            {
                Point2D current = Point2D{ x, y } - m_Origin;

                if (current.x < 0 || current.y < 0)
                {
                    std::cout << ' ';
                }
                else
                {
                    std::cout << m_Data.at(current.x + current.y * width);
                }
            }
            std::cout << "\n";
        }
    }

private:
    std::string m_Data;
    Rectangle m_Region;
    Point2D m_Origin;
};

std::vector<Point2D> ReadInput(std::string const& filename)
{
    std::vector<Point2D> result;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line)) 
    { 
        std::istringstream lstream(line);
        Point2D& p = result.emplace_back();
        lstream >> p.x;
        assert(lstream.get() == ',');
        lstream >> p.y;
    }

    return result;
}

Matrix CreateMatrix(std::vector<Point2D> const& shape)
{
    std::cout << "Creating Matrix\n";

    Point2D min = { std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max() };
    Point2D max = { std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min() };

    for (Point2D const& p : shape)
    {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
    }

    Rectangle region{min, max};
    
    Matrix matrix(region);

    std::cout << "Drawing shape\n";

    matrix.DrawShape(shape, '#');

    return matrix;
}

Rectangle GetBiggestRectangleInShape(std::vector<Point2D> const& shape, Matrix const& matrix)
{
    Rectangle bestRectangle;
    uint64_t bestArea = std::numeric_limits<uint64_t>::min();

    for (size_t first = 0; first < shape.size(); first++)
    {
        for (size_t second = first + 1; second < shape.size(); second++)
        {
            Point2D const& l = shape.at(first);
            Point2D const& r = shape.at(second);
            Rectangle currentRectangle{l, r};

            // if (!IsInside(points, currentRectangle.GetTL())) continue;
            // if (!IsInside(points, currentRectangle.GetTR())) continue;
            // if (!IsInside(points, currentRectangle.GetBL())) continue;
            // if (!IsInside(points, currentRectangle.GetBR())) continue;

            uint64_t currentArea = currentRectangle.GetArea();
            if (bestArea < currentArea)
            {
                if (matrix.FindAnyInRectangle(currentRectangle, '.')) continue;

                bestRectangle = currentRectangle;
                bestArea = currentArea;
            }
        }
    }

    return bestRectangle;
}


int main(int argc, char** argv)
{
    std::cout << "Looking closely at Red Tiles\n\n";

    auto points = ReadInput("input.txt");
    auto matrix = CreateMatrix(points);

    std::cout << "Debug Drawing the Matrix :\n";
    matrix.DebugDraw();

    std::cout << "Finding biggest Rectangle in shape\n";
    Rectangle rectangle = GetBiggestRectangleInShape(points, matrix);

    std::cout << "Biggest Area : " << rectangle.GetArea() << "\n";

    return 0;
}