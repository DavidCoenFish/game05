#include "common/common_pch.h"

#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/util/utf8.h"

#define MAX_PATH_LENGTH 1024 
const std::filesystem::path FileSystem::GetModualDir(HINSTANCE in_instance)
{
	wchar_t data[MAX_PATH_LENGTH];
	const auto length = GetModuleFileNameW(in_instance, &data[0], MAX_PATH_LENGTH);
	if (length == MAX_PATH_LENGTH)
	{
		return std::string();
	}
	std::string exePath = Utf8::Utf16ToUtf8(data);
	std::filesystem::path path(exePath);
	path.remove_filename();
	return path;
}

const std::filesystem::path FileSystem::GetTempDir()
{
	return std::filesystem::temp_directory_path();
}

// based on code Copyright (c) Microsoft Corporation. All rights reserved. ReadData.h
const FileSystem::TFileData FileSystem::SyncReadFile(const std::filesystem::path& absolutePath)
{
    LOG_MESSAGE_FILESYSTEM("FileSystem::SyncReadFile:%s", absolutePath.string().c_str());

	std::ifstream inFile(absolutePath, std::ios::in | std::ios::binary | std::ios::ate);
	auto blob = std::make_shared< std::vector< uint8_t > >();
   
    if (!inFile || inFile.fail() || false == inFile.is_open())
    {
        LOG_MESSAGE_FILESYSTEM("  file:%d fail():%d is_open():%d", inFile ? 1 : 0, inFile.fail() ? 1 : 0, inFile.is_open() ? 1 : 0);
        return blob;
    }
    DSC_CONDITION_THROW(!inFile || inFile.fail() || false == inFile.is_open(), "Failed to open file");

	std::streampos len = inFile.tellg();

    DSC_CONDITION_THROW(len < 0 || !inFile || inFile.fail(), "Failed to open file");

	blob->resize(size_t(len));

	inFile.seekg(0, std::ios::beg);

    DSC_CONDITION_THROW(!inFile || inFile.fail(), "Failed to open file");

	inFile.read(reinterpret_cast<char*>(blob->data()), len);

    DSC_CONDITION_THROW(!inFile || inFile.fail(), "Failed to open file");

	inFile.close();

	return blob;
}

const bool FileSystem::SyncWriteFile(const std::filesystem::path& absolutePath, const TFileData& pData)
{
	std::ofstream outFile(absolutePath, std::ios::out | std::ios::binary | std::ios::ate);
	if (!outFile)
	{
		throw std::exception("SyncWriteFile");
	}

	outFile.write(reinterpret_cast<char*>(pData->data()), pData->size());
	if (!outFile)
	{
		throw std::exception("SyncWriteFile");
	}

	outFile.close();

	return true;
}

const bool FileSystem::SyncAppendFile(const std::filesystem::path& absolutePath, const TFileData& pData)
{
	std::ofstream outFile(absolutePath, std::ios::out | std::ios::binary | std::ios::app);

	if (!outFile)
	{
		throw std::exception("SyncWriteFile");
	}

	outFile.write(reinterpret_cast<char*>(pData->data()), pData->size());
	if (!outFile)
	{
		throw std::exception("SyncWriteFile");
	}

	outFile.close();

	return true;
}

const bool FileSystem::SyncDeleteFile(const std::filesystem::path& absolutePath)
{
	absolutePath;
	return true;
}

void FileSystem::SyncGatherFiles(std::vector<std::filesystem::path>& filesFound, const std::filesystem::path& absolutePath)
{
	filesFound;
	absolutePath;
	return;
}

const std::string FileSystem::DataToString(const TFileData& pData)
{
	if (nullptr == pData)
	{
		return std::string();
	}
	//this is naughty, but convienient. just append the input data with a terminating zero. better would be to have a seperate func to append zero?
	pData->push_back(0);
	return std::string((const char* const)pData->data());
}
