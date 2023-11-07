#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <Network/Listener.hpp>
#include <Network/WebSocket.hpp>

TEST_CASE("WebSocket Test", "[WebSocket]")
{
    std::string host{ "127.0.0.1" };
    uint16_t port = 8080;
    std::string doc_root{ "." };
    auto state = std::make_shared<Shared_state>(doc_root);

    netAsio::io_context io;

    auto listener = std::make_shared<Listener>(
        io, tcp::endpoint{ netAsio::ip::make_address(host), port }, state);

    WebSocket websocket(tcp::socket(io), state);

    state->join(websocket);

    std::thread listenerThread([&] {
        listener->run();
        io.run();
        });

    SECTION("Fail function")
    {
        auto mesg = std::make_shared<std::string>("Hello");
        websocket.send(mesg);

    }

    SECTION("OnAccept function")
    {
        
    }

    SECTION("OnRead function")
    {

    }

    SECTION("OnWrite function")
    {

    }

    io.stop();
    listenerThread.join();
}

TEST_CASE("Shared_state Test", "[Shared_state]")
{
    SECTION("Join function")
    {

    }

    SECTION("Leave function")
    {

    }

    SECTION("Send function")
    {

    }
}
