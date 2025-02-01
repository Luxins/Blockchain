// Custom headers:
#include "Blockchain.hpp"

Blockchain::Blockchain() : primitive() {
}

void	Blockchain::addBlock(Block &&new_Block) {
	// Copy over the hashes to the new block and compute the blocks own hash
	if (primitive.empty())
		new_Block.previous_hash = {};
	else
		std::copy(std::begin(primitive.back().current_hash), std::end(primitive.back().current_hash), std::begin(new_Block.previous_hash));
	
	// Serialize block and store hash in it
	const std::string serialized_content = Block::serialize(new_Block);
	Block::computeHash(serialized_content, new_Block.current_hash);
	if (postAddBlockHook)
	{
		postAddBlockHook(new_Block);
	}
	primitive.emplace_back(new_Block);
}