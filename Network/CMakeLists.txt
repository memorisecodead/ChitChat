set(TARGET Network)
set(OUTPUT_NAME Network)

set(SOURCE_FILES
	BeastAttribute.hpp
	WebSocket.hpp
	HTTPSession.hpp
	Listener.hpp)
	
add_library(${TARGET} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET} PRIVATE ${EXTRA_LIBS} Boost::boost)
