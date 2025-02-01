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

std::array<unsigned char, SHA_DIGEST_LENGTH>	NullHash;
inline void	init_hash(std::array<unsigned char, SHA_DIGEST_LENGTH>	&hashArr)
{
	std::fill(std::begin(hashArr), std::end(hashArr), 0x00);
}

std::ostream&	operator<<(std::ostream& os, const std::array<unsigned char, SHA_DIGEST_LENGTH>& arr)
{
	// Converting arr to hex
	os << std::hex << std::setfill('0');
	for (auto byte : arr)
	{
		os << std::setw(2) << static_cast<int>(byte);
	}
	return os;
};

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

struct	Block {
	Block() {
		timestamp = Block::getCurrentTimestamp();
		Index = -1;
	}
	static long long getCurrentTimestamp() {
		std::chrono::system_clock::time_point point = std::chrono::system_clock::now();
		std::chrono::microseconds microsenconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(point.time_since_epoch());
		return microsenconds_since_epoch.count();
	}
	std::string	data;
	int	Index;
	std::array<unsigned char, SHA_DIGEST_LENGTH>	current_hash;
	std::array<unsigned char, SHA_DIGEST_LENGTH>	previous_hash;
	long long	timestamp;

	// Helpers
	static std::string serialize(const Block&	block)
	{
		std::stringstream	ss;
		ss	<< "|" << block.data
			<< "|" << block.Index
			<< "|" << block.previous_hash
			<< "|" << block.timestamp;
		return ss.str();
	}

	static void	computeHash(const std::string& data, std::array<unsigned char, SHA_DIGEST_LENGTH>&	dest)
	{
		SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(),
			std::begin(dest));
	}
};

struct	Blockchain {
	std::function<void(Block&)>	postAddBlockHook;

	Blockchain() : primitive() {
	}
	std::vector<Block>	primitive;
	void	addBlock(Block &new_Block) {
		// Copy over the hashes to the new block and compute the blocks own hash
		if (primitive.empty())
			init_hash(new_Block.previous_hash);
		else
			std::copy(std::begin(primitive.back().current_hash), std::end(primitive.back().current_hash), std::begin(new_Block.previous_hash));
		
		// Copy the hashable content of the block into a new memory location and then hash it
		const std::string serialized_content = Block::serialize(new_Block);
		Block::computeHash(serialized_content, new_Block.current_hash);
		if (postAddBlockHook)
		{
			postAddBlockHook(new_Block);
		}
		primitive.emplace_back(new_Block);
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
	init_hash(NullHash);

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
		std::fill(std::begin(new_Block.previous_hash), std::end(new_Block.previous_hash), 0xFF);
	};

	corrupted_chain.addBlock(current);
	testCaseRunner(std::bitset<NUM_FAILURES>().set(static_cast<std::size_t>(FailureDimensions::CorruptedGenesis)), isBlockchainValid, corrupted_chain);

	// Test case 3: two blocks, but both hashes are initalised to 0xFF...
	corrupted_chain.addBlock(current1);
	testCaseRunner(std::bitset<NUM_FAILURES>().set(static_cast<std::size_t>(FailureDimensions::CorruptedGenesis)).set(static_cast<std::size_t>(FailureDimensions::CorruptedLink)), isBlockchainValid, corrupted_chain);

	return 0;
}
