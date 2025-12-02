
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Dial
{
public:
    enum class Direction 
    {
        Left,
        Right
    };

    void Rotate(std::string const& input)
    {
        char direction;
        uint64_t count;
        std::istringstream stream(input);
        stream >> direction;
        stream >> count;
        
        if (direction == 'L') Rotate(Direction::Left, count);
        if (direction == 'R') Rotate(Direction::Right, count);
    }

    void Rotate(Direction direction, uint64_t count)
    {
        count %= 100;
        if (direction == Direction::Left)
        {
            m_Index = m_Index + 100 - count;
            m_Index %= 100;
        }
        else 
        {
            m_Index += count;
            m_Index %= 100;
        }

        std::cout  << "Rotate " << (direction == Direction::Left ? "Left " : "Right") << count << ". New Position : "<< m_Index << "\n";
    }

    uint64_t GetIndex() const { return m_Index; }

private:
    uint64_t m_Index = 50;
};

int main(int argc, char** argv)
{
    std::cout << "Begining Pasword cracking\n\n";
    Dial dial;
    uint64_t count = 0;
    std::ifstream stream("input.txt");
    std::string line;
    while (std::getline(stream, line))
    {
        dial.Rotate(line);
        if (dial.GetIndex() == 0)
        {
            count++;
        }
    }

    std::cout << "Password : " << count << "\n";
    return 0;
}