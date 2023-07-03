#pragma once

namespace Build
{
   const char* const GetBuildTime( void );
   const char* const GetBuildVersion( void );
   const char* const GetBuildDescription( void );

   const int GetBuildVersionMajor();
   const int GetBuildVersionMinor();
   const int GetBuildVersionPatch();
   const int GetBuildVersionStore();

};