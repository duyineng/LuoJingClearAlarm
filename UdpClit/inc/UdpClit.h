#pragma once

#include <string>
#include <arpa/inet.h>

class UdpClit
{
public:
	// 无参构造
	UdpClit();
	// 有参构造
	UdpClit(std::string& servIp, unsigned short servPort);
	~UdpClit();
	
	// 初始化网络参数
	int initNetParam(std::string& servIp, unsigned short servPort);	
	// 发送消息
	int sendMsg(const void* buf, size_t bufLen);

private:
	int m_fd = -1;
	struct sockaddr_in m_servAddr{};
	socklen_t m_servLen;
};
