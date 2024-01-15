#pragma once 

#include <memory>

class DataMaker
{
public:
	// 默认构造函数
	DataMaker();
	// 有参构造函数
	explicit DataMaker(int len);
	// 删除拷贝构造
	DataMaker(const DataMaker&) = delete;		
	// 删除赋值运算符重载
	DataMaker& operator=(const DataMaker&) = delete;	
	~DataMaker();

	void mallocDataLen(int len);
	int getDataLen();
	const char* getClearAlarmData();
private:
	template<typename T>
	int writeData(size_t startIndex, size_t len, T value);

private:
	// 创建智能指针对象，不管理任何内存
	std::unique_ptr<char[]> m_data;
	size_t m_dataLen = 0;
	uint32_t m_counter = 1;
};
