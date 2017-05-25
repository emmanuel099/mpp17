#include <cstdlib>
#include <iostream>
#include <thread>

int main(int argc, char** argv)
{
    std::thread helloWorldThread([] { std::cout << "hello world\n"; });
    helloWorldThread.join();

    return EXIT_SUCCESS;
}
