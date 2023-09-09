set(TARGET Network)
set(OUTPUT_NAME Network)

set(SOURCE_FILES
	BeastAttribute.hpp
	WebSocket.hpp
	WebSocket.cpp
	HTTPSession.hpp
	HTTPSession.cpp
	SharedState.hpp
	SharedState.cpp
	Listener.hpp
	Listener.cpp)
	
add_library(${TARGET} STATIC ${SOURCE_FILES})

target_link_libraries(${TARGET} PRIVATE ${EXTRA_LIBS} Boost::boost)
