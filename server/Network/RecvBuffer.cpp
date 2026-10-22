#include "RecvBuffer.h"

RecvBuffer::RecvBuffer()
{
    int buffersize = Basic_Buffer_size;
    _capacity = buffersize;
    _buffer.resize(buffersize);
    _wsabuf.buf = _buffer.data();
    _wsabuf.len = 0;
}

RecvBuffer::RecvBuffer(int buffersize)
{
    _capacity = buffersize;
    _buffer.resize(buffersize);
    _wsabuf.buf = _buffer.data();
    _wsabuf.len = 0;
}

bool RecvBuffer::OnWrite(int Writebytes)
{
    if (Writebytes > NonWriteSize())
    {
        return false;
    }

    _writePos += Writebytes;
    return true;
}

bool RecvBuffer::OnRead(int Readbytes)
{
    if (Readbytes > NonReadSize())
    {
        return false;
    }

    _readPos += Readbytes;
    return true;
}

WSABUF* RecvBuffer::GetWSABuffer()
{
    _wsabuf.buf = WritePos();
    _wsabuf.len = NonWriteSize();
    return &_wsabuf;
}

void RecvBuffer::Clear()
{
    int nonreadsize = NonReadSize(); // nonreadsize 캐싱
    //1. 읽기를 할 내용이 더이상 없는 경우 , _readPos == _writePos
    if (nonreadsize == 0)
    {
        _buffer.clear();
        _readPos = 0;
        _writePos = 0;
    }
    //2. 쓰기는 됬고 읽지는 않은 내용이 남아 있는 경우 _readPos , ... ,  _writePos
    else
    {
        std::copy(_buffer.begin() + _readPos, _buffer.begin() + _writePos, _buffer.begin());
        _readPos = 0;
        _writePos = nonreadsize;
    }
}
