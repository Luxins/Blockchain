# Defining a target
add_executable(Blockchain main.cpp)

# headers of data structures and openssl
target_include_directories(Blockchain PUBLIC 
	# ${CMAKE_CURRENT_SOURCE_DIR}/data_structures
	/opt/homebrew/Cellar/openssl@3/3.3.2/include
)
# Link openssl
target_link_libraries(Blockchain PRIVATE
	/opt/homebrew/Cellar/openssl@3/3.3.2/lib/libssl.a
	/opt/homebrew/Cellar/openssl@3/3.3.2/lib/libcrypto.a
	# DataStructures
)
# install(TARGETS Blockchain DESTINATION ${CMAKE_SOURCE_DIR}/bin)