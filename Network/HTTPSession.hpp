#pragma once 

#include "BeastAttribute.hpp"
#include "SharedState.hpp"

#include <iostream>
#include <cstdlib>
#include <memory>

class HTTPSession : public std::enable_shared_from_this<HTTPSession>
{
	tcp::socket _socket;
	boost::beast::flat_buffer _buffer;
	std::shared_ptr<Shared_state> _state;
	boost::beast::http::request<boost::beast::http::string_body> _req;

	void fail(error_code ec, const char* what);
	void onRead(error_code ec, std::size_t);
	void onWrite(error_code ec, std::size_t, bool close);

public:
	HTTPSession(tcp::socket socket, const std::shared_ptr<Shared_state>& state)
		: _socket(std::move(socket)), 
		  _state(state) {}

	void run();
};