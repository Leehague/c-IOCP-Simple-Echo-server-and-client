#pragma once
#include <winsock2.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // Winsock 라이브러리 링크


enum OperationType
{
	Recv = 0,
	Send = 1
};

class Session;
struct OverlappedEx
{
	OVERLAPPED overlapped;
	Session* session;
	
	OperationType type;
};


// 패킷의 가장 앞에 붙는 메타데이터
struct PacketHeader {
	std::uint16_t size; // 패킷의 전체 크기 (헤더 + 데이터)
	std::uint16_t id;   // 패킷 종류 
};
