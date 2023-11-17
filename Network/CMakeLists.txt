set(TARGET Network)
set(OUTPUT_NAME Network)

set(HEADER_FILES
	BeastAttribute.hpp
	WebSocket.hpp
	HTTPSession.hpp	
	Listener.hpp
	)

add_library(${TARGET} PUBLIC )

target_link_libraries(${TARGET} PUBLIC Boost::boost)

target_include_directories(${TARGET} PUBLIC Boost::boost)
