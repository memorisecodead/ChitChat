set(TARGET NetworkTests)
set(OUTPUT_NAME NetworkTests)

set(SOURCE_FILES NetworkTest.cpp)

add_executable(${TARGET} ${SOURCE_FILES})

set_target_properties(${TARGET} PROPERTIES FOLDER ${FOLDER})

target_link_libraries(${TARGET} 
						PRIVATE 
						Catch2::Catch2WithMain
						Boost::boost)

target_include_directories(${TARGET} 
							PRIVATE 
							$<TARGET_PROPERTY:Network,SOURCE_DIR>/..)

add_test(NAME ${OUTPUT_NAME} COMMAND ${TARGET})

install(TARGETS ${TARGET} RUNTIME DESTINATION ${TEST_INSTALL_DIR})
