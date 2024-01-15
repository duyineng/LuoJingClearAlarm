#include <iostream>
#include <fstream>
#include <cstring>
#include <json/json.h>
#include <functional>
#include <unistd.h>
#include "UdpClit.h"
#include "DataMaker.h"

// plc的设备信息
struct DevInfo
{
	std::string ip;
	uint16_t port;	
};

int readJsonFile(std::string filename, std::unique_ptr<DevInfo[]>& upDevInfos);

int main()
{
	// json文件的内容读到upDevInfos中
	std::unique_ptr<DevInfo[]>	upDevInfos;
	int devNum = readJsonFile("ipConfig.json", upDevInfos);
	if(devNum == -1)
	{
		std::cerr<<"readJsonFile() err"<<std::endl;
		
		return -1;
	}
	
	// 初始化所有客户端的网络参数
	int index = 0;
	UdpClit clit[devNum];
	for(auto& tempClit : clit)
	{
		DevInfo devInfo = upDevInfos.get()[index++];
		tempClit.initNetParam(devInfo.ip, devInfo.port);	
	}

	// 创建多个线程
	std::vector<pthread_t> vecTid(devNum);	
	for(int i = 0; i < devNum; i++)	
	{
		pthread_create(&vecTid[i], nullptr, [](void* arg) -> void* {

			UdpClit* clit = static_cast<UdpClit*>(arg);

			DataMaker dataMaker(40);	
			while(1)			
			{
				clit->sendMsg(dataMaker.getClearAlarmData(), dataMaker.getDataLen());

				usleep(100 * 1000);
			}

			return nullptr;

		},static_cast<void*>(&clit[i]));	
	}
	
	for(auto& tid : vecTid)
	{
		pthread_join(tid, nullptr);
	}

	return 0;
}

// 传入json文件名和设备信息结构体数组地址
int readJsonFile(std::string filename, std::unique_ptr<DevInfo[]>& upDevInfos)
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
