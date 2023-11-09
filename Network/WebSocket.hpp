#pragma once

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

#include "BeastAttribute.hpp"

class WebSocket;

template<typename T> struct Type2Type { typedef T OriginalType; };

/// for sonar view

class Shared_state
{
	std::string _doc_root;
	std::unordered_set<Type2Type<WebSocket>::OriginalType*> _sessions;

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

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
	boost::beast::flat_buffer _buffer;
	boost::beast::websocket::stream<tcp::socket> _ws;
	std::shared_ptr<Type2Type<Shared_state>::OriginalType> _state;
	std::vector<std::shared_ptr<const std::string>> _queue;

	void fail(error_code ec, const char* what);
	void onAccept(error_code ec);
	void onRead(error_code ec, std::size_t bytes_transferred);
	void onWrite(error_code ec, std::size_t bytes_transferred);

public:
	WebSocket(tcp::socket socket, const std::shared_ptr<Shared_state>& state)
		: _ws(std::move(socket)), _state(state) {}

	virtual ~WebSocket() { _state->leave(*this); }

	template<typename Body, typename Allocator>
	void run(boost::beast::http::request<Body
		, boost::beast::http::basic_fields<Allocator>> req);

	void send(const std::shared_ptr<const std::string>& ss);

	std::vector<std::shared_ptr<const std::string>> queue() { return _queue; }
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

	for (auto* session : _sessions)
		session->send(ss);
}

void WebSocket::fail(error_code ec, const char* what)
{
	if (ec == netAsio::error::operation_aborted ||
		ec == boost::beast::websocket::error::closed)
		return;

	std::cerr << what << ": " << "\n";
}

void WebSocket::onAccept(error_code ec)
{
	if (ec)
		return fail(ec, ec.message().c_str());

	_state->join(*this);

	_ws.async_read(_buffer,
		[sp = shared_from_this()](
			error_code ec, std::size_t bytes)
		{
			sp->onRead(ec, bytes);
		});
}

void WebSocket::onRead(error_code ec, std::size_t bytes_transferred)
{
	if (ec)
		return fail(ec, ec.message().c_str());

	_state->send(boost::beast::buffers_to_string(_buffer.data()));

	_buffer.consume(_buffer.size());

	_ws.async_read(
		_buffer,
		[sp = shared_from_this()](
			error_code ec, std::size_t bytes)
		{
			sp->onRead(ec, bytes);
		});
}

void WebSocket::onWrite(error_code ec, std::size_t bytes_transferred)
{
	if (ec)
		return fail(ec, ec.message().c_str());

	_queue.erase(_queue.begin());

	if (!_queue.empty())
		_ws.async_write(
			netAsio::buffer(*_queue.front()),
			[sp = shared_from_this()](
				error_code ec, std::size_t bytes)
			{
				sp->onWrite(ec, bytes);
			});
}

void WebSocket::send(const std::shared_ptr<const std::string>& ss)
{
	_queue.push_back(ss);

	if (_queue.size() > 1)
		return;

	_ws.async_write(
		netAsio::buffer(*_queue.front()),
		[sp = shared_from_this()](
			error_code ec, std::size_t bytes)
		{
			sp->onWrite(ec, bytes);
		});
}
