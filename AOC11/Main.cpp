
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

struct MathProblem
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

template<class T>
std::vector<T> ReadAll(std::istream& stream)
{
    std::vector<T> result;

    T value;
    while (stream >> value)
    {
        result.push_back(value);
    }

    return result;
}

std::vector<MathProblem> ReadInput(std::string const& filename)
{
    std::vector<MathProblem> problems;

    std::ifstream stream(filename);
    std::string line;
    while (std::getline(stream, line))
    {
        std::istringstream lstream(line);
        if (!stream.eof())
        {
            auto variables = ReadAll<uint64_t>(lstream);

            if (problems.empty()) problems.resize(variables.size());

            for (size_t index = 0; index < variables.size(); index++)
            {
                problems.at(index).AddVariable(variables.at(index));
            }
        }
        else
        {
            auto operators = ReadAll<char>(lstream);
            for (size_t index = 0; index < operators.size(); index++)
            {
                problems.at(index).SetOperator(operators.at(index));
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