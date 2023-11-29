#pragma once

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

#include "BeastAttribute.hpp"

/**
* @brief declaration of WebSocket
*/
class WebSocket;

/**
* @brief type adapter
*/
template<typename T> struct Type2Type { typedef T OriginalType; };

/**
* @brief class Shared_state
* @details class appears to be a container for maintaining the shared state of WebSocket sessions. 
*		   It stores the document root path and keeps track of active WebSocket sessions.
*/
class Shared_state
{
	std::string _doc_root;
	std::unordered_set<Type2Type<WebSocket>::OriginalType*> _sessions;

public:
	/**
	* @brief definition of constructor
	*/
	explicit Shared_state(std::string doc)
		: _doc_root(std::move(doc)) {}


	/**
	* @brief getter for current root
	*/
	const std::string& doc_root() const noexcept
	{
		return _doc_root;
	}

	/**
	* @brief method for takes a reference to a WebSocket object session 
	*        and adds it to the set of WebSocket sessions _sessions using the insert function
	*/
	void join(WebSocket& session);
	
	/**
	* @brief method for takes a reference to a WebSocket object session
	*        and removes it from the set of WebSocket sessions _sessions using the erase function 
	*/
	void leave(WebSocket& session);
	
	/**
	* @brief method then iterates over each pointer session in the _sessions 
	* @details set and calls the send function on each session, passing the ss shared pointer as an argument.
	*        This function is responsible for sending a message to all WebSocket sessions in the shared state.
	*/
	void send(std::string message);
};

/**
* @brief class WebSocket
* @details Is used to establish and maintain a WebSocket connection 
*		   between a client and a server.
*/
class WebSocket : public std::enable_shared_from_this<WebSocket>
{
	boost::beast::flat_buffer _buffer;
	boost::beast::websocket::stream<tcp::socket> _ws;
	std::shared_ptr<Type2Type<Shared_state>::OriginalType> _state;
	std::vector<std::shared_ptr<const std::string>> _queue;

	/**
	* @brief method called in case of an error 
	*	     and passing the error code and error description.
	*/
	void fail(error_code ec, const char* what);
	
	/**
	* @brief method is called after accepting a WebSocket connection
	* @details If ec contains an error, the function calls the fail function with the corresponding error message.
	*          If the connection acceptance is successful, the function calls the join shared state _state function 
	*          to add the current WebSocket object to the list of active connections. 
	*		   The function then asynchronously reads data from the socket _ws 
	*		   into the _buffer and passes control to the onRead function.
	*/
	void onAccept(error_code ec);
	
	/**
	* @brief method is called after reading data from a WebSocket
	* @details If ec contains an error, the function calls the fail function with the corresponding error message.
	*		   If the data read is successful, the function sends the data to the shared _state using the 
	*		   send function, passing the data from the _buffer. The function then clears the _buffer using the consume 
	*		   function and asynchronously continues reading data from the socket into the _buffer 
	*		   and passes control to the onRead function.
	*/
	void onRead(error_code ec, std::size_t bytes_transferred);
	
	/**
	* @brief method is called after successful writing of data to the WebSocket
	* @details If ec contains an error, the function calls the fail function with the corresponding error message. 
	*          If the data write was successful, the function removes the first element from the _queue 
	*		   because the data was successfully sent. The function then checks whether the _queue queue is empty. 
	*		   If the queue is not empty, the function asynchronously writes data from _queue.front() to the _ws socket 
	*		   and passes control to the onWrite function.
	*/
	void onWrite(error_code ec, std::size_t bytes_transferred);

public:
	/**
	* @brief definition of constructor
	*/
	WebSocket(tcp::socket socket, const std::shared_ptr<Shared_state>& state)
		: _ws(std::move(socket)), _state(state) {}

	/**
	* @brief class destructor that calls the leave generic _state function 
	*        when the WebSocket object is destroyed
	*/
	virtual ~WebSocket() { _state->leave(*this); }

	/**
	* @brief method that performs HTTP request processing 
	*        and WebSocket connection establishment
	*/
	template<typename Body, typename Allocator>
	void run(boost::beast::http::request<Body
		, boost::beast::http::basic_fields<Allocator>> req);

	/**
	* @brief method for sending a message over WebSocket
	* @details It takes a std::shared_ptr on a constant std::string 
	*          object that represents the message to send. 
	*          The function adds the message to the _queue queue.
	*/
	void send(const std::shared_ptr<const std::string>& ss);

	/**
	* @brief getter for current queue
	*/
	std::vector<std::shared_ptr<const std::string>> queue() const { return _queue; }
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
	if (ec == asio::error::operation_aborted ||
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
			asio::buffer(*_queue.front()),
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
		asio::buffer(*_queue.front()),
		[sp = shared_from_this()](
			error_code ec, std::size_t bytes)
		{
			sp->onWrite(ec, bytes);
		});
}
