set(TARGET Network)
set(OUTPUT_NAME Network)

set(HEADER_FILES
	BeastAttribute.hpp
	WebSocket.hpp
	HTTPSession.hpp	
	Listener.hpp
	)

add_library(${TARGET} INTERFACE)

target_link_libraries(${TARGET} INTERFACE  ${Boost_INCLUDE_DIRS})

target_include_directories(${TARGET} INTERFACE  ${Boost_INCLUDE_DIRS})
