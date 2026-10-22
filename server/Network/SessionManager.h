
#include "Session.h"


class SessionManager
{
public:
	void AddSesion(std::shared_ptr<Session> s);
private:

	std::vector<std::shared_ptr<Session>> _sessions;
};