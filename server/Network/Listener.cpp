#include "Listener.h"
#include "NetworkUtils.h"
#include "Session.h"
#include "SessionManager.h"
#include <memory>
#include "IocpCore.h"

extern SessionManager Gsessionmanager;


Listener::Listener()
{
}

bool Listener::StartAccept(int port, IocpCore* iocpcore)
{
	WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false; // Winsock 2.2 버전으로 초기화 요청

	_listensocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (_listensocket == INVALID_SOCKET)
	{
		int errorCode = ::WSAGetLastError();
		// TODO: 로깅 및 적절한 에러 처리/종료
		return false;
	}

	//주소 구조체(SOCKADDR_IN) 설정
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;                    // IPv4 사용
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);     // 내 컴퓨터의 모든 IP에서의 접속을 허용
	serverAddr.sin_port = htons(port);                  // 사용할 포트 번호 지정 

	if (bind(_listensocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		// 바인딩 실패 처리 (예: 이미 다른 프로그램이 해당 포트를 쓰고 있을 때 등)
		int err = WSAGetLastError();
		closesocket(_listensocket);
		return false;
	}

	if (listen(_listensocket, SOMAXCONN) == SOCKET_ERROR) {
		int err = WSAGetLastError();
		closesocket(_listensocket);
		return false;
	}

}

void Listener::Execute(IocpCore* iocpcore)
{
	while (true)
	{
		
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(clientAddr);
		SOCKET newsocket = ::accept(_listensocket, (SOCKADDR*)&clientAddr, &addrLen);

		if (newsocket == INVALID_SOCKET)
		{
			//에러처리
			std::cout << "invalid socket" << std::endl;
			continue;
		}
		
		std::shared_ptr<Session> newsession = std::make_shared<Session>(&newsocket);
		Gsessionmanager.AddSesion(newsession);
		if (iocpcore->Register(newsession))
		{
			newsession->Receive(); //WSA 최초 호출
		}
		
	}
	
}
