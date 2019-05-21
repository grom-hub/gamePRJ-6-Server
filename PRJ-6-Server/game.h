#ifndef GAME_H
#define GAME_H


//#pragma once

#include <vector>
#include <string>
//#include "server.h"


const int SEND_BUFF_WORK_SIZE = 100000;


struct CreateData
{
        char skin;
        std::string name;
};

struct PrintData
{
        char skin;
        int id;
        int x;
        int y;
};

struct PrintStatusData
{
        int pwr;
};

struct UnitBox
{
        int id;
        char skin;
        int x;
        int y;
        int pwr;
        int time;
        std::string name;
};

struct PwrPointBox
{
        std::string skin;
        int pwr;
        int time;
        int x;
        int y;
};


class Game
{
public:
        Game();
        void recvData(char *recvBuff, int clientid);
        void sendData(char *sendBuff, int &sendSize);
        void deletePlayer(int clientid);
        void unitPwrDecrement();
        void pointPwrIncrement();




private:
        char *recvBuffPtr;
    char *sendPreBuffPtr;
    int clientidBuff;
        int unitsFrameNum;
        int pwrPointsFrameNum;
        int printObjectsSize[3];
    CreateData createData;
        std::vector<PrintData> printObjects;
        PrintData printObject;
        std::vector<UnitBox> units;
        UnitBox unit;
        std::vector<PwrPointBox> pwrPoints;
        PrintStatusData printStatus;



        void createPlayer(int &sendSize);
        void movePlayer();

        void sendUnits(int &sendSize);
        void sendPwrPoints(int &sendSize);
        void sendStatus(int &sendSize);
        void sendZero(int &sendSize);

        void checkPointCollision(int unitid);
        bool checkObstacle(int checkindex);
        void takePWR();

};

#endif // GAME_H
