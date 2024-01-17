#pragma once

typedef enum {INFO, ERROR, CRITICAL} LOG_LEVEL;

void logger(LOG_LEVEL level, const char* module, const char* msg);
void logger_info(const char* module, const char* msg);
void logger_error(const char* module, const char* msg);
void logger_critical(const char* module, const char* msg);
