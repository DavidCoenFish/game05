#include "common/common_pch.h"

#include "common/util/utf8.h"

const std::wstring Utf8::Utf8ToUtf16( const std::string& in_utf8 )
{
   if (in_utf8.empty())
   {
      return std::wstring();
   }
   const size_t num_chars = MultiByteToWideChar(CP_UTF8, 0, in_utf8.c_str(), (int)in_utf8.length(), NULL, 0);
   std::wstring wstrTo;
   if (num_chars)
   {
      wstrTo.resize(num_chars);
      if (MultiByteToWideChar(CP_UTF8, 0, in_utf8.c_str(), (int)in_utf8.length(), wstrTo.data(), (int)num_chars))
      {
         return wstrTo;
      }
   }
   return std::wstring();
}

const std::string Utf8::Utf16ToUtf8( const std::wstring& in_utf16 )
{
   if( in_utf16.empty() ) 
   {
      return std::string();
   }
   const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, in_utf16.c_str(), (int)in_utf16.size(), NULL, 0, NULL, NULL);
   std::string result( sizeNeeded, 0 );
   WideCharToMultiByte(CP_UTF8, 0, in_utf16.c_str(), (int)in_utf16.size(), result.data(), sizeNeeded, NULL, NULL);
   return result;
}
