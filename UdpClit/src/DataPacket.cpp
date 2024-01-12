#include "DataPacket.h"

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>

// 默认构造函数
DataPacket::DataPacket()
{

}

// 有参构造函数
DataPacket::DataPacket(int len)
{
	m_dataLen = len;
	m_data = std::unique_ptr<char[]>(new char[len]);
}

// 析构函数
DataPacket::~DataPacket()
{

}

// 分配数据包长度
void DataPacket::mallocDataLen(int len)
{
	m_dataLen = len;
	m_data = std::unique_ptr<char[]>(new char[len]);
}

// 获取数据包长度
int DataPacket::getDataLen()
{
	return m_dataLen;	
}

// 获取用于清除告警的数据包
const char* DataPacket::getClearData()
{
	int ret = writeData<uint16_t>(0, 2, 1);		
	if(ret == -1)
	{
		std::cerr<<"writeData<uint16_t>() err"<<std::endl;
		
		return nullptr;
	}

	ret = writeData<uint16_t>(2, 2, 801);		
	if(ret == -1)
	{
		std::cerr<<"writeData<uint16_t>() err"<<std::endl;

		return nullptr;
	}

	ret = writeData<uint32_t>(36, 4, m_counter++);		
	if(ret == -1)
	{
		std::cerr<<"writeData<uint16_t>() err"<<std::endl;

		return nullptr;
	}

	return m_data.get();	
}

template<typename T>
int DataPacket::writeData(size_t startIndex, size_t len, T value)
{
	if((startIndex + len) > 40)	
	{
		std::cerr<<"将在数据块范围外写入数据，错误！"<<std::endl;
		
		return -1;
	}
	
	T bigendData;
	if(len == 2)
	{
		bigendData = htons(value);	
	}
	else if(len == 4)
	{
		bigendData = htonl(value);	
	}

	memcpy(m_data.get() + startIndex, &bigendData, len);

	return 0;
}


