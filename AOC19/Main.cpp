
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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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

struct Rectangle
{
    Point2D A;
    Point2D B;

    static Rectangle GetBoundingRectangle(std::vector<Point2D> const& points)
    {
        Point2D min{ std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max() };
        Point2D max{ std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min() };

        for (Point2D const& p : points)
        {
            min.x = std::min(min.x, p.x);
            min.y = std::min(min.y, p.y);
            max.x = std::max(max.x, p.x);
            max.y = std::max(max.y, p.y);
        }

        return Rectangle{ min, max };
    }

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

    int64_t GetTop() const { return std::min(A.y, B.y); }
    int64_t GetLeft() const { return std::min(A.x, B.x); }
    int64_t GetBottom() const { return std::max(A.y, B.y); }
    int64_t GetRight() const { return std::max(A.x, B.x); }

    Point2D GetTL() const { return {std::min(A.x, B.x), std::min(A.y, B.y)}; }
    Point2D GetTR() const { return {std::max(A.x, B.x), std::min(A.y, B.y)}; }
    Point2D GetBL() const { return {std::min(A.x, B.x), std::max(A.y, B.y)}; }
    Point2D GetBR() const { return {std::max(A.x, B.x), std::max(A.y, B.y)}; }
};

bool Intersect1D(int64_t a, int64_t b, int64_t value)
{
    return (a < b) ? (value >= a && value <= b) : value >= b && value <= a;
}

bool IntersectVertical(Point2D const& a, Point2D const& b, Point2D const& value)
{
    return Intersect1D(a.y, b.y, value.y);
}

bool IntersectHorizontal(Point2D const& a, Point2D const& b, Point2D const& value)
{
    return Intersect1D(a.x, b.x, value.x);
}


bool IntersectAxisAlignedSegment(Point2D const& start, Point2D const& end, Point2D const& point)
{
    if (start.y == end.y) // Horizontal Line
    {
        if (point.y < start.y) return false;

        int64_t minX = std::min(start.x, end.x);
        int64_t maxX = std::max(start.x, end.x);

        return point.x >= minX && point.x <= maxX;

    }
    else if (start.x == end.x) // Vertical Line
    {
        if (point.x < start.x) return false;

        int64_t minY = std::min(start.y, end.y);
        int64_t maxY = std::max(start.y, end.y);

        return point.y >= minY && point.y <= maxY;
    }

    assert(start == end);
    return start == point;
}

class AAShape
{
public:
    AAShape(std::vector<Point2D> const& vertices)
        : m_Vertices(vertices)
    {
        // Make sure data is valid (all vertical or horizontal lines)
        for (size_t index = 0; index < vertices.size(); index++)
        {
            size_t next = (index + 1) % vertices.size();
            Point2D const& first = vertices.at(index);
            Point2D const& second = vertices.at(next);

            assert(first.x == second.x || first.y == second.y);
        }
    }

    std::vector<Point2D> const& GetVertices() const { return m_Vertices; }

    bool IsOnOutline(Point2D const& point) const
    {
        for (size_t index = 0; index < m_Vertices.size(); index++)
        {
            size_t next = (index + 1) % m_Vertices.size();
            Point2D const& first = m_Vertices.at(index);
            Point2D const& second = m_Vertices.at(next);
            
            bool horizontal = first.x == second.x && first.x == point.x;
            bool vIntersect = Intersect1D(first.y, second.y, point.y);
            if (horizontal && vIntersect) return true;
            
            bool vertical = first.y == second.y && first.y == point.y;
            bool hIntersect = Intersect1D(first.x, second.x, point.x);
            if (vertical && hIntersect) return true;
        }

        return false;
    }

    bool Contains(Point2D const& point) const
    {
#if 1
        //Half-line Method https://web.cs.ucdavis.edu/~okreylos/TAship/Spring2000/PointInPolygon.html
        size_t intersectCount = 0;
        for (size_t index = 0; index < m_Vertices.size(); index++)
        {
            size_t next = (index + 1) % m_Vertices.size();
            Point2D const& first = m_Vertices.at(index);
            Point2D const& second = m_Vertices.at(next);

            // Edge is strictly is above Ray
            if (point.y > first.y && point.y > second.y) continue; 
            // Edge is strictly below Ray
            if (point.y <= first.y && point.y <= second.y)
            {
                //Check if point is on Edge (early exit)
                if (point.y == first.y && point.y == second.y && Intersect1D(first.x, second.x, point.x))
                {
                    return true;
                }
                continue;
            }

            // Edge is strictly at the left of the Ray
            if (point.x > first.x && point.x > second.x) continue;
            // Edge is Vertical on the same column with point
            if (point.x == first.x && point.x == second.x)
            {
                // Check if point is on Edge (early exit)
                if (Intersect1D(first.y, second.y, point.y))
                {
                    return true;
                }
                continue;
            }
            
            intersectCount++;
        }
        return intersectCount & 1;
#else
        // AABBPolygon collision https://stackoverflow.com/questions/47853987/how-to-determine-if-a-point-is-within-an-polygon-consist-of-horizontal-and-verti
        size_t left = 0;
        size_t right = 0;
        size_t top = 0;
        size_t bottom = 0;

        for (size_t index = 0; index < m_Vertices.size(); index++)
        {
            size_t next = (index + 1) % m_Vertices.size();
            Point2D const& first = m_Vertices.at(index);
            Point2D const& second = m_Vertices.at(next);

            bool horizontal = first.x == second.x;
            bool vIntersect = Intersect1D(first.y, second.y, point.y);

            if (horizontal && vIntersect)
            {
                if (point.x == first.x) return true;

                if (first.x < point.x) 
                    left++;
                else
                    right++;
                continue;
            }

            bool vertical = first.y == second.y;
            bool hIntersect = Intersect1D(first.x, second.x, point.x);

            if (vertical && hIntersect)
            {
                if (point.y == first.y) return true;

                if (first.y < point.y)
                    top++;
                else
                    bottom++;
                continue;
            }
        }

        if (left && right) return left & 1 || right & 1;
        if (top && bottom) return top & 1 || bottom & 1;
        return false;
#endif
    }

