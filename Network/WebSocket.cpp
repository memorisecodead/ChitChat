#include "WebSocket.hpp"

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
