# Defining the library
add_library(BlockchainLib
	Block.cpp
	Blockchain.cpp
)

# public interface for headers
target_include_directories(BlockchainLib PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
	/opt/homebrew/Cellar/openssl@3/3.3.2/include
)

# link openssl publicly
target_link_libraries(BlockchainLib PUBLIC
	/opt/homebrew/Cellar/openssl@3/3.3.2/lib/libssl.a
	/opt/homebrew/Cellar/openssl@3/3.3.2/lib/libcrypto.a
)
