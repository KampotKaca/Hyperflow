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
#define log_trace(file, line, ...) log_log(LOG_TRACE, file, line, __VA_ARGS__)
#define log_debug(file, line, ...) log_log(LOG_DEBUG, file, line, __VA_ARGS__)
#define log_info(file, line, ...)  log_log(LOG_INFO,  file, line, __VA_ARGS__)
#define log_warn(file, line, ...)  log_log(LOG_WARN,  file, line, __VA_ARGS__)
#define log_error(file, line, ...) log_log(LOG_ERROR, file, line, __VA_ARGS__)
#define log_fatal(file, line, ...) log_log(LOG_FATAL, file, line, __VA_ARGS__)

#define LOG_TRACE(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define LOG_LOG(fmt, ...) log_simple(fmt, __VA_ARGS__)

#else
#define log_trace(...)
#define log_debug(...)
#define log_info(...)
#define log_warn(...)
#define log_error(...)
#define log_fatal(...)

#define LOG_TRACE(...)
#define LOG_DEBUG(...)
#define LOG_INFO(...) 
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_FATAL(...)

#define LOG_LOG(fmt, ...)
#endif
const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

void log_log(int level, const char *file, int line, const char *fmt, ...);
void log_simple(const char *fmt, ...);
#endif
