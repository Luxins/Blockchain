// Third party headers:
#include <catch2/catch_test_macros.hpp>

// Costume headers:
#include "Blockchain.hpp"
#include "helpers.hpp"

TEST_CASE("Blockchain with one valid block", "[blockchain]")
{
	Blockchain	blockchain;
	blockchain.addBlock(Block());
	auto failures = isBlockchainValid(blockchain);
	REQUIRE(failures.none());
}

