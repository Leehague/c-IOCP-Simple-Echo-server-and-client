#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int SERVER_PORT = 7777;
constexpr int BUFFER_SIZE = 1024;


struct PacketHeader {
    std::uint16_t size; // 패킷의 전체 크기 (헤더 + 데이터)
    std::uint16_t id;   // 패킷 종류 
};

// 서버로부터 데이터를 계속 수신하는 백그라운드 스레드 함수
void RecvWorker(SOCKET clientSocket)
{
    char recvBuffer[BUFFER_SIZE];
    while (true)
    {
        int bytesReceived = recv(clientSocket, recvBuffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) break;
        // 최소한 헤더 크기 이상은 받아야 파싱 가능
        if (bytesReceived >= sizeof(PacketHeader))
        {
            PacketHeader* header = reinterpret_cast<PacketHeader*>(recvBuffer);
            uint16_t bodySize = header->size - sizeof(PacketHeader);
            // 헤더 뒷부분(recvBuffer + sizeof(PacketHeader))부터 bodySize만큼 문자열 생성
            std::string msg(recvBuffer + sizeof(PacketHeader), bodySize);
            std::cout << "\n[서버 수신 (ID:" << header->id << ")]: " << msg << "\n> " << std::flush;
        }
    }
}

int main()
{
    
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup 실패\n";
        return 1;
    }
    //소켓 생성 (TCP)
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "소켓 생성 실패: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }
    //서버 주소 설정 및 연결
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    std::cout << "서버(" << SERVER_IP << ":" << SERVER_PORT << ")에 연결 시도 중...\n";
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "연결 실패: " << WSAGetLastError() << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "서버 연결 성공! (메시지를 입력하고 Enter를 누르세요. 'exit' 입력 시 종료)\n";
    //수신 전용 스레드 시작
    std::thread recvThread(RecvWorker, clientSocket);
    //메인 스레드: 사용자 입력 및 송신(Send) 루프
    std::string input;
    while (true)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, input) || input == "exit")
        {
            break;
        }
        if (input.empty()) continue;

        
        const uint16_t bodySize = static_cast<uint16_t>(input.size());
        const uint16_t packetSize = sizeof(PacketHeader) + bodySize;

        std::vector<char> sendbuf(packetSize);
        
        //헤더채우기
        PacketHeader* header = reinterpret_cast<PacketHeader*>(sendbuf.data());
        header->id = 1;
        header->size = packetSize;
        
        //body 채우기
        std::copy(input.begin(), input.end(), sendbuf.begin() + sizeof(PacketHeader));

        int sendResult = send(clientSocket, &sendbuf[0], static_cast<int>(sendbuf.size()), 0);
        if (sendResult == SOCKET_ERROR)
        {
            std::cerr << "send 실패: " << WSAGetLastError() << "\n";
            break;
        }
    }
    // 6. 종료 처리
    closesocket(clientSocket);
    if (recvThread.joinable())
    {
        recvThread.join();
    }
    WSACleanup();
    return 0;
}
