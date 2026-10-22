#pragma once
#include "Network/NetworkUtils.h"
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <atomic>
#include "Network/SessionManager.h"
#include "Network/Listener.h"
#include "Network/IocpCore.h"


SessionManager Gsessionmanager;

constexpr size_t WorkerThreadNum = 4;
constexpr unsigned int waitingTimeOut = 10; //ms 단위


std::atomic<bool> GIsRunning = true;
void WorkerThread(IocpCore* iocpcore)
{

	while (GIsRunning.load())
	{
		iocpcore->Dispactch(waitingTimeOut);
	}
}

int main()
{
	//초기화
	Gsessionmanager = SessionManager();
	Listener listener;
	IocpCore iocpcore;

	listener.StartAccept(7777 , &iocpcore);

	std::thread t(&Listener::Execute, &listener, &(iocpcore));
	t.detach();

	//Worker thread
	std::vector <std::thread> workerthreads;
	workerthreads.reserve(WorkerThreadNum);

	for (int i = 0; i < WorkerThreadNum; i++)
	{
		workerthreads.emplace_back(WorkerThread , &iocpcore);
	}

	std::string input;
	while (true)
	{
		std::cout << "> ";
		if (!std::getline(std::cin, input) || input == "exit")
		{
			GIsRunning.store(false);
			break;
		}
		if (input.empty()) continue;
	}

	for (auto& t : workerthreads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
	return 0;
}