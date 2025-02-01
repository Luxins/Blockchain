// Custom headers:
#include "Block.hpp"

// System headers:
#include <iomanip>

// Overloads:
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

Block::Block() {
	timestamp = Block::getCurrentTimestamp();
	Index = -1;
}
long long Block::getCurrentTimestamp() {
	std::chrono::system_clock::time_point point = std::chrono::system_clock::now();
	std::chrono::microseconds microsenconds_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(point.time_since_epoch());
	return microsenconds_since_epoch.count();
}

// Helpers
std::string Block::serialize(const Block&	block)
{
	std::stringstream	ss;
	ss	<< "|" << block.data
		<< "|" << block.Index
		<< "|" << block.previous_hash
		<< "|" << block.timestamp;
	return ss.str();
}

void	Block::computeHash(const std::string& data, std::array<unsigned char, SHA_DIGEST_LENGTH>&	dest)
{
	SHA1(reinterpret_cast<const unsigned char *>(data.c_str()), data.size(),
		std::begin(dest));
}

