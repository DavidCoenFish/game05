#pragma once

typedef std::size_t THashKey;

// https://en.cppreference.com/w/cpp/utility/hash
namespace Hash
{
	const THashKey AppendHash(const std::string& in_value, const THashKey in_hash = 0);
	const THashKey AppendHash(const int in_value, const THashKey in_hash = 0);

};
