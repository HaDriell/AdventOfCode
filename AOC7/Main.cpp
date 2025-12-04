
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Extent2D
{
    uint64_t Width;
    uint64_t Height;
};

struct Matrix2D
{
    std::vector<char> Data;
    Extent2D Extent;

    char At(int x, int y) const
    {
        if (x < 0 || x >= Extent.Width) return '.';
        if (y < 0 || y >= Extent.Height) return  '.';

        return Data[x + y * Extent.Width];
    }
};

void ReadInput(std::string const& filename, Matrix2D& matrix)
{
    std::ifstream stream(filename);
    std::string line;

    matrix.Extent.Height = 0;
    matrix.Extent.Width = 0;
    matrix.Data.clear();
    while (std::getline(stream, line)) 
    {
        matrix.Extent.Width = line.size();
        matrix.Extent.Height++;
        for (char c : line)
        {
            matrix.Data.push_back(c);
        }
    }
    // Ensure invariant of size
    assert(matrix.Data.size() == matrix.Extent.Width * matrix.Extent.Height);
}

bool CanUseForklift(Matrix2D const& matrix, int x, int y)
{
    uint64_t obstacles = 0;
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            if (dx == 0 && dy == 0) continue;
            if (matrix.At(x + dx, y + dy) == '@') obstacles++;
        }
    }

    return obstacles < 4;
}

void DebugDrawMatrix(Matrix2D const& matrix)
{
    for (uint64_t x = 0; x < matrix.Extent.Width; x++)
    {
        for (uint64_t y = 0; y < matrix.Extent.Height; y++)
        {
            std::cout << matrix.At(x, y);
        }

        std::cout << '\n';
    }
}

int main(int argc, char** argv)
{
    std::cout << "Begin Program\n\n";

    Matrix2D matrix;
    ReadInput("input.txt", matrix);
    DebugDrawMatrix(matrix);
    uint64_t moveableCount = 0;
    for (uint64_t y = 0; y < matrix.Extent.Height; y++)
    {
        for (uint64_t x = 0; x < matrix.Extent.Width; x++)
        {
            if (matrix.At(x, y) == '.') continue;;
            if (CanUseForklift(matrix, x, y)) moveableCount++;
        }
    }

    std::cout << " Moveable count : " << moveableCount << "\n";

    return 0;
}