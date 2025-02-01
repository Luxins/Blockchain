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

TEST_CASE("Blockchain with two valid blocks", "[blockchain]")
{
	Blockchain	blockchain;
	blockchain.addBlock(Block());
	blockchain.addBlock(Block());
	auto failures = isBlockchainValid(blockchain);
	REQUIRE(failures.none());
}

TEST_CASE("Blockchain with corrupted genesis hash", "[blockchain]")
{
	Blockchain	blockchain;
	blockchain.postAddBlockHook = blockCorruptionHook;
	blockchain.addBlock(Block());
	auto failures = isBlockchainValid(blockchain);
	REQUIRE(failures == expectedFailures({FailureDimensions::CorruptedGenesis}));
}

TEST_CASE("Blockchain with two corrupted blocks", "[blockchain]")
{
	Blockchain	blockchain;
	blockchain.postAddBlockHook = blockCorruptionHook;
	blockchain.addBlock(Block());
	blockchain.addBlock(Block());
	auto failures = isBlockchainValid(blockchain);
	REQUIRE(failures == expectedFailures({FailureDimensions::CorruptedGenesis, FailureDimensions::CorruptedLink}));
}
