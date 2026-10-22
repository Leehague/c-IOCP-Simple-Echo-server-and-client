#include "IocpCore.h"
#include "Session.h"

void PrintErrorCode(int32_t errorCode)
{
	LPVOID errorMsg;

	// 시스템으로부터 에러 코드에 해당하는 메시지를 가져옵니다.
	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errorMsg,
		0,
		NULL
	);

	std::wcout << L"Error Code: " << errorCode << L" / Message: " << (wchar_t*)errorMsg << std::endl;

	// 사용된 메모리를 해제합니다.
	::LocalFree(errorMsg);
}


IocpCore::IocpCore()
{
	// IOCP 핸들 생성 (보통 코어 수에 맞추어 동시 실행 스레드 수를 지정, 0이면 CPU 코어 수만큼 자동 설정)
	_iocpHandle = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE, // 처음에는 기존 소켓 없이 생성하므로 INVALID_HANDLE_VALUE
		NULL,                 // 기존 IOCP 객체 (처음엔 NULL)
		0,                    // Completion Key (보통 0)
		0                     // Concurrency Value (0이면 시스템 코어 수)
	);
}

IocpCore::~IocpCore()
{
}

bool IocpCore::Register(std::shared_ptr<class Session> _clientsession)
{
	HANDLE h = ::CreateIoCompletionPort(
		(HANDLE)_clientsession->GetSocket(),
		_iocpHandle,
		(ULONG_PTR)_clientsession.get(), // shared_ptr이 아닌 실제 주소값 전달
		0
	);

	return (h != INVALID_HANDLE_VALUE);
}

void IocpCore::Dispactch(unsigned int timesout)
{
	DWORD BytesNum = 0;
	ULONG_PTR completionKey = 0;
	LPOVERLAPPED lpoverlapped = nullptr;
	bool result = ::GetQueuedCompletionStatus(_iocpHandle, &BytesNum , &completionKey, &lpoverlapped, timesout);
	


	if (lpoverlapped != nullptr)
	{
		OverlappedEx* overlappedex = CONTAINING_RECORD(lpoverlapped, OverlappedEx, overlapped);
		Session* session = reinterpret_cast<Session*>(completionKey);

		
		if (result == false)
		{
			//타임 아웃 제외 
			int32_t errCode = ::WSAGetLastError();
			if (errCode == WAIT_TIMEOUT) return;

			PrintErrorCode(errCode);

			return;
		}

		if (overlappedex->type == OperationType::Recv)
		{
			session->OnReceive(BytesNum);
		}
		else if (overlappedex->type == OperationType::Send)
		{
			session->OnSend();
		}

	}

}

