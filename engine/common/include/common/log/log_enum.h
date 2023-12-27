#pragma once

enum class LogTopic : int
{
   None = 0,
   Info,
   Debug,
   Warning,
   Error,
   Scope,

   Log,
   FileSystem,
   DrawSystem,
   UISystem,
   UIVerbose,
   Render,

   UnitTest0,
   UnitTest1,

   //have a heap of custom enums, allow consumers to rebrand a topic if they want to
   Custom0,
   Custom1,
   Custom2,
   Custom3,
   Custom4,
   Custom5,
   Custom6,
   Custom7,
   Custom8,
   Custom9,

   Count
};

