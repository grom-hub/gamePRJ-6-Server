//#include <vector>
#include <cstring> // std::memcpy()
#include <iostream>
#include <algorithm> // std::fill()


#include "game.h"



Game::Game()
{

    unitsFrameNum = 1;
    pwrPointsFrameNum = 1;

    pwrPoints.resize(5);

    pwrPoints[0].x = 7;
    pwrPoints[0].y = 20;
    pwrPoints[0].pwr = 1;
    pwrPoints[0].time = 10;

    pwrPoints[1].x = 10;
    pwrPoints[1].y = 45;
    pwrPoints[1].pwr = 2;
    pwrPoints[1].time = 5;

    pwrPoints[2].x = 10;
    pwrPoints[2].y = 55;
    pwrPoints[2].pwr = 3;
    pwrPoints[2].time = 15;

    pwrPoints[3].x = 30;
    pwrPoints[3].y = 0;
    pwrPoints[3].pwr = 2;
    pwrPoints[3].time = 3;

    pwrPoints[4].x = -5;
    pwrPoints[4].y = 80;
    pwrPoints[4].pwr = 5;
    pwrPoints[4].time = 0;

}




void Game::recvData(char *recvBuff, int clientid)
{
    recvBuffPtr = recvBuff;
    clientidBuff = clientid;

    if(unitsFrameNum > 100) // цикл счетчика
        unitsFrameNum = 1;
    if(pwrPointsFrameNum > 100)
        pwrPointsFrameNum = 1;


    if (recvBuffPtr[0] == 3) // Стандартный режим
    {
        if(recvBuffPtr[2] != 0) // Обработка команды управления персонажем
            if(recvBuffPtr[2] == 5)
                takePWR();
            else
                movePlayer();
    }
}



void Game::sendData(char *sendPreBuff, int &sendSize)
{
    sendPreBuffPtr = sendPreBuff;

    if (recvBuffPtr[0] == 2) // создать игрока
        createPlayer(sendSize);


    if(recvBuffPtr[0] == 3)
    {
        if(unitsFrameNum == recvBuffPtr[3]
            && pwrPointsFrameNum == recvBuffPtr[4])
            sendZero(sendSize);
        else
        {
            std::fill(printObjectsSize, printObjectsSize + 3, 0); // Обнулить размеры векторов
            sendSize = sizeof(int) * 3 + 4;

            if(unitsFrameNum != recvBuffPtr[3])
            {
                sendUnits(sendSize);
            }
            if(pwrPointsFrameNum != recvBuffPtr[4])
            {
                sendPwrPoints(sendSize);
                sendStatus(sendSize);
            }
            sendPreBuffPtr[0] = 4; // тип пакета
            sendPreBuffPtr[1] = unitsFrameNum;
            sendPreBuffPtr[2] = pwrPointsFrameNum;
            sendPreBuffPtr[3] = 0; // резерв
            std::memcpy(&sendPreBuffPtr[4], &printObjectsSize, sizeof(int) * 3);
        }
    }
}



void Game::sendUnits(int &sendSize)
{
    printObjects.clear();
    printObjects.reserve(units.size());

    for(int i = 0; i < units.size(); ++i)
    {
        printObject.skin = units[i].skin;
        printObject.id = units[i].id;
        printObject.x = units[i].x;
        printObject.y = units[i].y;

        printObjects.push_back(printObject);
    }

    printObjectsSize[0] = printObjects.size();

    if(sendSize + printObjects.size() * sizeof(PrintData) > SEND_BUFF_WORK_SIZE)
        std::cout << "SendBuffer overload !\n";

    std::memcpy(&sendPreBuffPtr[sendSize], printObjects.data(), printObjects.size() * sizeof(PrintData));
    sendSize += printObjects.size() * sizeof(PrintData);
}



void Game::sendPwrPoints(int &sendSize)
{
    printObjects.clear();

    for(int i = 0; i < pwrPoints.size(); ++i)
    {
        pwrPoints[i].skin = "(" + std::to_string(pwrPoints[i].pwr) + ")";

        for(int j = 0; j < 3; ++j)
        {
            printObject.skin = pwrPoints[i].skin[j];
            printObject.id = 0; // без 0 будет конфлик с units.id
            printObject.x = pwrPoints[i].x;
            printObject.y = pwrPoints[i].y + j - 1;

            printObjects.push_back(printObject);
        }
    }

    printObjectsSize[1] = printObjects.size();

    if(sendSize + printObjects.size() * sizeof(PrintData) > SEND_BUFF_WORK_SIZE)
        std::cout << "SendBuffer overload !\n";

    std::memcpy(&sendPreBuffPtr[sendSize], printObjects.data(), printObjects.size() * sizeof(PrintData));
    sendSize += printObjects.size() * sizeof(PrintData);
}



