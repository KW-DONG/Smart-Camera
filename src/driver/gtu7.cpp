#include "gtu7.h"
#include <vector>
#include <iostream>
#include <regex>

void GTU7::threadLoop()
{
    while (mRun) {
        getMessage();
    }
}

void GTU7::getMessage()
{
    if (mSerialCallback == nullptr) return;
    std::string data = receive();
    std::vector<std::string> vStr;
    std::string delimiter = ",";
    size_t pos = data.find("$GPGLL");
    if (pos != std::string::npos)
    {
        std::cout << "Found" << std::endl;
        std::string croppedStr = data.substr(pos, data.length());
        for (int i = 0; i < 5 && pos != std::string::npos; i++)
        {
            pos = croppedStr.find(delimiter);
            if (pos == 0)
                return;
            vStr.push_back(croppedStr.substr(0, pos));
            croppedStr.erase(0, pos + delimiter.length());
        }
        double lat = vStr[2] == "N" ? std::stod(vStr[0]) : -std::stod(vStr[0]);
        double lon = vStr[4] == "W" ? std::stod(vStr[3]) : -std::stod(vStr[3]);
        std::cout << "latitude: " << lat << " longitude: " << lon << std::endl;
        mSerialCallback->next(lat, lon);
    }
}

void GTU7::start(std::string& portName)
{
    if (open(portName.c_str(),115200,0,8,1,1))
    {
        mThread = std::thread(&GTU7::threadLoop, this);
        mRun = true;
    }
}

void GTU7::stop()
{
    if (mRun == true)
    {
        mRun = false;
        mThread.join();
    }
}