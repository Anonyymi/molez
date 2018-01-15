#ifndef MLIBC_LOG
#define MLIBC_LOG

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

// Detect operating system
#define MLIBC_LOG_OS_WINDOWS 0
#define MLIBC_LOG_OS_LINUX   1

#if defined(_WIN32) || defined(_WIN64)
#define MLIBC_LOG_OS MLIBC_LOG_OS_WINDOWS
#define NOMINMAX
#include <windows.h>
#else
#define MLIBC_LOG_OS MLIBC_LOG_OS_LINUX
#endif

// Logger return codes
#define MLIBC_LOG_CODE_OK       0
#define MLIBC_LOG_CODE_ERR      1
#define MLIBC_LOG_CODE_INIT_ERR 2
#define MLIBC_LOG_CODE_FREE_ERR 3
#define MLIBC_LOG_CODE_LOG_ERR  4
#define MLIBC_LOG_CODE_LOG_NOP  5

// Logger logging levels
typedef enum
{
	MLIBC_LOG_LEVEL_DBG = 0,
	MLIBC_LOG_LEVEL_INF = 1,
	MLIBC_LOG_LEVEL_WRN = 2,
	MLIBC_LOG_LEVEL_ERR = 3
} mlibc_log_level_t;

// Logger data structure
typedef struct
{
	mlibc_log_level_t level;
#if MLIBC_LOG_OS == MLIBC_LOG_OS_WINDOWS
	HANDLE hConsole;
#endif
} mlibc_log_logger;

// Logger instance
extern mlibc_log_logger * mlibc_log_instance;

// Function API
inline int mlibc_log_init(mlibc_log_level_t level);
inline int mlibc_log_free();
inline int mlibc_vlog(mlibc_log_level_t level, const char * msg, va_list args);
inline int mlibc_log(mlibc_log_level_t level, const char * msg, ...);
inline int mlibc_dbg(const char * msg, ...);
inline int mlibc_inf(const char * msg, ...);
inline int mlibc_wrn(const char * msg, ...);
inline int mlibc_err(const char * msg, ...);

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Init logger
// -- * arg: mlibc_log_level_t level
// ----------------------------------------------------------------------------
int mlibc_log_init(mlibc_log_level_t level)
{
	if (mlibc_log_instance != NULL)
	{
		return MLIBC_LOG_CODE_INIT_ERR;
	}

	mlibc_log_instance = (mlibc_log_logger *)malloc(sizeof(*mlibc_log_instance));
	mlibc_log_instance->level = level;
#if MLIBC_LOG_OS == MLIBC_LOG_OS_WINDOWS
	mlibc_log_instance->hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	mlibc_log(MLIBC_LOG_LEVEL_DBG, "mlibc_log_init(instance=%p, level=%d)", mlibc_log_instance, mlibc_log_instance->level);

	return MLIBC_LOG_CODE_OK;
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Free logger
// ----------------------------------------------------------------------------
int mlibc_log_free()
{
	if (mlibc_log_instance == NULL)
	{
		return MLIBC_LOG_CODE_FREE_ERR;
	}

	mlibc_log(MLIBC_LOG_LEVEL_DBG, "mlibc_log_free()");

	free(mlibc_log_instance);
	mlibc_log_instance = NULL;

	return MLIBC_LOG_CODE_OK;
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log generic va_list
// -- * arg: mlibc_log_level_t level
// -- * arg: const char * msg
// -- * arg: va_list args
// ----------------------------------------------------------------------------
int mlibc_vlog(mlibc_log_level_t level, const char * msg, va_list args)
{
	if (mlibc_log_instance == NULL)
	{
		return MLIBC_LOG_CODE_LOG_ERR;
	}

	if (level < mlibc_log_instance->level)
	{
		return MLIBC_LOG_CODE_LOG_NOP;
	}

#if MLIBC_LOG_OS == MLIBC_LOG_OS_WINDOWS
	if (mlibc_log_instance->hConsole == NULL)
	{
		return MLIBC_LOG_CODE_LOG_ERR;
	}

	switch (level)
	{
	case MLIBC_LOG_LEVEL_DBG:
		printf("DBG | ");
		SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case MLIBC_LOG_LEVEL_INF:
		printf("INF | ");
		SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case MLIBC_LOG_LEVEL_WRN:
		printf("WRN | ");
		SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case MLIBC_LOG_LEVEL_ERR:
		printf("ERR | ");
		SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	default:
		printf("??? | ");
		SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	}

	vprintf(msg, args);
	printf("\n");

	SetConsoleTextAttribute(mlibc_log_instance->hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	switch (level)
	{
	case MLIBC_LOG_LEVEL_DBG:
		printf("DBG | ");
		break;
	case MLIBC_LOG_LEVEL_INF:
		printf("INF | ");
		break;
	case MLIBC_LOG_LEVEL_WRN:
		printf("WRN | ");
		break;
	case MLIBC_LOG_LEVEL_ERR:
		printf("ERR | ");
		break;
	default:
		printf("??? | ");
		break;
	}

	vprintf(msg, args);
	printf("\n");
#endif

	return MLIBC_LOG_CODE_OK;
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log generic
// -- * arg: mlibc_log_level_t level
// -- * arg: const char * msg
// -- * arg: va_list ...
// ----------------------------------------------------------------------------
int mlibc_log(mlibc_log_level_t level, const char * msg, ...)
{
	va_list args;
	va_start(args, msg);

	return mlibc_vlog(level, msg, args);

	va_end(args);
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log debug
// -- * arg: const char * msg
// -- * arg: va_list ...
// ----------------------------------------------------------------------------
int mlibc_dbg(const char * msg, ...)
{
	va_list args;
	va_start(args, msg);

	return mlibc_vlog(MLIBC_LOG_LEVEL_DBG, msg, args);

	va_end(args);
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log info
// -- * arg: const char * msg
// -- * arg: va_list ...
// ----------------------------------------------------------------------------
int mlibc_inf(const char * msg, ...)
{
	va_list args;
	va_start(args, msg);

	return mlibc_vlog(MLIBC_LOG_LEVEL_INF, msg, args);

	va_end(args);
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log warning
// -- * arg: const char * msg
// -- * arg: va_list ...
// ----------------------------------------------------------------------------
int mlibc_wrn(const char * msg, ...)
{
	va_list args;
	va_start(args, msg);

	return mlibc_vlog(MLIBC_LOG_LEVEL_WRN, msg, args);

	va_end(args);
}

// ----------------------------------------------------------------------------
// -- Function
// ----------------------------------------------------------------------------
// -- Log error
// -- * arg: const char * msg
// -- * arg: va_list ...
// ----------------------------------------------------------------------------
int mlibc_err(const char * msg, ...)
{
	va_list args;
	va_start(args, msg);

	return mlibc_vlog(MLIBC_LOG_LEVEL_ERR, msg, args);

	va_end(args);
}

#ifdef __cplusplus
}
#endif

#endif // MLIBC_LOG