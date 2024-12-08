#include "application_character_00_pch.h"
#include "build.h"

/*
   including txt files without the build marking itself dirty consecutively seems to work?
   want some way of getting when the build was done into the build with out dirtying the build and forcing a rebuild....
*/

const char* const Build::GetBuildHost( void )
{
   return 
#include "..\..\..\..\engine\version\output\build_host.txt"
	   ;
}

const char* const Build::GetBuildConfiguration(void)
{
	return
#if defined(_DEBUG)
	"DEBUG"
#endif
#if defined(NDEBUG)
	"RELEASE"
#endif
		;
}

const char* const Build::GetBuildPlatform(void)
{
	return "Win64";
}

const char* const Build::GetBuildTime(void)
{
   return 
	   __DATE__ 
	   " "
	   __TIME__
	   ;
}
const char* const Build::GetBuildVersion(void)
{
   return 
#include "..\..\..\..\engine\version\output\build_version.txt"
	   ;
}

const int Build::GetBuildVersionMajor()
{
   return 
#include "..\..\..\..\engine\version\output\build_version_major.txt"
	  ;
}

const int Build::GetBuildVersionMinor()
{
   return 
#include "..\..\..\..\engine\version\output\build_version_minor.txt"
	  ;
}

const int Build::GetBuildVersionPatch()
{
   return 
#include "..\..\..\..\engine\version\output\build_version_patch.txt"
	  ;
}
const int Build::GetBuildVersionStore()
{
   return 
#include "..\..\..\..\engine\version\output\build_version_store.txt"
	  ;
}


