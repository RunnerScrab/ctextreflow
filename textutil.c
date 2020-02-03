#include "textutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "charvector.h"

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

int strarray_create(struct strarray* array, size_t initial_size)
{
	array->length = 0;
	array->capacity = initial_size;
	array->strings = (cv_t*) malloc(sizeof(cv_t) * initial_size);
	array->hyphenpoints = (unsigned char*) malloc(sizeof(unsigned char) * initial_size);
	return array->strings ? 0 : -1;
}

void strarray_push(struct strarray* array, char* val, unsigned char bIsHyphenPoint)
{
	if(array->length >= array->capacity)
	{
		array->capacity <<= 1;
		array->strings = (cv_t*) realloc(array->strings, sizeof(cv_t) * array->capacity);
		array->hyphenpoints = (unsigned char*) realloc(array->hyphenpoints, sizeof(unsigned char) * array->capacity);
	}
	memset(&array->strings[array->length], 0, sizeof(cv_t));
	memset(&array->hyphenpoints[array->length], 0, sizeof(unsigned char));
	array->hyphenpoints[array->length] = bIsHyphenPoint;
	cv_init(&array->strings[array->length], 16);
	cv_appendstr(&(array->strings[array->length]), val);

	++array->length;
}

void strarray_destroy(struct strarray* array)
{
	size_t idx = 0;
	for(; idx < array->length; ++idx)
	{
		cv_destroy(&array->strings[idx]);
	}
	free(array->hyphenpoints);
	free(array->strings);
}

void intstack_create(struct intstack* stack, size_t initial)
{
	stack->data = (int*) malloc(sizeof(int) * initial);
	stack->capacity = initial;
	stack->length = 0;
}

int intstack_pop(struct intstack* stack)
{
	if(stack->length > 0)
	{
		--stack->length;
	}

	int retval = stack->data[stack->length];
	stack->data[stack->length] = 0;

	return retval;
}

int intstack_peek(struct intstack* stack)
{
	return stack->data[stack->length - 1];
}

void intstack_push(struct intstack* stack, int val)
{
	if(stack->length >= stack->capacity)
	{
		stack->capacity <<= 1; //We can only push one value at a time through this function
		stack->data = (int*) realloc(stack->data, sizeof(int) * stack->capacity);
	}

	stack->data[stack->length] = val;
	++stack->length;
}

void intstack_destroy(struct intstack* stack)
{
	free(stack->data);
}

void SplitWord(const char* inword, size_t inwordlen, cv_t* outword_a, cv_t* outword_b)
{
	size_t halflen = inwordlen / 2;
	cv_strncpy(outword_a, inword, halflen);
	cv_strncpy(outword_b, &inword[halflen], inwordlen - halflen);

}

unsigned int CanWordBeSplit(const char* word, size_t len)
{
	if(len < 8 || isupper(word[0]) || word[0] == '"')
		return 0;
	else
		return 1;
}

void TokenizeString(const char* input, size_t len, strarray_t* out)
{
	char* savep = 0;
	char* ret = 0;
	char* inputcopy = (char*) malloc(sizeof(char) * len + 1);
	memcpy(inputcopy, input, sizeof(char) * (len + 1));

	do
	{
		ret = strtok_r(ret ? 0 : inputcopy, " ", &savep);

		if(ret && ret >= &inputcopy[len - 1])
		{
			free(inputcopy);
			return;
		}
		else if(ret)
		{
			size_t wordlen = strlen(ret);
			if(0 && CanWordBeSplit(ret, wordlen))
			{
				cv_t a, b;
				cv_init(&a, wordlen);
				cv_init(&b, wordlen);
				SplitWord(ret, wordlen, &a, &b);
				strarray_push(out, a.data, 1);
				strarray_push(out, b.data, 0);
				cv_destroy(&a);
				cv_destroy(&b);
			}
			else
			{
				strarray_push(out, ret, 0);
			}
		}
	}
	while(ret);

	free(inputcopy);
}

void FindParagraphs(const char* text, size_t length, struct intstack* paragraphlocs)
{
	/*
	  Find the first text.

	  Find first newline after the text begins.

	  If newline followed by another newline, mark location of last
	  newline as the start of a new paragraph.

	  Count the number of contiguous newlines and store with the paragraph.
	*/

	const char* p = text;
	for(; *p && isspace(*p); ++p);
	intstack_push(paragraphlocs, p - text);
	const char* found = 0;
	do
	{
		found = strchr(p, '\n');
		if(found)
		{
			size_t nls_found = 1;
			const char* nlrun = found;
			for(; *nlrun && *nlrun == '\n'; ++nlrun, ++nls_found);
			if(nls_found > 1)
			{
				//We have encountered at least one blank line, which separates
				//paragraphs
				intstack_push(paragraphlocs, found - text - 1); // Push end of last paragraph
				intstack_push(paragraphlocs, nlrun - text); //Push start of new one

			}
			p = nlrun;
		}
	}
	while(found);
}

