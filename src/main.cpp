// System headers:
#include <map>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <optional>
#include <sstream>
#include <array>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
#include <ctime>
#include <iostream>
#include <bitset>
#include <ostream>
#include <iomanip>

// Third party headers:
#include <openssl/sha.h>

// Custom headers:
#include "Block.hpp"
#include "Blockchain.hpp"

constexpr std::array<unsigned char, SHA_DIGEST_LENGTH>	NullHash = {};


enum class	FailureDimensions : std::size_t
{
    CorruptedGenesis,
    CorruptedLink,
    COUNT // Sentinel value for the number of dimensions
};
static constexpr size_t	NUM_FAILURES = static_cast<size_t>(FailureDimensions::COUNT);

struct	TestCounter
{
	static int &get()
	{
		static int counter = 0;
		return counter;
	}
};

std::bitset<NUM_FAILURES>	isBlockchainValid(const Blockchain&	blockchain)
{
	std::bitset<NUM_FAILURES>	failureFlags;

	std::optional<std::array<unsigned char, SHA_DIGEST_LENGTH>>	hashPreviousBlock;
	for (const Block& block : blockchain.primitive)
	{
		std::array<unsigned char, SHA_DIGEST_LENGTH>	currentHash;
		std::copy(std::begin(block.current_hash), std::end(block.current_hash), currentHash.begin());
		std::array<unsigned char, SHA_DIGEST_LENGTH>	backrefHashCurrentBlock;
		std::copy(std::begin(block.previous_hash), std::end(block.previous_hash), backrefHashCurrentBlock.begin());
		if (!hashPreviousBlock)
		{
			if (backrefHashCurrentBlock != NullHash)
			{
				failureFlags.set(static_cast<size_t>(FailureDimensions::CorruptedGenesis), true);
			}
		}
		else if (hashPreviousBlock != backrefHashCurrentBlock)
		{
			failureFlags.set(static_cast<size_t>(FailureDimensions::CorruptedLink), true);
		}
		hashPreviousBlock = currentHash;
	}
	return (failureFlags);
}



template <typename CALLABLE, typename... ARGS, typename RETURN_TYPE = std::invoke_result_t<CALLABLE, ARGS...>>
void	runTestCase(RETURN_TYPE expected, CALLABLE &&test, ARGS&&... args)
{
	int		&testNum = TestCounter::get();
	auto	result = std::forward<CALLABLE>(test)(std::forward<ARGS>(args)...);
	bool	succeeded = (expected == result);
	std::cout	<< (succeeded ? "\033[32m" : "\033[31m")
				<< "Test " << testNum <<
				(succeeded ? " succeeded" : " failed")
				<< "\033[0m\n";
	++testNum;
}


int main(void)
{
	auto	expectedFailures = [](std::initializer_list<FailureDimensions>	dims)
	{
		std::bitset<NUM_FAILURES>	bits;
		for (auto dim : dims)
		{
			bits.set(static_cast<size_t>(dim));
		}
		return bits;
	};

	struct	testCaseInformation
	{
		size_t											numBlocks;
		std::bitset<NUM_FAILURES>						expectedFailures;
		std::optional<std::function<void(Block&)>>		postAddBlockHook;
	};
	std::vector<testCaseInformation>	testCases;

	auto blockCorruptionHook = 	[](Block &new_Block)
	{
		std::fill(std::begin(new_Block.previous_hash), std::end(new_Block.previous_hash), 0xFF);
	};

	testCases.push_back({
		1,
		expectedFailures({})
	});

	testCases.push_back({
		2,
		expectedFailures({})
	});

	testCases.push_back({
		1,
		expectedFailures({FailureDimensions::CorruptedGenesis}),
		blockCorruptionHook
	});

	testCases.push_back({
	2,
	expectedFailures({FailureDimensions::CorruptedGenesis, FailureDimensions::CorruptedLink}),
	blockCorruptionHook
	});

	for (auto testCase : testCases)
	{
		Blockchain	blockchain;
		if (testCase.postAddBlockHook)
		{
			blockchain.postAddBlockHook = testCase.postAddBlockHook.value();
		}
		for (size_t blockNum = 0; blockNum < testCase.numBlocks; ++blockNum)
		{
			blockchain.addBlock(Block());
		}
		runTestCase(testCase.expectedFailures, isBlockchainValid, blockchain);
	}

	return 0;
}
