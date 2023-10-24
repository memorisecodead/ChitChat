#include <catch2/catch_test_macros.hpp>

#include <Network/Listener.hpp>
#include <Tests/NetworkTests/GearTests.hpp>

TEST_CASE("Listener Constuct")
{
	using namespace GearTests;
	using namespace GearTests::testproperties;
	using namespace GearTests::badproperties;

	SECTION("Constuctor with parameters")
	{
		CHECK_NOTHROW
		(
			std::make_shared<Listener>
			(testIO, tcp::endpoint{ netAsio::ip::make_address(testHost), testPort },
			std::make_shared<Shared_state>(testRoot))
			->run()
		);

		netAsio::io_context io;

		CHECK_NOTHROW(std::make_shared<HTTPClient>(io)->run());
		io.run();
	}

	SECTION("Constructor with bad parameters")
	{
		CHECK_NOTHROW
		(
			std::make_shared<Listener>
			(testIO, tcp::endpoint{ netAsio::ip::make_address(badHost), badPort },
			std::make_shared<Shared_state>(badRoot))
			->run()
		);
	}
}
