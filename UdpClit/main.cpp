#include <iostream>
#include <fstream>
#include <cstring>
#include <json/json.h>
#include <functional>
#include <unistd.h>
#include "UdpClit.h"
#include "DataPacket.h"

// plc的设备信息
struct DevInfo
{
	std::string ip;
	uint16_t port;	
};

int readJsonFile(std::string filename, DevInfo* devInfos[]);
int readJsonFile2(std::string filename, std::unique_ptr<DevInfo[]>& upDevInfos);

int main()
{
	std::unique_ptr<DevInfo[]>	upDevInfos;
	
	int devNum = readJsonFile2("ipConfig.json", upDevInfos);
	if(devNum == -1)
	{
		std::cerr<<"readJsonFile() err"<<std::endl;
		
		return -1;
	}
	std::cout<<"devNum = "<<devNum<<std::endl;
	
	UdpClit clit[devNum];

	// 初始化所有客户端的网络参数
	int index = 0;
	for(auto& tempClit : clit)
	{
		DevInfo devInfo = upDevInfos.get()[index];
		tempClit.initNetParam(devInfo.ip, devInfo.port);	

		index++;
	}

	// 创建多个线程，让多个客户端
	std::vector<pthread_t> tidVec(devNum);	
	for(int i = 0; i < devNum; i++)	
	{
		pthread_create(&tidVec[i], nullptr, [](void* arg) -> void* {

			UdpClit* clit = static_cast<UdpClit*>(arg);

			DataPacket dataPacket(40);	
			int dataLen = dataPacket.getDataLen();
			std::cout<<"dataLen ="<<dataLen<<std::endl;
			while(1)			
			{
				const char* data = dataPacket.getClearData();	
				clit->sendMsg(data, dataLen);
				sleep(1);
			}

			return nullptr;

		},static_cast<void*>(&clit[i]));	
	}
	
	for(auto& tid : tidVec)
	{
		pthread_join(tid, nullptr);
	}

	return 0;
}

// 传入json文件名，和设备信息结构体数组地址
int readJsonFile(std::string filename, DevInfo* devInfos[])
{
	std::ifstream ifs(filename.data());	
	if(!ifs.is_open())
	{
		std::cerr<<"open jsonfile  err"<<std::endl;
	
		return -1;
	}

	Json::Value root;
	Json::Reader reader;
	bool bl = reader.parse(ifs, root);
	if(!bl)
	{
		std::cerr<<"reader.parse() err"<<std::endl;
	
		return -1;
	}

	*devInfos = new DevInfo[root.size()];	

	int index = 0;
	for(const auto& tempValue : root)
	{
		devInfos[index]->ip = tempValue["devIp"].asString();	
		devInfos[index]->port = static_cast<uint16_t>(tempValue["devPort"].asUInt());	
		
		index++;
	}

	return root.size();
}

int readJsonFile2(std::string filename, std::unique_ptr<DevInfo[]>& upDevInfos)
{
	std::ifstream ifs(filename.data());	
	if(!ifs.is_open())
	{
		std::cerr<<"open jsonfile  err"<<std::endl;
	
		return -1;
	}

	Json::Value root;
	Json::Reader reader;
	bool bl = reader.parse(ifs, root);
	if(!bl)
	{
		std::cerr<<"reader.parse() err"<<std::endl;
	
		return -1;
	}
	
	// 创建独占的智能指针对象
	std::unique_ptr<DevInfo[]> tempUpDevinfos(new DevInfo[root.size()]);	
	// 控制权转移
	upDevInfos = move(tempUpDevinfos);

	int index = 0;
	for(const auto& tempValue : root)
	{
		upDevInfos.get()[index].ip = tempValue["devIp"].asString();	
		upDevInfos.get()[index].port = static_cast<uint16_t>(tempValue["devPort"].asUInt());	
		
		index++;
	}

	return root.size();
}