add_executable(tests
	test_blockchain.cpp
	helpers.cpp
)

target_link_libraries(tests PRIVATE BlockchainLib Catch2::Catch2WithMain)

# Find the tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(tests)