void Game::sendStatus(int &sendSize)
{

    for(int i = 0; i < units.size(); ++i)
    {
        if(units[i].id == recvBuffPtr[1])
        {
            printStatus.pwr = units[i].pwr;
            break;
        }
    }

    if(sendSize + sizeof(PrintStatusData) > SEND_BUFF_WORK_SIZE)
        std::cout << "SendBuffer overload !\n";

    std::memcpy(&sendPreBuffPtr[sendSize], &printStatus, sizeof(PrintStatusData));
    sendSize += sizeof(PrintStatusData);
}



// 	printObjectsSize[2] = printObjects.size();



void Game::sendZero(int &sendSize)
{
    sendPreBuffPtr[0] = 5;
    sendSize = 1;
}



void Game::createPlayer(int &sendSize)
{
    createData.name.resize(recvBuffPtr[1]);
    std::memcpy((void*) createData.name.data(), &recvBuffPtr[3], recvBuffPtr[1]);

    unit.id = clientidBuff;
    unit.skin = recvBuffPtr[2];
    unit.name = createData.name;
    unit.x = 6;
    unit.y = 34;
    unit.pwr = 60;
    unit.time = 0;

    bool checkFreeSpace;
    do
    {
        checkFreeSpace = true;
        for(int i = 0; i < units.size(); ++i)
        {
            if(unit.x == units[i].x && unit.y == units[i].y)
            {
                checkFreeSpace = false;
                unit.y ++;
                break;
            }
        }
    }
    while(!checkFreeSpace);

    units.push_back(unit);
    unitsFrameNum ++;

    sendPreBuffPtr[0] = 2;
    sendPreBuffPtr[1] = clientidBuff;
    sendSize = 2;

    std::cout << "Create person id = " << clientidBuff << "\n";
}



void Game::movePlayer()
{
    for(int i = 0; i < units.size(); ++i)
    {
        if(units[i].id == recvBuffPtr[1])
        {
            if(checkObstacle(i)) // Проверка использует recvBuffPtr[2]
            {
                if(recvBuffPtr[2] == 1)
                    units[i].x++;
                if(recvBuffPtr[2] == 2)
                    units[i].x--;
                if(recvBuffPtr[2] == 3)
                    units[i].y++;
                if(recvBuffPtr[2] == 4)
                    units[i].y--;

                unitsFrameNum ++;
            }
            break;
        }
    }
}




void Game::deletePlayer(int clientid)
{
    for (int i = 0; i < units.size(); ++i)
    {
        if(units[i].id == clientid)
        {
            units.erase(units.begin() + i);
            unitsFrameNum ++;
            break;
        }
    }
    std::cout << "deletePlayer - " << clientid << "\nunits size = " << units.size() << "\n";
}



bool Game::checkObstacle(int checkindex)
{
    int moveX = 0;
    int moveY = 0;
    if(recvBuffPtr[2] == 1) moveX = 1;
    if(recvBuffPtr[2] == 2) moveX = -1;
    if(recvBuffPtr[2] == 3) moveY = 1;
    if(recvBuffPtr[2] == 4) moveY = -1;

    for(int i = 0; i < units.size(); ++i)
    {
        if(units[checkindex].x + moveX == units[i].x && units[checkindex].y + moveY == units[i].y)
        {
            return false;
        }
    }

    return true;
}



void Game::takePWR()
{
    for(int i = 0; i < units.size(); ++i)
    {
        if(units[i].id == recvBuffPtr[1])
        {
            for (int j = 0; j < pwrPoints.size(); ++j)
            {
                if(units[i].x == pwrPoints[j].x && units[i].y == pwrPoints[j].y && pwrPoints[j].pwr > 0)
                {
                    units[i].pwr ++;
                    pwrPoints[j].pwr --;
                    pwrPoints[j].time = 0;
                    pwrPointsFrameNum ++;
                    return;
                }
            }
            break;
        }
    }
}



void Game::unitPwrDecrement()
{
    for (int i = 0; i < units.size(); ++i)
    {
        units[i].time ++;
        if(units[i].time == 20)
        {
            units[i].pwr --;
            units[i].time = 0;
            pwrPointsFrameNum ++;
        }
    }
}


void Game::pointPwrIncrement()
{
    for (int i = 0; i < pwrPoints.size(); ++i)
    {
        if(pwrPoints[i].pwr < 9)
        {
            pwrPoints[i].time ++;
            if(pwrPoints[i].time == 60)
            {
                pwrPoints[i].pwr ++;
                pwrPoints[i].time = 0;
                pwrPointsFrameNum ++;
            }
        }
    }
}


