#include "hshared.h"

namespace hf
{
	uint32_t TrailingZeros64(uint64_t n)
	{
		return n ? __builtin_ctzll(n) : 64u;
	}
}