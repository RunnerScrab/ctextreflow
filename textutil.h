#ifndef TEXTUTIL_H_
#define TEXTUTIL_H_
#include <stdarg.h>
#include "charvector.h"

typedef struct reflow_strarray
{
	cv_t* strings;
	size_t length;
	size_t capacity;
	unsigned char* hyphenpoints;
	unsigned char* escapedwords;
} reflow_strarray_t;

int reflow_strarray_create(reflow_strarray_t* array, size_t initial_size);
void reflow_strarray_push(reflow_strarray_t* array, char* val, unsigned char bIsEscaped, unsigned char bIsHyphenPoint);
void reflow_strarray_destroy(reflow_strarray_t* array);

typedef struct reflow_intstack
{
	int* data;
	size_t length;
	size_t capacity;
} reflow_intstack_t;

void reflow_intstack_create(struct reflow_intstack* stack, size_t initial);
int reflow_intstack_pop(struct reflow_intstack* stack);
int reflow_intstack_popleft(struct reflow_intstack* stack);
int reflow_intstack_peek(struct reflow_intstack* stack);
void reflow_intstack_clear(struct reflow_intstack* stack);
void reflow_intstack_push(struct reflow_intstack* stack, int val);
void reflow_intstack_destroy(struct reflow_intstack* stack);

void FindParagraphs(const char* text, size_t length, struct reflow_intstack* paragraphlocs);
void TokenizeString(const char* input, size_t inputlen, reflow_strarray_t* out);
void ReflowParagraph(const char* text, size_t len, const int width, cv_t* output, unsigned char par_indent_count);
void ReflowParagraphBinary(const char* text, size_t len, const int width, cv_t* output, unsigned char par_indent_count);
void StripNewline(const char* input, size_t inputlen, char* out, size_t bufferlen);
#endif
