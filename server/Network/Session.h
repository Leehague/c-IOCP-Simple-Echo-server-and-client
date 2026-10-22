#pragma once
#include "NetworkUtils.h"
#include <vector>
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include <memory>

class Session
{
public:
	

	Session(SOCKET* s);

	SOCKET GetSocket(){ return *_socket; }

	//WSARecv 호출 , 최초 호출은 바깥에서 해야됨
	void Receive(); 

	void OnReceive(int numOfWriteBytes); //Waiting thread -> active thread 로 바뀌며 받아온 I/O complretion 을 처리후 다시 Receive (WSARecv) 호출

	void Send(std::shared_ptr<SendBuffer> sendbuf);

	void OnSend();


private:
	SOCKET* _socket = nullptr;

	RecvBuffer _recvbuffer;

	
};