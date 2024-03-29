#pragma once

#define ARRAY_LITERAL_SIZE(DATA) (sizeof(DATA)/sizeof(DATA[0]))

#define TOKEN_PAIR(TOKEN) TOKEN,#TOKEN
#define ENUM_TOKEN_PAIR(ENUM, TOKEN) ENUM::TOKEN,#TOKEN

#define DSC_CONDITION_THROW(CONDITION, MESSAGE) if (CONDITION) { throw std::exception(MESSAGE); }

#if defined(_DEBUG)
   #define DSC_ASSERT(CONDITION, MESSAGE) assert(MESSAGE && (CONDITION))
   #define DSC_ASSERT_ALWAYS(MESSAGE) assert(MESSAGE && false)
#else
   //#define DSC_ASSERT(CONDITION, MESSAGE) (void)0
   //#define DSC_ASSERT_ALWAYS (void)0
   #define DSC_ASSERT(CONDITION, MESSAGE) __noop
   #define DSC_ASSERT_ALWAYS __noop
#endif

#define DSC_TODO(MESSAGE, ...) DSC_ASSERT_ALWAYS(MESSAGE)

#define DSC_PI 3.1415926535897932384626433832795f
#define DSC_PI_DIV_180 0.01745329251994329576923690768489f
#define DSC_180_DIV_PI 57.295779513082320876798154814105f

