#include "SendBuffer.h"


SendBuffer::SendBuffer(int buffersize)
{
	_buffer = std::vector<char>(buffersize);
	_capacity = buffersize;
	_wsabuf.buf = _buffer.data();
	_wsabuf.len = 0;
}

WSABUF* SendBuffer::GetWSABuffer()
{
	_wsabuf.buf = _buffer.data();
	_wsabuf.len = _capacity - _writepos;

	return &_wsabuf;
}