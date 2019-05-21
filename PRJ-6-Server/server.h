#ifndef SERVER_H
#define SERVER_H


//#pragma once

#include <set>

#include "game.h"

const int SEND_BUFF_SIZE = 102400;
const int RECV_BUFF_SIZE = 1024;

class Server
{
public:
    void initServer();
    void closeConnection();
    void mainLoop(Game &gm);




private:
    int listener;
    struct sockaddr_in addr;
    std::set<int> clients;

    char recvBuff[RECV_BUFF_SIZE];
    char sendPreBuff[SEND_BUFF_SIZE];
    char sendBuff[SEND_BUFF_SIZE];

    int bytes_read;

    int bytesSend;
    int totalBytesSend;
    int targetSendSize;
    int sendSize;

    int clientid;


};

#endif // SERVER_H
