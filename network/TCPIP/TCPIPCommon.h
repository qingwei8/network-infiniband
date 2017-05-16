#pragma once
#include <crtdbg.h>
#include <string>
#include "NetworkCommon.h"

namespace hiveTCPIP
{
	const char END_OF_PACKET = '\n';
	const int  TCP_DEFAULT_MAX_PACKET_SIZE = 8192;
	const int  TCP_DEFAULT_RECEIVE_BUFFER_SIZE = TCP_DEFAULT_MAX_PACKET_SIZE * 2;
	const int  UDP_DEFAULT_RECEIVE_BUFFER_SIZE = 65536;
	const int  UDP_DEFAULT_MAX_PACKET_SIZE = 8192;
	const int  OUTGOING_PACKET_QUEUE_JAM_SIZE = 100;
	const int  CHECK_OUTGOING_PACKET_QUEUE_FREQUENCY = 50;
	const int  TCP_CONNECTION_MONITOR_DEFAULT_INTERVAL_TIME = 4000;

	const std::string DEFAULT_UDP_NODE_SIG = "UDP_NODE";
	const std::string DEFAULT_TCP_NODE_SIG = "TCP_NODE";

	namespace CONFIG_KEYWORD
	{
		const std::string PORT      = "PORT";
		const std::string SOCKET    = "SOCKET";
		const std::string TARGET_IP = "TARGET_IP";
		const std::string UDP_SOCKET_USAGE        = "UDP_SOCKET_USAGE";
		const std::string UDP_RECEIVE_BUFFER_SIZE = "UDP_RECEIVE_BUFFER_SIZE";
		const std::string UDP_MAX_PACKET_SIZE     = "UDP_MAX_PACKET_SIZE";
		const std::string TCP_RECEIVE_BUFFER_SIZE = "TCP_RECEIVE_BUFFER_SIZE";
		const std::string TCP_MAX_PACKET_SIZE     = "TCP_MAX_PACKET_SIZE";
		const std::string TCP_CONNECTION_MONITOR_INTERVAL_TIME = "TCP_CONNECTION_MONITOR_INTERVAL_TIME";
	}

	enum ESocketUsage
	{
		SOCKET_UNDEFINED = 1,
		SOCKET_BROADCAST = 2,
		SOCKET_MULTICAST = 4, 
		SOCKET_UNICAST   = 8,
		SOCKET_RECEIVE   = 16,
	};

	struct SSocketInfo
	{
		unsigned short Port;
		std::string    IP;
		ESocketUsage   Usage;

		bool operator==(const SSocketInfo& vRight)
		{
			return false;
		}
	};

	struct SRawDataBuffer
	{
	public:
		SRawDataBuffer() : m_pBuffer(nullptr), m_BufferSize(0) {}
		SRawDataBuffer(int vBufferSize) : m_pBuffer(new char[vBufferSize]), m_BufferSize(vBufferSize)  {}
		~SRawDataBuffer() {}

		void clear()   {_ASSERTE(m_pBuffer); memset(m_pBuffer, 0, m_BufferSize);}
		void reset()   {m_pBuffer = nullptr; m_BufferSize = 0;}
		void release() {delete m_pBuffer; m_pBuffer = nullptr; m_BufferSize = 0;}   //NOTE: 这里并没有把m_pBuffer的内存释放放到SRawDataBuffer的析构中而是要求显式的释放内存，这是因为m_pBuffer的写入完全开放，
		                                                                      //      用户可以把其他内存的指针（甚至指向已分配内存的一部分的指针）赋值给m_pBuffer，因此不能直接在析构函数中释放m_pBuffer
		                                                                      //      指向的内存。另外一个潜在解决方案是为SRawDataBuffer增加一个bool变量来指明是否在析构函数中自动释放m_pBuffer，但这个
		                                                                      //      方案可能在进行SRawDataBuffer之间的直接拷贝时，使得有两个SRawDataBuffer负责删除指向同一块内存的m_pBuffer
		void shallowCopy(const SRawDataBuffer& vSource) 
		{
			_ASSERTE(vSource.isValidBuffer()); 
			m_BufferSize = vSource.m_BufferSize; 
			m_pBuffer = vSource.m_pBuffer;
		}

		void incompleteShallowCopy(unsigned int vBufferSize, const SRawDataBuffer& vSource)
		{
			_ASSERTE(vSource.isValidBuffer() && (vBufferSize <= vSource.m_BufferSize));
			m_BufferSize = vBufferSize;
			m_pBuffer = vSource.m_pBuffer;
		}

		void incompleteShallowCopy(unsigned int vBufferSize, const SRawDataBuffer& vSource, unsigned int vOffset)
		{
			_ASSERTE(vSource.isValidBuffer() && ((vBufferSize + vOffset) <= vSource.m_BufferSize));
			m_BufferSize = vBufferSize;
			m_pBuffer = vSource.m_pBuffer + vOffset;
		}

		char* fetchBufferAddress() const {return m_pBuffer;}
		char  getContentAt(unsigned int vIndex) const {_ASSERTE(vIndex < m_BufferSize); return m_pBuffer[vIndex];}

		bool isValidBuffer() const {return m_pBuffer && (m_BufferSize > 0);}

		unsigned int getBufferSize() const {return m_BufferSize;}

	private:
		char *m_pBuffer;
		unsigned int m_BufferSize;
	};

	enum EOpOnNetworkFailure
	{
		TERMINATE_THREAD,
		CONTINUE_THREAD,
	};
}