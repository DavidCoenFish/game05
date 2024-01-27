#pragma once
/*
provide some bare bone access to the file system.

see OverlayFileSystem for the intergration of overlays and async functions

*/
namespace FileSystem
{
   typedef std::shared_ptr<std::vector<uint8_t>> TFileData;

   const std::filesystem::path GetModualDir(HINSTANCE in_instance);
   const std::filesystem::path GetTempDir();
   const TFileData SyncReadFile(const std::filesystem::path& in_absolute_path);
   const bool SyncWriteFile(const std::filesystem::path& in_absolute_path, const TFileData& in_data);
   const bool SyncAppendFile(const std::filesystem::path& in_absolute_path, const TFileData& in_data);
   const bool SyncDeleteFile(const std::filesystem::path& in_absolute_path);
   void SyncGatherFiles(std::vector<std::filesystem::path>& out_files_found, const std::filesystem::path& in_absolute_path);
   const std::string DataToString(const TFileData& in_data);

};
