#include "UdpClit.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

UdpClit::UdpClit()
{
	
}

UdpClit::~UdpClit()
{
	if(m_fd != -1)
	{
		close(m_fd);
	}
}

// 有参构造，指定客户端要连接的服务器ip和端口
UdpClit::UdpClit(std::string& servIp, unsigned short servPort)
{
	// 创建套接字
	// SOCK_DGRAM表示用于报式协议，0表示报式协议中的UDP协议
	m_fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if(m_fd == -1)
	{
		std::string errorMsg = "socket() err: " + std::string(strerror(errno)); 
		throw std::runtime_error(errorMsg);
	}

	// 绑定服务器地址结构 
	m_servAddr.sin_family = AF_INET;	
	int ret = inet_pton(AF_INET, servIp.data(), (void*)&m_servAddr.sin_addr.s_addr);	
	if(ret != 1)
	{
		std::string errorMsg = "inet_pton() err: " + std::string(strerror(errno)); 
		throw std::runtime_error(errorMsg);
	}
	m_servAddr.sin_port = htons(servPort);

	m_servLen = sizeof(m_servAddr);
}

// 成功返回0，失败返回-1
int UdpClit::initNetParam(std::string& servIp, unsigned short servPort)
{
	// 创建套接字
	m_fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if(m_fd == -1)
	{
		perror("socket() err");
		
		return -1;
	}

	// 绑定服务器地址结构 
	m_servAddr.sin_family = AF_INET;	
	int ret = inet_pton(AF_INET, servIp.data(), (void*)&m_servAddr.sin_addr.s_addr);	
	if(ret != 1)
	{
		perror("inet_pton() err");

		return -1;
	}
	m_servAddr.sin_port = htons(servPort);

	m_servLen = sizeof(m_servAddr);

	return 0;
}

// 传入要发送的buf缓冲区，要发送的实际大小
int UdpClit::sendMsg(const void* buf, size_t bufLen)
{
	int sendLen = sendto(m_fd, buf, bufLen, 0, (struct sockaddr*)&m_servAddr, m_servLen);	
	if(sendLen == -1)
	{
		perror("sendto() err");

		return -1;
			
	}

	// 成功，返回实际发送的字节数
	return sendLen;	
}
