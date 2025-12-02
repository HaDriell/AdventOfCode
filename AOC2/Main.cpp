
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class DialThatZCounts
{
public:
    void Rotate(std::string const& input)
    {
        char direction;
        uint64_t count;
        std::istringstream stream(input);
        stream >> direction;
        stream >> count;
        
        if (direction == 'L')
        {
            while (count--) 
            {
                RotateLeft();
            }
        }
        else // direction == 'R'
        {
            while (count--) 
            {
                RotateRight();
            }
        }
    }

    void RotateLeft()
    {
        if (m_Index == 99)
        {
            m_Index = 0;
            m_ZCount++;
        }
        else
        {
            m_Index++;
        }
    }

    void RotateRight()
    {
        if (m_Index == 0)
        {
            m_Index = 99;
        }
        else
        {
            m_Index--;
            if (m_Index == 0) m_ZCount++;
        }
    }

    uint64_t GetZCount() const { return m_ZCount; }

private:
    uint64_t m_Index = 50;
    uint64_t m_ZCount = 0;
};

int main(int argc, char** argv)
{
    std::cout << "Begining Pasword cracking\n\n";
    DialThatZCounts dial;
    std::ifstream stream("input.txt");
    std::string line;
    while (std::getline(stream, line))
    {
        dial.Rotate(line);
    }

    std::cout << "Password : " << dial.GetZCount() << "\n";
    return 0;
}