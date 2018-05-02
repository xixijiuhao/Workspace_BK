#pragma once


#define IS_OUT(right, format, ...) is_out(right, ""format" at %s(%d): func %s", ##__VA_ARGS__, __FILE__, __LINE__, __FUNCTION__)  

bool is_out(bool right, const char * fmt, ...);

