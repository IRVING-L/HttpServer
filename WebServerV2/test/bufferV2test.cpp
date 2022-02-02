#include "../lib/bufferV2.h"
#include <iostream>
#include <string>

int main(int argc, char const *argv[])
{
    Buffer buf;
    std::cout << buf._size() << std::endl;
    buf.append("this is my house");
    std::cout << buf._size() << std::endl;
    std::cout << buf._all2str() << std::endl;
    char str[10] = {"sssss"};
    buf.append(str, sizeof(str));
    std::cout << buf._size() << std::endl;
    std::cout << buf._all2str() << std::endl;
    return 0;
}
