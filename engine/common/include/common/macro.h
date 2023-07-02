#pragma once

#define ARRAY_LITERAL_SIZE(DATA) (sizeof(DATA)/sizeof(DATA[0]))

#define TOKEN_PAIR(TOKEN) TOKEN,#TOKEN
#define ENUM_TOKEN_PAIR(ENUM, TOKEN) ENUM::TOKEN,#TOKEN

#define DSC_CONDITION_THROW(CONDITION, MESSAGE) if (CONDITION) { throw std::exception(MESSAGE); }


#if defined(_DEBUG)
   #define DSC_ASSERT(CONDITION, MESSAGE) assert(MESSAGE && (CONDITION))
   #define DSC_ASSERT_ALWAYS(MESSAGE) assert(MESSAGE && false)
#else
   #define DSC_ASSERT(CONDITION) (void)0
   #define DSC_ASSERT_ALWAYS (void)0
#endif

