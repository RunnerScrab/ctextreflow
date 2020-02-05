#ifndef TEXTUTIL_H_
#define TEXTUTIL_H_
#include <stdarg.h>
#include "charvector.h"

//O(n * log(n)), generally performs better than the shortest-paths algorithm - performs better on processors with larger L1 cache
void ReflowTextBinary(const char* input, const size_t len, cv_t* output, const int width, unsigned char num_indent_spaces);

//O(width * n), can occasionally perform better than the binary-search algorithm - performs better on processors with smaller L1 cache
void ReflowText(const char* input, const size_t len, cv_t* output, const int width, unsigned char num_indent_spaces);

#endif
