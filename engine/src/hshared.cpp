#include "hshared.h"

namespace hf
{
	uint32_t TrailingZeros64(uint64_t n)
	{
		return n ? __builtin_ctzll(n) : 64u;
	}

	uint32_t TrailingZeros128(__int128 n)
	{
		if (n == 0) return 128u; // Special case for zero
		uint64_t low = (uint64_t)n;
		uint64_t high = (uint64_t)(n >> (__int128)64);

		if (low != 0) return __builtin_ctzll(low);
		else return (__int128)64 + __builtin_ctzll(high);
	}
}