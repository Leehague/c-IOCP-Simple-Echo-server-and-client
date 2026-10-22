#include "Session.h"
#include "RecvBuffer.h"

Session::Session(SOCKET* s)
{
	_socket = s;
	_recvbuffer = RecvBuffer();
}

void Session::Receive()
{
	std::cout << "Receive Start" << std::endl;

	OverlappedEx* recvOverlapped = new OverlappedEx(); // 새로운 오버랩ex 생성

	//초기화
	ZeroMemory(&recvOverlapped->overlapped, sizeof(OVERLAPPED)); //생성한 오버랩ex recvOverlapped 의 overlapped 부분 초기화 

	recvOverlapped->session = this;
	recvOverlapped->type = OperationType::Recv;

	DWORD bytesReceived = 0;
	DWORD flags = 0;

	// WSARecv 호출
	int result = WSARecv(
		GetSocket(),            // 데이터를 받을 대상 소켓
		(_recvbuffer.GetWSABuffer()),    // 수신할 버퍼 정보 , Session에 귀속된 RecvBuffer 객체에서 관리함
		1,                       // 버퍼 개수 (보통 1개)
		&bytesReceived,          // [출력] 동기적으로 즉시 수신된 바이트 수 (비동기면 보통 0)
		&flags,                  // 플래그 (보통 0)
		(LPOVERLAPPED)&recvOverlapped->overlapped, // 핵심: 오버랩 구조체 주소 전달
		NULL                     // 비동기 완료 시 호출될 콜백 함수 (IOCP를 쓰므로 NULL)
	);

	// 에러 처리
	if (result == SOCKET_ERROR) {
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			// 진짜 에러 발생! 연결 끊김 등 처리
			delete recvOverlapped;
		}
	}
}

void Session::OnReceive(int numOfWriteBytes)
{
	std::cout << " call OnReceive" << std::endl;

	if (_recvbuffer.OnWrite(numOfWriteBytes) == false)
	{
		_recvbuffer.Clear();
		return;
	}

	

	while (true)
	{
		//패킷 헤더 만큼은 왔는지 확인
		if (_recvbuffer.NonReadSize() < sizeof(PacketHeader))
		{
			_recvbuffer.Clear();
			break;
		}

		PacketHeader* header = reinterpret_cast<PacketHeader*>(_recvbuffer.ReadPos());

		std::uint16_t packetsize = header->size;

		if (_recvbuffer.NonReadSize() < packetsize)
		{
			_recvbuffer.Clear();
			break;
		}

		//패킷 처리로직

		//서버에 출력
		uint16_t bodySize = packetsize - sizeof(PacketHeader);
		std::string bodyMsg(_recvbuffer.ReadPos() + sizeof(PacketHeader), bodySize);
		std::cout << "[수신 패킷 ID: " << header->id << "] " << bodyMsg << std::endl;
		
		
		//echo (전송)
		std::shared_ptr<SendBuffer> sendbuffer = std::make_shared<SendBuffer>(packetsize * 2);
		std::copy(_recvbuffer.ReadPos(), _recvbuffer.ReadPos() + packetsize, sendbuffer->GetBuffStart());
		sendbuffer->SetWritepos(packetsize);
		
		
		Send(sendbuffer);
		if (_recvbuffer.OnRead(packetsize) == false)
		{
			_recvbuffer.Clear();
			return;
		}
	}

	_recvbuffer.Clear();

	Receive();
}

void Session::Send(std::shared_ptr<SendBuffer> sendbuf)
{
	OverlappedEx* overlappedex = new OverlappedEx();
	overlappedex->session = this;
	overlappedex->type = OperationType::Send;
	DWORD buffercount=0;
	WSASend(GetSocket() , sendbuf->GetWSABuffer(),1, &buffercount,0, &(overlappedex->overlapped), nullptr);
}

void Session::OnSend()
{

}
