#include "common/common_pch.h"

#include "common/util/hash.h"

const THashKey Hash::AppendHash(const std::string& in_value, const THashKey in_hash)
{
	const THashKey hash = std::hash<std::string>{}(in_value);
	return in_hash ^ (hash << 1);
}

const THashKey Hash::AppendHash(const int in_value, const THashKey in_hash)
{
	return in_hash ^ (in_value << 1);
}