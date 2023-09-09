#pragma once

#include "BeastAttribute.hpp"
#include "SharedState.hpp"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

class Shared_state;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
	boost::beast::flat_buffer _buffer;
	boost::beast::websocket::stream<tcp::socket> _ws;
	std::shared_ptr<Shared_state> _state;
	std::vector<std::shared_ptr<const std::string>> _queue;

	void fail(error_code ec, const char* what);
	void onAccept(error_code ec);
	void onRead(error_code ec, std::size_t bytes_transferred);
	void onWrite(error_code ec, std::size_t bytes_transferred);

public:
	WebSocket(tcp::socket socket, const std::shared_ptr<Shared_state>& state)
		: _ws(std::move(socket)), _state(state) {}

	~WebSocket() { _state->leave(*this); }

	template<typename Body, typename Allocator>
	void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);

	void send(const std::shared_ptr<const std::string>& ss);
};

template<class Body, class Allocator>
void WebSocket::run(boost::beast::http::request<Body,
	boost::beast::http::basic_fields<Allocator>> req)
{
	_ws.async_accept(
		req,
		std::bind(
			&WebSocket::onAccept,
			shared_from_this(),
			std::placeholders::_1));
}
