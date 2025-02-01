#include <map>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <optional>
#include <sstream>
#include <array>
#include <openssl/sha.h>
#include <vector>
#include <string>
#include <string.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <bitset>

std::array<char, SHA_DIGEST_LENGTH>	NullHash;
#define NULL_HASH_INIT(HASH_ARR)	std::fill(std::begin(HASH_ARR), std::end(HASH_ARR), 0x00)

enum	FailureDimensions : std::size_t
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

struct	Block {
	Block() {
		timestamp = Block::getCurrentTimestamp();
		Index = -1;
	}
	int	Index;
	static long long getCurrentTimestamp() {
		std::chrono::system_clock::time_point point = std::chrono::system_clock::now();
		std::chrono::microseconds microsenconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(point.time_since_epoch());
		return microsenconds_since_epoch.count();
	}
	long long	timestamp;
	std::string	data;
	unsigned char	current_hash[SHA_DIGEST_LENGTH];
	unsigned char	previous_hash[SHA_DIGEST_LENGTH];
};

struct	Blockchain {
	std::function<void(Block&)>	postAddBlockHook;

	Blockchain() : primitive() {
	}
	std::vector<Block>	primitive;
	void	addBlock(Block &new_Block) {
		// Copy over the hashes to the new block and compute the blocks own hash
		if (primitive.empty())
			NULL_HASH_INIT(new_Block.previous_hash);
		else
			memcpy(new_Block.previous_hash, primitive.back().current_hash, sizeof(new_Block.previous_hash));
		
		// Copy the hashable content of the block into a new memory location and then hash it
		std::stringstream	serialised_hashable_content_stream;
		serialised_hashable_content_stream << "|" << new_Block.data << "|" << new_Block.Index << "|" << new_Block.previous_hash << "|" << new_Block.timestamp;
		std::string serialised_hashable_content = serialised_hashable_content_stream.str();
		SHA1(reinterpret_cast<const u_int8_t *>(serialised_hashable_content.c_str()), sizeof(Block) - sizeof(Block::current_hash), new_Block.current_hash);
		if (postAddBlockHook)
		{
			postAddBlockHook(new_Block);
		}
		primitive.emplace_back(new_Block);
	}
};

std::bitset<NUM_FAILURES>	isBlockchainValid(Blockchain blockchain)
{
	std::bitset<NUM_FAILURES>	failureFlags;

	std::optional<std::array<char, SHA_DIGEST_LENGTH>>	hashPreviousBlock;
	for (Block& block : blockchain.primitive)
	{
		std::array<char, SHA_DIGEST_LENGTH>	currentHash;
		std::copy(std::begin(block.current_hash), std::end(block.current_hash), currentHash.begin());
		std::array<char, SHA_DIGEST_LENGTH>	backrefHashCurrentBlock;
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
void	runTestCase(RETURN_TYPE expectation, CALLABLE &&test, ARGS&&... args)
{
	int &testNrBinding = TestCounter::get();
	RETURN_TYPE	reality = std::forward<CALLABLE>(test)(std::forward<ARGS>(args)...);
	bool succeeded = expectation == reality;
	std::cout << (succeeded ? "\033[32m" : "\033[31m") << "Test " << testNrBinding << (succeeded ? " succeeded" : " failed") << "\033[0m\n";
	++testNrBinding;
}


int main(void)
{
	// Initialise the NullHashGlobal
	NULL_HASH_INIT(NullHash);

	auto testCaseRunner = runTestCase<decltype(isBlockchainValid), Blockchain&>;
	Blockchain	chain = Blockchain();
	// Test case 0: only one block
	Block	current = Block();

	chain.addBlock(current);
	testCaseRunner(std::bitset<NUM_FAILURES>(), isBlockchainValid, chain);
	
	// Test case 1: two blocks
	Block	current1 = Block();

	chain.addBlock(current1);
	bool	test1_should = true;
	testCaseRunner(std::bitset<NUM_FAILURES>(), isBlockchainValid, chain);

	// Test case 2: one block, but the hash is initalised to 0xFF...
	Blockchain corrupted_chain = Blockchain();
	corrupted_chain.postAddBlockHook = [](Block &new_Block)
	{
		memset(new_Block.previous_hash, 0xFF, SHA_DIGEST_LENGTH);
	};

	corrupted_chain.addBlock(current);
	testCaseRunner(std::bitset<NUM_FAILURES>().set(FailureDimensions::CorruptedGenesis), isBlockchainValid, corrupted_chain);

	// Test case 3: two blocks, but both hashes are initalised to 0xFF...
	corrupted_chain.addBlock(current1);
	testCaseRunner(std::bitset<NUM_FAILURES>().set(FailureDimensions::CorruptedGenesis).set(FailureDimensions::CorruptedLink), isBlockchainValid, corrupted_chain);

	return 0;
}
