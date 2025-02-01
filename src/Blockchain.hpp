#pragma once
// System headers:
#include <functional>
#include <vector>

// Custom headers:
#include "Block.hpp"
struct	Blockchain {
	std::function<void(Block&)>	postAddBlockHook;

	Blockchain();
	std::vector<Block>	primitive;
	void	addBlock(Block &&new_Block);
};