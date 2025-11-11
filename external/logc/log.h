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
#define log_tracei(file, line, ...) log_base(LOG_TRACE, file, line, __VA_ARGS__)
#define log_debugi(file, line, ...) log_base(LOG_DEBUG, file, line, __VA_ARGS__)
#define log_infoi(file, line, ...)  log_base(LOG_INFO,  file, line, __VA_ARGS__)
#define log_warni(file, line, ...)  log_base(LOG_WARN,  file, line, __VA_ARGS__)
#define log_errori(file, line, ...) log_base(LOG_ERROR, file, line, __VA_ARGS__)
#define log_fatali(file, line, ...) log_base(LOG_FATAL, file, line, __VA_ARGS__)

#define log_trace(...) log_base(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_base(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_base(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_base(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_base(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_base(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define log_log(fmt, ...) log_simple(fmt, __VA_ARGS__)

#else
#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)

#define log_log(fmt, ...)
#endif
const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_base(int level, const char *file, int line, const char *fmt, ...);
void log_simple(const char *fmt, ...);
#endif
