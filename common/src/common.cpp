// デバッグ用
#include "Thread.h"
#include "Socket.h"
#include <stdlib.h>
#include <iostream>


int main(void)
{
    Thread _Thread1;
    Thread _Thread2;
    Socket _Socket1;
    Socket _Socket2;

    std::cout << _Thread1.Equals(_Thread1) << "\n";
    std::cout << _Thread1.Equals(_Thread2) << "\n";
    std::cout << _Thread2.Equals(_Thread1) << "\n";
    std::cout << _Thread2.Equals(_Thread2) << "\n";
    std::cout << _Thread1.Equals(_Socket1) << "\n";
    std::cout << _Thread1.Equals(_Socket2) << "\n";
    std::cout << _Thread2.Equals(_Socket1) << "\n";
    std::cout << _Thread2.Equals(_Socket2) << "\n";
    std::cout << "\n";

    std::cout << _Socket1.Equals(_Socket1) << "\n";
    std::cout << _Socket1.Equals(_Socket2) << "\n";
    std::cout << _Socket2.Equals(_Socket1) << "\n";
    std::cout << _Socket2.Equals(_Socket2) << "\n";
    std::cout << _Socket1.Equals(_Thread1) << "\n";
    std::cout << _Socket1.Equals(_Thread2) << "\n";
    std::cout << _Socket2.Equals(_Thread1) << "\n";
    std::cout << _Socket2.Equals(_Thread2) << "\n";
    std::cout << "\n";

    // 正常終了
    exit(0);
}
