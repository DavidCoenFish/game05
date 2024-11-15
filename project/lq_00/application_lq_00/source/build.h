#pragma once

namespace Build
{
   const char* const GetBuildTime( void );
   const char* const GetBuildVersion( void );
   const char* const GetBuildHost(void);
   const char* const GetBuildConfiguration(void);
   const char* const GetBuildPlatform( void );

   const int GetBuildVersionMajor();
   const int GetBuildVersionMinor();
   const int GetBuildVersionPatch();
   const int GetBuildVersionStore();

};