#pragma once

#include "BeastAttribute.hpp"
#include "HTTPSession.hpp"

#include <iostream>
#include <memory>
#include <string>

class Shared_state;

class Listener : public std::enable_shared_from_this<Listener>
{
	tcp::acceptor _acceptor;
	tcp::socket _socket;
	std::shared_ptr<Shared_state> _state;

	void fail(error_code ec, const char* what);
	void onAccept(error_code ec);
	void configure(tcp::endpoint endpoint);

public:
	Listener(netAsio::io_context& io, 
			tcp::endpoint endpoint, 
			const std::shared_ptr<Shared_state>& state)
			: _acceptor(io),
			  _socket(io),
			  _state(state) 
	{
		configure(endpoint);
	}

	void run();
};
