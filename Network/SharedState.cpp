#include "SharedState.hpp"

void Shared_state::join(WebSocket& session)
{
	_sessions.insert(&session);
}

void Shared_state::leave(WebSocket& session)
{
	_sessions.erase(&session);
}

void Shared_state::send(std::string message)
{
	const auto ss = std::make_shared<const std::string>(std::move(message));

	for (auto session : _sessions)
		session->send(ss);
}