void ReflowText(const char* text, size_t len, int width, cv_t* output, unsigned char bIndentFirstWord)
{
	//Uses a shortest paths method to solve optimization problem
	strarray_t words;
	strarray_create(&words, 64);

	TokenizeString((char*) text, len, &words);

	if(bIndentFirstWord)
	{
		cv_t spaced;
		cv_init(&spaced, words.strings[0].length + 2);
		cv_sprintf(&spaced, "%*s%s", bIndentFirstWord, "", words.strings[0].data);
		cv_swap(&spaced, &words.strings[0]);
		cv_destroy(&spaced);
	}

	size_t idx = 0;
	size_t count = words.length;

	struct intstack offsets;
	intstack_create(&offsets, count + 1);
	intstack_push(&offsets, 0);

	for(idx = 0; idx < count; ++idx)
	{
		intstack_push(&offsets, intstack_peek(&offsets) + words.strings[idx].length);
	}

	int* minima = (int*) malloc(sizeof(int) * (count+1));
	int* breaks = (int*) malloc(sizeof(int) * (count + 1));
	int wsincelasthyphen = 0;
	memset(breaks, 0, sizeof(int) * (count + 1));
	minima[0] = 0;
	memset(&minima[1], 0x7F, sizeof(int) * count);

	int i = 0, j = 0, w = 0, cost = 0;
	unsigned char thishyphenpoint = 0;
	for(; i <= count; ++i)
	{
		for(j = i + 1; j <= count; ++j)
		{
			thishyphenpoint = words.hyphenpoints[j];

			if(j < count)
			{
				w = offsets.data[j] - offsets.data[i] + j - i - 1;
			}
			else
			{
				//We don't need to optimize for the alignment of the last line
				w = offsets.data[j] - offsets.data[i] + j - i - 1;
				//w = width;
			}

			if(w > width)
			{
				break;
			}

			register int k = width - w;
			cost = minima[i] + (k * k);

			if(cost < minima[j])
			{
//				if(!thishyphenpoint || (thishyphenpoint && wsincelasthyphen >= width<<1))
				{
					wsincelasthyphen = 0;
					minima[j] = cost;
					breaks[j] = i;
				}
			}
			wsincelasthyphen += width;
		}
	}

	intstack_t revbreak;
	intstack_create(&revbreak, count);
	j = count;
	intstack_push(&revbreak, count);
	while(j > 0)
	{
		i = breaks[j];
		intstack_push(&revbreak, i);
		j = i;
	}
	i = intstack_pop(&revbreak);

	do
	{
		j = intstack_pop(&revbreak);

		for(idx = i; idx < j; ++idx)
		{
			if(!words.hyphenpoints[idx])
			{
				cv_strncat(&words.strings[idx], " ", 2);
			}
			cv_strncat(output, words.strings[idx].data, words.strings[idx].length);
		}
		if(words.hyphenpoints[idx - 1])
		{
			cv_strncat(output, "-", 2);
		}
		cv_strncat(output, "\n", 2);

		i = j;
	}
	while(j);

	intstack_destroy(&revbreak);
	free(minima);
	free(breaks);
	intstack_destroy(&offsets);
	strarray_destroy(&words);
}

void StripNewline(const char* input, size_t inputlen, char* out, size_t bufferlen)
{
	//Preserves blank lines
	const char* pos = input;
	const char* found = 0;
	size_t spaceleft = bufferlen - 1;
	do
	{
		found = strchr(pos, '\n');
		size_t offset = found - pos;
		if(found && (offset <= spaceleft))
		{
			const char* p = found + 1;
			if(*p && '\n' == *p)
			{
				for(;*(p) && '\n' == *p; ++p);
				found = p - 1;
				offset = min(spaceleft, ((found) - pos) + 1);
				strncat(out, pos, offset);
			}
			else
			{
				strncat(out, pos, offset);
			}
			spaceleft -= offset;
			pos = found + 1;
		}
		else if(found)
		{
			strncat(out, pos, spaceleft);
			break;
		}
		else
		{
			strncat(out, pos, &input[inputlen] - pos);
			spaceleft -= &input[inputlen] - pos;
		}
	}
	while(found);
}
