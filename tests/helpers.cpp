// Costume headers:
#include "Blockchain.hpp"
#include "helpers.hpp"

constexpr std::array<unsigned char, SHA_DIGEST_LENGTH>	NullHash = {};

std::bitset<NUM_FAILURES>	expectedFailures(std::initializer_list<FailureDimensions>	dims)
{
	std::bitset<NUM_FAILURES>	bits;
	for (auto dim : dims)
	{
		bits.set(static_cast<size_t>(dim));
	}
	return bits;
};

void blockCorruptionHook(Block &new_Block)
{
	std::fill(std::begin(new_Block.previous_hash), std::end(new_Block.previous_hash), 0xFF);
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
