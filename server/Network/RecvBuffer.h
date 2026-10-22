#pragma once

#include <vector>
#include "NetworkUtils.h"

constexpr auto Basic_Buffer_size = 65536; // 2 ^ 16
//쓰기는 커널(윈도우 커널)담당, 읽기는 게임서버(유저 프로세스) 담당
class RecvBuffer
{
public:
	
	RecvBuffer();
	RecvBuffer(int buffersize);

	bool OnWrite(int Writebytes);
	bool OnRead(int Readbytes);

	WSABUF* GetWSABuffer();
	void Clear();

public:	
	char* WritePos() { return _buffer.data() + _writePos; }

	char* ReadPos() { return _buffer.data() + _readPos; }

	//빈공간 크기(쓰기가 되지 않고 남은 크기) _writePos ~ end
	int NonWriteSize() { return _capacity - _writePos; };

	// 현재 버퍼에 쌓여있는 데이터 크기(쓰기는 됬고 읽기가 되지 않은 크기) _readPos ~ _writePos
	int NonReadSize() { return _writePos - _readPos; };

	char GetcharinBuf(int idx) { return _buffer[_readPos+idx]; }

private:
	int             _capacity = 0;
	int             _readPos = 0;
	int             _writePos = 0;
	std::vector<char> _buffer;
	WSABUF _wsabuf;
	
};