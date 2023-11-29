#include <boost/asio/signal_set.hpp>

#include <Network/Listener.hpp>
#include <Network/ListenerConfig.hpp>

int main()
{
    asio::io_context io;

    std::make_shared<Listener>(
        io, tcp::endpoint{ asio::ip::make_address(host::address), port::channel},
        std::make_shared<Shared_state>(root::base))->run();

    asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const&, int)
        {
            io.stop();
        });

    io.run();

    return EXIT_SUCCESS;
}
