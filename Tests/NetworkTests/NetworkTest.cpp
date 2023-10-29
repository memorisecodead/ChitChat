#include <catch2/catch_test_macros.hpp>

#include <Network/Listener.hpp>
#include <Tests/NetworkTests/GearTests.hpp>

TEST_CASE("WebSocket tests", "[WebSocket]")
{
	using namespace GearTests;
	using namespace GearTests::testproperties;
	using namespace GearTests::badproperties;

	boost::asio::ip::tcp::socket testSocket(testIO);

	// Create a shared state
	std::shared_ptr<Shared_state> sharedState = std::make_shared<Shared_state>("doc_root");

	// Create a WebSocket instance
	WebSocket webSocket(testSocket, sharedState);

	SECTION("Test WebSocket send") {
		// Create a shared string
		std::shared_ptr<const std::string> message = std::make_shared<const std::string>("Hello, WebSocket!");

		// Call the send function
		webSocket.send(message);

		// Assert that the WebSocket's queue contains the sent message
		REQUIRE(webSocket._queue.size() == 1);
		REQUIRE(*(webSocket._queue[0]) == "Hello, WebSocket!");

	}
}
