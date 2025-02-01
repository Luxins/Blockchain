#pragma once
// System headers
#include <string>
#include <sstream>
#include <array>
#include <iostream>
#include <ostream>

// Third party headers:
#include <openssl/sha.h>

struct	Block {
	Block();
	static long long getCurrentTimestamp();
	std::string	data;
	int	Index;
	std::array<unsigned char, SHA_DIGEST_LENGTH>	current_hash;
	std::array<unsigned char, SHA_DIGEST_LENGTH>	previous_hash;
	long long	timestamp;

	// Helpers
	static std::string serialize(const Block&	block);

	static void	computeHash(const std::string& data, std::array<unsigned char, SHA_DIGEST_LENGTH>&	dest);
};