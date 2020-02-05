#ifndef TEXTUTIL_H_
#define TEXTUTIL_H_
#include <stdarg.h>
#include "charvector.h"

typedef struct strarray
{
	cv_t* strings;
	size_t length;
	size_t capacity;
	unsigned char* hyphenpoints;
	unsigned char* escapedwords;
} strarray_t;

int strarray_create(struct strarray* array, size_t initial_size);
void strarray_push(struct strarray* array, char* val, unsigned char bIsEscaped, unsigned char bIsHyphenPoint);
void strarray_destroy(struct strarray* array);

typedef struct intstack
{
	int* data;
	size_t length;
	size_t capacity;
} intstack_t;

void intstack_create(struct intstack* stack, size_t initial);
int intstack_pop(struct intstack* stack);
int intstack_peek(struct intstack* stack);
void intstack_push(struct intstack* stack, int val);
void intstack_destroy(struct intstack* stack);

void FindParagraphs(const char* text, size_t length, struct intstack* paragraphlocs);
void TokenizeString(const char* input, size_t inputlen, strarray_t* out);
void ReflowText(const char* text, size_t len, const int width, cv_t* output, unsigned char);
void StripNewline(const char* input, size_t inputlen, char* out, size_t bufferlen);
#endif
