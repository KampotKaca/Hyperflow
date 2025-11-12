/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#define ALWAYS_LOG true

#include <stdio.h>

#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#if !RELEASE || ALWAYS_LOG
#define log_trace_i(file, line, ...) log_log(LOG_TRACE, file, line, __VA_ARGS__)
#define log_debug_i(file, line, ...) log_log(LOG_DEBUG, file, line, __VA_ARGS__)
#define log_info_i(file, line, ...)  log_log(LOG_INFO,  file, line, __VA_ARGS__)
#define log_warn_i(file, line, ...)  log_log(LOG_WARN,  file, line, __VA_ARGS__)
#define log_error_i(file, line, ...) log_log(LOG_ERROR, file, line, __VA_ARGS__)
#define log_fatal_i(file, line, ...) log_log(LOG_FATAL, file, line, __VA_ARGS__)

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define log_trace_s(...) log_log(LOG_TRACE, NULL, 0, __VA_ARGS__)
#define log_debug_s(...) log_log(LOG_DEBUG, NULL, 0, __VA_ARGS__)
#define log_info_s(...)  log_log(LOG_INFO,  NULL, 0, __VA_ARGS__)
#define log_warn_s(...)  log_log(LOG_WARN,  NULL, 0, __VA_ARGS__)
#define log_error_s(...) log_log(LOG_ERROR, NULL, 0, __VA_ARGS__)
#define log_fatal_s(...) log_log(LOG_FATAL, NULL, 0, __VA_ARGS__)

#else
#define log_tracei(file, line, ...)
#define log_debugi(file, line, ...)
#define log_infoi(file, line, ...)
#define log_warni(file, line, ...)
#define log_errori(file, line, ...)
#define log_fatali(file, line, ...)

#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)

#define log_trace_s(...)
#define log_debug_s(...)
#define log_info_s(...)
#define log_warn_s(...)
#define log_error_s(...)
#define log_fatal_s(...)

#endif
const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);
#endif
