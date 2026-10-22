#include "SessionManager.h"

void SessionManager::AddSesion(std::shared_ptr<Session> s)
{
	_sessions.push_back(s);
}
