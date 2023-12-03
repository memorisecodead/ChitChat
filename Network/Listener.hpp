#pragma once

#include <iostream>
#include <memory>
#include <string>

#include"BeastAttribute.hpp"
#include "HTTPSession.hpp"

/**
* @brief declaration of Shared_state
*/
class Shared_state;

/**
* @class Listener
* @brief This class performs some of the logic required to start the WebServer
* @details This object is directly the root of the back-end. The main tasks are to 
*		   communicate with HTTPSession, accepting new hosts, processing and accepting messages
*		   from them. The Listener object is created HTTPSession for communicate between clients.
*/
class Listener : public std::enable_shared_from_this<Listener>
{
	tcp::acceptor _acceptor;
	tcp::socket _socket;
	std::shared_ptr<Shared_state> _state;

	/**
	* @brief method called in case of an error 
	*		 and passing the error code and error description.
	*/
	void fail(error_code ec, const char* what);

	/**
	* @brief method for creating an http session
	* @details if the connection to the address and port is correct, the http session is started.
	*/
	void onAccept(error_code ec);
	
	/**
	* @brief method to verify the correct and working address
	* @details if one of the checks fails, the listener will not be started.
	*/
	void configure(tcp::endpoint endpoint);

public:
	/**
	* @brief definition of constructor
	*/
	Listener(asio::io_context& io,
			tcp::endpoint endpoint, 
			const std::shared_ptr<Shared_state>& state)
			: _acceptor(io),
			  _socket(io),
			  _state(state) 
	{
		configure(endpoint);
	}

	/**
    * @brief method to start the accepting connection
    *        by initiating an asynchronous read operation
	*/
	void run();
};

void Listener::fail(error_code ec, const char* what)
{
	if (ec == asio::error::operation_aborted)
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

	_acceptor.set_option(asio::socket_base::reuse_address(true));

	if (ec) { fail(ec, ec.message().c_str()); return; }

	_acceptor.bind(endpoint, ec);

	if (ec) { fail(ec, ec.message().c_str()); return; }

	_acceptor.listen(asio::socket_base::max_listen_connections, ec);

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
