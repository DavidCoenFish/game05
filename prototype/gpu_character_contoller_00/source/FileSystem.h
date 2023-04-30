#pragma once

namespace FileSystem
{
	const std::shared_ptr<std::vector< uint8_t >> SyncReadFile(const std::filesystem::path& absolutePath);

}
