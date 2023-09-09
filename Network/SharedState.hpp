#pragma once

#include <string>
#include <memory>
#include <unordered_set>

#include "WebSocket.hpp"

class WebSocket;

class Shared_state
{
	std::string _doc_root;
	std::unordered_set<WebSocket*> _sessions;

public:
	explicit Shared_state(std::string doc)
		: _doc_root(std::move(doc)) {}

	const std::string& doc_root() const noexcept
	{
		return _doc_root;
	}

	void join(WebSocket& session);
	void leave(WebSocket& session);
	void send(std::string message);
};
