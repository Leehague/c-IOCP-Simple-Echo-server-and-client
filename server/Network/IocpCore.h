#pragma once
#include "NetworkUtils.h"

// iocp handle 생성 및 관리 

// 주기적으로 GQCS(GetQueuedCompletionStatus) 호출( active(worker) thread 에 의해 호출 )




class IocpCore
{
public:
	IocpCore(); //기본 생성자
	~IocpCore();
	
	HANDLE GetIocpHandle() { return _iocpHandle; }


	//소켓을 IOCP 핸들에 등록 
	bool Register(std::shared_ptr<class Session> _clientsession);

	void Dispactch(unsigned int timesout); //주기적으로 GQCS(GetQueuedCompletionStatus) 호출

private:
	HANDLE _iocpHandle; // iocp handle 관리 
};