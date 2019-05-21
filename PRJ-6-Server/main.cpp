// #include <sys/time.h>
// #include <unistd.h> // close()
// #include <fcntl.h>
// #include <algorithm> // max_element()
// #include <set>
//#include <cstring> // std::memcpy()
//#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "game.h"
#include "server.h"



void inputHeandler(Server &srv)
{
    char a;
    while(true)
    {
        a = getchar();
        if(a == 'q')
        {
            srv.closeConnection();
            return;
        }
    }
}


void timeFlow(Game &gm)
{
    while(true)
    {
        usleep(100000);
        gm.unitPwrDecrement();
        gm.pointPwrIncrement();
    }
}



int main()
{
    Game gm;
    Server srv;

    std::thread thr1(inputHeandler, std::ref(srv));
    std::thread thr2(timeFlow, std::ref(gm));

    srv.initServer();

    srv.mainLoop(gm);

    std::cout << "End?" << std::endl;

    if (thr1.joinable())
        thr1.join();
    if (thr2.joinable())
        thr2.join(); // Выполнение возвращается функции main когда поток заканчивается
    // func_thread.detach(); В этом случае поток заканчивается принудительно

    return 0;
}
