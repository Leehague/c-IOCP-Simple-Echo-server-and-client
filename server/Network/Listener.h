#pragma once
#include "NetworkUtils.h"
//소켓 생성 및 바인드 , 리스너 소켓등록
//새로운 클라이언트가 접속할 때마다 active 소켓을 생성해서 Session 과 연결시켜야함

class Listener
{
public:
	Listener();
	
	//소켓 생성 및 바인드 , 리스너 소켓등록
	bool StartAccept(int port , class IocpCore* iocpcore); 

	//새로운 클라이언트가 접속할 때마다 active 소켓을 생성해서 Session 과 연결
	void Execute(IocpCore* iocpcore); 
private:
	SOCKET _listensocket =0;
};