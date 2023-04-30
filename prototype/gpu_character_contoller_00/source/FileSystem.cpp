#include "pch.h"
#include "FileSystem.h"

// based on code Copyright (c) Microsoft Corporation. All rights reserved. ReadData.h
const std::shared_ptr<std::vector< uint8_t >> FileSystem::SyncReadFile(const std::filesystem::path& absolutePath)
{
	std::ifstream inFile(absolutePath, std::ios::in | std::ios::binary | std::ios::ate);
	auto pBlob = std::make_shared< std::vector< uint8_t > >();
   
	if ((!inFile) || (inFile.fail()))
	{
		throw std::exception("SyncReadFile");
		//return pBlob;
	}

	std::streampos len = inFile.tellg();
	if (!inFile)
	{
		throw std::exception("SyncReadFile");
	}

	pBlob->resize(size_t(len));

	inFile.seekg(0, std::ios::beg);
	if (!inFile)
	{
		throw std::exception("SyncReadFile");
	}

	inFile.read(reinterpret_cast<char*>(pBlob->data()), len);
	if (!inFile)
	{
		throw std::exception("SyncReadFile");
	}

	inFile.close();

	return pBlob;
}
