
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

class MathProblem
{
public:
    void AddVariable(uint64_t variable)
    {
        m_Variables.push_back(variable);
    }

    void SetOperator(char character)
    {
        m_Operator = character;
    }

    uint64_t GetResult() const
    {
        uint64_t result = m_Variables.at(0);
        for (uint64_t index = 1; index < m_Variables.size(); index++)
        {
            uint64_t value = m_Variables.at(index);
            switch (m_Operator)
            {
                case '+': result += value; break;
                case '*': result *= value; break;
                default: break;
            }
        }
        return result;
    }

    void DebugDraw() const
    {
        bool first = true;
        for (uint64_t value : m_Variables)
        {
            if (first) first = false;
            else std::cout << " " << m_Operator << " ";
            std::cout << value;
        }

        std::cout << " = " << GetResult() << "\n";
    }

private:
    std::vector<uint64_t> m_Variables;
    char m_Operator = 0;
};

class OctoMatrix
{
public:
    OctoMatrix(std::string const& data, size_t width, size_t height)
        : m_Data(data)
        , m_Width(width)
        , m_Height(height)
    {
    }

    uint64_t GetWidth() const { return m_Width; }
    uint64_t GetHeight() const { return m_Height; }

    char ReadChar(size_t column, size_t line) const
    {
        return m_Data.at(column + line * m_Width);
    }

    std::string ReadLine(size_t line) const
    {
        size_t start = 0 + line * m_Width; // x + y * width
        return m_Data.substr(start, m_Width);
    }

    std::string ReadVertical(size_t column, size_t start, size_t end) const
    {
        std::string output;

        for (size_t line = start; line <= end; line++)
        {
            output += ReadChar(column, line);
        }

        return output;
    }

private:
    std::string m_Data;
    size_t m_Width;
    size_t m_Height;
};

std::vector<MathProblem> ReadInput(std::string const& filename)
{
    std::ifstream stream(filename);

    size_t height = 0;
    size_t width = 0;
    std::string data;

    std::string line;
    while (std::getline(stream, line))
    {
        data += line;
        width = line.size();
        height++;
    }

    std::vector<MathProblem> problems;

    OctoMatrix matrix(data, width, height);
    std::vector<std::string> variables;
    for(size_t column = matrix.GetWidth() - 1;; column--)
    {
        std::string vline = matrix.ReadVertical(column, 0, matrix.GetHeight() - 2); // dodge last line
        char operand = matrix.ReadChar(column, matrix.GetHeight() - 1);
        
        variables.push_back(vline);
        if (operand != ' ')
        {
            MathProblem& problem = problems.emplace_back();
            problem.SetOperator(operand);
            for (std::string const& variable : variables)
            {
                problem.AddVariable(std::stoull(variable));
            }

            // Clear variables
            if (column == 0)
            {
                break; // End of Parse
            }
            else
            {
                // Next Problem to Parse
                variables.clear();
                column--; // We want to skip the next empty column
            }
        }
    }

    return problems;
}

int main(int argc, char** argv)
{
    std::cout << "Doing Homework\n\n";
    std::vector<MathProblem> problems = ReadInput("input.txt");

    uint64_t total = 0;
    for (MathProblem const& problem : problems)
    {
        total += problem.GetResult();
        problem.DebugDraw();
    }

    std::cout << "Total : " << total << "\n";

    return 0;
}