#pragma once
// System headers:
#include <bitset>
#include <array>
#include <bitset>
#include <optional>
#include <algorithm>

enum class	FailureDimensions : std::size_t
{
    CorruptedGenesis,
    CorruptedLink,
    COUNT // Sentinel value for the number of dimensions
};

static constexpr size_t	NUM_FAILURES = static_cast<size_t>(FailureDimensions::COUNT);

std::bitset<NUM_FAILURES>	isBlockchainValid(const Blockchain&	blockchain);