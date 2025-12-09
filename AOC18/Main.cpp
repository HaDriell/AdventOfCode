
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Point2D
{
    uint64_t x, y;
};

struct Rectangle
{
    uint64_t x, y, width, height;

    uint64_t GetArea() const
    {
        return width * height;
    }

    static Rectangle CreateFromPoints(Point2D const& a, Point2D const& b)
    {
        uint64_t minX = std::min(a.x, b.x);
        uint64_t maxX = std::max(a.x, b.x);
        uint64_t minY = std::min(a.y, b.y);
        uint64_t maxY = std::max(a.y, b.y);
        uint64_t width = maxX - minX + 1; // a rectangle {0, 0}, {0, 1} should have a width of 1
        uint64_t height = maxY - minY + 1; // a rectangle {0, 0}, {1, 0} should have a height of 1
        return { minX, minY, width, height };
    }
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

Rectangle GetBiggestRectangleInPoints(std::vector<Point2D> const& points)
{
    Rectangle bestRectangle;
    uint64_t bestArea = std::numeric_limits<uint64_t>::min();

    for (size_t first = 0; first < points.size(); first++)
    {
        for (size_t second = first + 1; second < points.size(); second++)
        {
            Point2D const& l = points.at(first);
            Point2D const& r = points.at(second);
            Rectangle currentRectangle = Rectangle::CreateFromPoints(l, r);
            uint64_t currentArea = currentRectangle.GetArea();
            if (currentArea > bestArea)
            {
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
    Rectangle biggestRectangle = GetBiggestRectangleInPoints(points);

    std::cout << "Biggest Area : " << biggestRectangle.GetArea() << "\n";

    return 0;
}