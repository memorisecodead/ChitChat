#include "Listener.hpp"

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
	_acceptor.set_option(netAsio::socket_base::reuse_address(true));
	_acceptor.bind(endpoint, ec);
	_acceptor.listen(netAsio::socket_base::max_listen_connections, ec);

	if (ec)
	{
		fail(ec, ec.message().c_str());
		return;
	}
}

void Listener::run()
{
	_acceptor.async_accept(_socket,
		[self = shared_from_this()](error_code ec)
		{
			self->onAccept(ec);
		});
}
