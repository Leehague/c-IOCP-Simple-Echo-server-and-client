#pragma once

#include <vector>
#include "NetworkUtils.h"

class SendBuffer
{

public:

	SendBuffer(int buffersize);
	WSABUF* GetWSABuffer();
	char* GetBuffStart() { return &_buffer[0]; };

	void SetWritepos(int writepos) { _writepos = writepos; };
private:

	int  _capacity = 0;
	int _writepos = 0;

	WSABUF _wsabuf;
	std::vector<char> _buffer;
};