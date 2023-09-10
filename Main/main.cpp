#include <boost/asio/signal_set.hpp>

#include <Network/Listener.hpp>
#include <Network/SharedState.hpp>

int main(int argc, char* argv[])
{
    std::string host{"127.0.0.1"};
    uint16_t port = 8080;
    std::string doc_root{ "." };

    netAsio::io_context io;

    std::make_shared<Listener>(
        io, tcp::endpoint{netAsio::ip::make_address(host), port},
        std::make_shared<Shared_state>(doc_root))->run();

    netAsio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(
        [&io](boost::system::error_code const&, int)
        {
            io.stop();
        });

    io.run();

    return EXIT_SUCCESS;
}
