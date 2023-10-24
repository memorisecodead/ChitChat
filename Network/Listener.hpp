#pragma once

#include <iostream>
#include <memory>
#include <string>

#include"BeastAttribute.hpp"
#include "HTTPSession.hpp"

class Shared_state;

/// sonar check

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

void Listener::fail(error_code ec, const char* what)
{
	if (ec == netAsio::error::operation_aborted)
		return;
	std::cerr << what << ": " << "\n";
}

void Listener::onAccept(error_code ec)
{
	if (ec)
		return fail(ec, ec.message().c_str());

	std::make_shared<HTTPSession>(std::move(_socket), _state)->run();

	_acceptor.async_accept(_socket, [self = shared_from_this()](error_code ec)
		{
			self->onAccept(ec);
		});
}

void Listener::configure(tcp::endpoint endpoint)
{
	error_code ec;

	_acceptor.open(endpoint.protocol(), ec);

	if (ec) { fail(ec, ec.message().c_str()); return; }

	_acceptor.set_option(netAsio::socket_base::reuse_address(true));

	if (ec) { fail(ec, ec.message().c_str()); return; }

	_acceptor.bind(endpoint, ec);

	if (ec) { fail(ec, ec.message().c_str()); return; }

	_acceptor.listen(netAsio::socket_base::max_listen_connections, ec);

	if (ec) { fail(ec, ec.message().c_str()); return; }
}

void Listener::run()
{
	_acceptor.async_accept(_socket,
		[self = shared_from_this()](error_code ec)
		{
			self->onAccept(ec);
		});
}