    bool Contains(Rectangle const& rectangle) const
    {
        int64_t top = rectangle.GetTop();
        int64_t left = rectangle.GetLeft();
        int64_t bottom = rectangle.GetBottom();
        int64_t right = rectangle.GetRight();

        // Check Rectangle Bounds Horizontally
        for (int64_t x = left; x <= right; x++)
        {
            if (!Contains(Point2D{x, top})) return false;
            if (!Contains(Point2D{x, bottom})) return false;
        }

        for (int64_t y = top; y <= bottom; y++)
        {
            if (!Contains(Point2D{left, y})) return false;
            if (!Contains(Point2D{right, y})) return false;
        }

        return true;
    }

    void DebugDraw(Rectangle const& region, size_t margin = 10) const
    {
        enum Color { Black, Green, Red };
        int64_t left = region.GetLeft() - margin;
        int64_t right = region.GetRight() + margin;
        int64_t top = region.GetTop() - margin;
        int64_t bottom = region.GetBottom() + margin;

        int width = right - left;
        int height = bottom - top;

        std::vector<uint8_t> data(width * height * 3, 0x0);

        Point2D current = Point2D::ZERO;
        for (size_t py = 0; py < height; py++)
        {
            current.y = py + top;
            for (size_t px = 0; px < width; px++)
            {
                current.x = px + left;

                Color color = Black;
                if (Contains(current))
                {
                    color = IsOnOutline(current) ? Red : Green;
                }

                size_t pixelIndex = 3 * (px + py * width);

                switch (color)
                {
                    case Black:
                        data.at(pixelIndex + 0) = 0x0;
                        data.at(pixelIndex + 1) = 0x0;
                        data.at(pixelIndex + 2) = 0x0;
                    break;
                    case Red:
                        data.at(pixelIndex + 0) = 0xA0;
                        data.at(pixelIndex + 1) = 0x0;
                        data.at(pixelIndex + 2) = 0x0;
                    break;
                    case Green:
                        data.at(pixelIndex + 0) = 0x0;
                        data.at(pixelIndex + 1) = 0xA0;
                        data.at(pixelIndex + 2) = 0x0;
                    break;
                }

                // Checker board pattern
                if ((py & 1) ^ (px & 1))
                {
                    data.at(pixelIndex + 0) += 0x30;
                    data.at(pixelIndex + 1) += 0x30;
                    data.at(pixelIndex + 2) += 0x30;
                }
            }
        }

        stbi_write_bmp("debug_draw.bmp", width, height, 3, data.data());
    }

private:
    std::vector<Point2D> m_Vertices;
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

std::vector<Rectangle> CreateCandidateQueue(std::vector<Point2D> const& points)
{
    std::vector<Rectangle> result;

    for (size_t first = 0; first < points.size(); first++)
    {
        for (size_t second = first + 1; second < points.size(); second++)
        {
            Point2D const& l = points.at(first);
            Point2D const& r = points.at(second);

            result.emplace_back(Rectangle{ l ,r });
        }
    }

    std::sort(result.begin(), result.end(), [](Rectangle const& l, Rectangle const& r)
    {
        return l.GetArea() > r.GetArea();
    });

    return result;
}

Rectangle GetBiggestRectangleInShape(AAShape const& shape)
{
    std::vector<Point2D> const& vertices = shape.GetVertices();
    std::vector<Rectangle> candidates = CreateCandidateQueue(vertices);

    for (Rectangle const& rectangle : candidates)
    {
        if (shape.Contains(rectangle))
        {
            return rectangle;
        }
    }

    assert(false); // shouldn't ever happen
    return {};
}


int main(int argc, char** argv)
{
    std::cout << "Looking closely at Red Tiles\n\n";

    auto points = ReadInput("input.txt");
    AAShape shape(points);
    Rectangle bound = Rectangle::GetBoundingRectangle(points);

    std::cout << "Drawing Debug\n";

    constexpr size_t K_MAX_SIZE = 2048;

    int64_t debugWidth = std::min(bound.GetWidth(), K_MAX_SIZE) / 2;
    int64_t debugHeight = std::min(bound.GetHeight(), K_MAX_SIZE) / 2;
    Point2D size{ debugWidth, debugHeight };
    
    int64_t centerX = (bound.GetRight() - bound.GetLeft()) / 2;
    int64_t centerY = (bound.GetBottom() - bound.GetTop()) / 2;
    Point2D center{ centerX, centerY };
    Rectangle debugRegion = Rectangle{ center - size, center + size };

    // shape.DebugDraw(debugRegion, 2);

    std::cout << "Debugged Region :\n";
    std::cout << "Top : " << debugRegion.GetTop() << "\n";
    std::cout << "Left : " << debugRegion.GetLeft() << "\n";
    std::cout << "Bottom : " << debugRegion.GetBottom() << "\n";
    std::cout << "Right : " << debugRegion.GetRight() << "\n";

    std::cout << "Finding biggest Rectangle in shape\n";
    Rectangle rectangle = GetBiggestRectangleInShape(shape);

    std::cout << "Biggest Area : " << rectangle.GetArea() << "\n";

    return 0;
}