#include "textutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "charvector.h"

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

int reflow_strarray_create(struct reflow_strarray* array, size_t initial_size)
{
	array->length = 0;
	array->capacity = initial_size;

	size_t strarrsize = sizeof(reflow_word_t) * initial_size;
	array->strings = (reflow_word_t*) malloc(strarrsize);
	memset(array->strings, 0, strarrsize);

	return array->strings ? 0 : -1;
}

void reflow_strarray_push(struct reflow_strarray* array, char* val, unsigned char bIsEscaped, unsigned char bIsHyphenPoint)
{
	if(array->length >= array->capacity)
	{
		array->capacity = max((array->capacity<<3), (array->capacity + array->length));
		size_t strarrsize = sizeof(reflow_word_t) * array->capacity;
		array->strings = (reflow_word_t*) realloc(array->strings, strarrsize);
		memset(&array->strings[array->length], 0, sizeof(reflow_word_t) * (array->capacity - array->length));
	}
	reflow_word_t* pword = &array->strings[array->length];
	pword->bHyphenPoint = bIsHyphenPoint;
	pword->bEscaped = bIsEscaped;
	cv_init(&pword->string, 16);
	cv_appendstr(&pword->string, val);

	++array->length;
}

void reflow_strarray_destroy(struct reflow_strarray* array)
{
	size_t idx = 0;
	for(; idx < array->length; ++idx)
	{
		cv_destroy(&array->strings[idx].string);
	}
	free(array->strings);
}

void reflow_intstack_create(struct reflow_intstack* stack, size_t initial)
{
	stack->data = (int*) malloc(sizeof(int) * initial);
	stack->capacity = initial;
	stack->length = 0;
}

int reflow_intstack_pop(struct reflow_intstack* stack)
{
	if(stack->length > 0)
	{
		--stack->length;
	}

	int retval = stack->data[stack->length];
	stack->data[stack->length] = 0;

	return retval;
}

void reflow_intstack_clear(struct reflow_intstack* stack)
{
	memset(stack->data, 0, sizeof(int) * stack->capacity);
	stack->length = 0;
}

int reflow_intstack_popleft(struct reflow_intstack* stack)
{
	//This method actually makes this a deque
	if(stack->length > 0)
	{
		--stack->length;
	}
	int retval = stack->data[0];
	memmove(stack->data, &stack->data[1], sizeof(int) * stack->length);
	return retval;
}

int reflow_intstack_peek(struct reflow_intstack* stack)
{
	return stack->data[stack->length - 1];
}

void reflow_intstack_push(struct reflow_intstack* stack, int val)
{
	if(stack->length >= stack->capacity)
	{
		stack->capacity <<= 1; //We can only push one value at a time through this function
		stack->data = (int*) realloc(stack->data, sizeof(int) * stack->capacity);
	}

	stack->data[stack->length] = val;
	++stack->length;
}

void reflow_intstack_destroy(struct reflow_intstack* stack)
{
	free(stack->data);
}

typedef struct intpair
{
	int x, y;
} intpair_t;

typedef struct reflow_pairdeque
{
	intpair_t* data;
	size_t length, capacity;
} reflow_pairdeque_t;

void reflow_pairdeque_create(struct reflow_pairdeque* stack, size_t initial)
{
	stack->data = (intpair_t*) malloc(sizeof(intpair_t) * initial);
	memset(stack->data, 0, sizeof(intpair_t) * initial);
	stack->capacity = initial;
	stack->length = 0;
}

void reflow_pairdeque_popnoret(struct reflow_pairdeque* stack)
{
	if(stack->length > 0)
	{
		--stack->length;
	}

	memset(&stack->data[stack->length], 0, sizeof(intpair_t));
}

void reflow_pairdeque_pop(struct reflow_pairdeque* stack, intpair_t* out)
{
	if(stack->length > 0)
	{
		--stack->length;
	}

	*out = stack->data[stack->length];
	memset(&stack->data[stack->length], 0, sizeof(intpair_t));
}

void reflow_pairdeque_clear(struct reflow_pairdeque* stack)
{
	memset(stack->data, 0, sizeof(intpair_t) * stack->capacity);
	stack->length = 0;
}

void reflow_pairdeque_popleft(struct reflow_pairdeque* stack, intpair_t* out)
{
	//This method actually makes this a deque
	if(stack->length > 0)
	{
		--stack->length;
	}
	*out = stack->data[0];
	memmove(stack->data, &stack->data[1], sizeof(intpair_t) * stack->length);
}

inline int reflow_pairdeque_peek_x(struct reflow_pairdeque* stack)
{
	return stack->data[stack->length - 1].x;
}

inline int reflow_pairdeque_peek_y(struct reflow_pairdeque* stack)
{
	return stack->data[stack->length - 1].y;
}


void reflow_pairdeque_push(struct reflow_pairdeque* stack, int x, int y)
{
	if(stack->length >= stack->capacity)
	{
		stack->capacity <<= 1; //We can only push one value at a time through this function
		stack->data = (intpair_t*) realloc(stack->data, sizeof(intpair_t) * stack->capacity);
	}

	stack->data[stack->length].x = x;
	stack->data[stack->length].y = y;
	++stack->length;
}

void reflow_pairdeque_destroy(struct reflow_pairdeque* stack)
{
	free(stack->data);
}

void SplitWord(const char* inword, size_t inwordlen, cv_t* outword_a, cv_t* outword_b)
{
	size_t halflen = inwordlen / 2;
	cv_strncpy(outword_a, inword, halflen);
	cv_push(outword_a, 0);
	cv_strncpy(outword_b, &inword[halflen], inwordlen - halflen);
	cv_push(outword_b, 0);
}

unsigned int CanWordBeSplit(const char* word, size_t len)
{
	if(len < 8 || isupper(word[0]) || word[0] == '"')
		return 0;
	else
		return 1;
}

void TokenizeString(const char* input, size_t len, reflow_strarray_t* out)
{
	char* savep = 0;
	char* ret = 0;
	char* inputcopy = (char*) malloc(sizeof(char) * (len +1));
	memcpy(inputcopy, input, sizeof(char) * (len));
	inputcopy[len] = 0;
	size_t offset = 0, lastescapetoken = 0;
	do
	{
		ret = strtok_r(ret ? 0 : inputcopy, " `", &savep);


		if(ret && ret >= &inputcopy[len - 1])
		{
			free(inputcopy);
			return;
		}
		else if(ret)
		{
			offset = ret - inputcopy;
			size_t wordlen = strlen(ret);
			if(offset > 0 && input[offset - 1] == '`' && lastescapetoken != offset - 1 &&
				(offset + wordlen) < len && input[offset + wordlen] == '`')

			{
				//Special handling of escaped words. Escaped words are not displayed, though
				//they perform control functions (enabling color, bold, italics, etc.).
				//Here we store metadata for them for the reflow algorithm to later use when
				//placing words down.
				cv_t rebuilttoken;
				cv_init(&rebuilttoken, wordlen + 3);
				cv_sprintf(&rebuilttoken, "`%s`", ret);

				unsigned char spacealignment = 1;
				//bit 1 set for any escaped word, bit 2 set for space on the left, bit 4 set for space on the right
				if((offset - 2) > 0 && isspace(input[offset - 2]))
					spacealignment |= 2;
				if((offset + wordlen + 1) < len &&
					isspace(input[offset + wordlen + 1]))
					spacealignment |= 4;

				reflow_strarray_push(out, rebuilttoken.data, spacealignment, 0);
				cv_destroy(&rebuilttoken);
				lastescapetoken =  offset + wordlen;
			}
			else if(CanWordBeSplit(ret, wordlen))
			{
				cv_t a, b;
				cv_init(&a, wordlen);
				cv_init(&b, wordlen);
				SplitWord(ret, wordlen, &a, &b);
				//By default, the word's cost is its length.
				reflow_strarray_push(out, a.data, 0, 1);
				reflow_strarray_push(out, b.data, 0, 0);
				cv_destroy(&a);
				cv_destroy(&b);
			}
			else
			{
				reflow_strarray_push(out, ret, 0, 0);
			}
		}
	}
	while(ret);

	free(inputcopy);
}

void FindParagraphs(const char* text, size_t length, struct reflow_intstack* paragraphlocs)
{
	const char* p = &text[strspn(text, " ")];

	reflow_intstack_push(paragraphlocs, p - text);
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
				reflow_intstack_push(paragraphlocs, found - text - 1); // Push end of last paragraph
				reflow_intstack_push(paragraphlocs, nlrun - text); //Push start of new one

			}
			p = nlrun;
		}
	}
	while(found);
}



void ReflowParagraph(const char* text, size_t len, const int width, cv_t* output, unsigned char bIndentFirstWord)
{
	//Uses a shortest paths method to solve optimization problem
	reflow_strarray_t words;
	reflow_strarray_create(&words, 64);

	TokenizeString((char*) text, len, &words);

	if(bIndentFirstWord)
	{
		cv_t spaced;
		cv_init(&spaced, words.strings[0].string.length + 2);
		cv_sprintf(&spaced, "%*s%s", bIndentFirstWord, "", words.strings[0].string.data);
		cv_swap(&spaced, &words.strings[0].string);
		cv_destroy(&spaced);
	}

	size_t idx = 0;
	size_t count = words.length;

	struct reflow_intstack offsets;
	reflow_intstack_create(&offsets, count + 1);
	reflow_intstack_push(&offsets, 0);

	//Calculate word costs. Words which do not represent a control sequence have a
	//cost simply equal to their length.
	for(idx = 0; idx < count; ++idx)
	{
		reflow_intstack_push(&offsets, reflow_intstack_peek(&offsets) +
				(words.strings[idx].bEscaped ? 0 : words.strings[idx].string.length));
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
			thishyphenpoint = words.strings[j].bHyphenPoint;

			if(j < count)
			{
				w = offsets.data[j] - offsets.data[i] + j - i - 1;
			}
			else
			{
				//We don't need to optimize for the alignment of the last line
				w = width;
			}

			if(w > width)
			{
				break;
			}

			register int k = width - w;
			cost = minima[i] + (k * k);

			if(cost < minima[j])
			{
				if(!thishyphenpoint || (thishyphenpoint && wsincelasthyphen >= (width)))
				{
					if(thishyphenpoint)
						wsincelasthyphen = 0;
					else
					        wsincelasthyphen += width;
					minima[j] = cost;
					breaks[j] = i;
				}
			}

		}
	}

	reflow_intstack_t revbreak;
	reflow_intstack_create(&revbreak, count);
	j = count;
	reflow_intstack_push(&revbreak, count);
	while(j > 0)
	{
		i = breaks[j];
		reflow_intstack_push(&revbreak, i);
		j = i;
	}
	i = reflow_intstack_pop(&revbreak);

	cv_t wordbuf;
	cv_init(&wordbuf, 64);
	do
	{
		j = reflow_intstack_pop(&revbreak);

		for(idx = i; idx < j; ++idx)
		{
			reflow_word_t* pword = &words.strings[idx];
			unsigned char spaceescapeflag = pword->bEscaped;
			if(!pword->bHyphenPoint)
			{
				//bit 1 set for any escaped word, bit 2 set for space on the left, bit 4 set for space on the right
				if(spaceescapeflag)
				{
					cv_sprintf(&wordbuf, "%*s%s%*s",
						!!(spaceescapeflag & 2), "", pword->string.data,
						!!(spaceescapeflag & 4), "");
					cv_swap(&wordbuf, &pword->string);
				}
				else if(!words.strings[idx + 1].bEscaped)
				{
					cv_strncat(&pword->string, " ", 2);
				}
			}

			cv_strncat(output, pword->string.data, pword->string.length);
		}
		if(words.strings[idx - 1].bHyphenPoint)
		{
			cv_strncat(output, "-", 2);
		}
		cv_strncat(output, "\n", 2);

		i = j;
	}
	while(j);

	cv_destroy(&wordbuf);
	reflow_intstack_destroy(&revbreak);
	free(minima);
	free(breaks);
	reflow_intstack_destroy(&offsets);
	reflow_strarray_destroy(&words);
}

inline int costfn(int i, int j, const int* minima, const int* offsets, const int width, const int count)
{
	int w = j < count ? (offsets[j] - offsets[i] + j - i - 1) : width;

	if(w > width)
	{
		return  (w - width) << 16;
	}
	return minima[i] + (width - w) * (width - w);

}

int hfn(int l, int k, const int* minima, const int* offsets, const int width, const int count)
{
	int low = l + 1;
	int high = count;
	int mid = 0;
	while(low < high)
	{
		mid = (low + high)>>1;
		if(costfn(l, mid, minima, offsets, width, count) <=
			costfn(k, mid, minima, offsets, width, count))
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	}
	if(costfn(l, high, minima, offsets, width, count) <=
		costfn(k, high, minima, offsets, width, count))
	{
		return high;
	}
	return l + 2;
}

void ReflowParagraphBinary(const char* text, size_t len, const int width, cv_t* output, unsigned char bIndentFirstWord)
{
	reflow_strarray_t words;
	reflow_strarray_create(&words, 64);

	TokenizeString((char*) text, len, &words);

	if(bIndentFirstWord)
	{
		cv_t spaced;
		cv_init(&spaced, words.strings[0].string.length + 2);
		cv_sprintf(&spaced, "%*s%s", bIndentFirstWord, "", words.strings[0].string.data);
		cv_swap(&spaced, &words.strings[0].string);
		cv_destroy(&spaced);
	}

	size_t idx = 0;
	size_t count = words.length;

	struct reflow_intstack offsets;
	reflow_intstack_create(&offsets, count + 1);
	reflow_intstack_push(&offsets, 0);

	//Calculate word costs. Words which do not represent a control sequence have a
	//cost simply equal to their length.
	for(idx = 0; idx < count; ++idx)
	{
		reflow_intstack_push(&offsets, reflow_intstack_peek(&offsets) +
				(words.strings[idx].bEscaped ? 0 : words.strings[idx].string.length));
	}

	int* minima = (int*) malloc(sizeof(int) * (count + 1));
	int* breaks = (int*) malloc(sizeof(int) * (count + 1));
	int wsincelasthyphen = 0;
	memset(breaks, 0, sizeof(int) * (count + 1));
	memset(minima, 0, sizeof(int) * (count + 1));

	struct reflow_pairdeque deque;
	reflow_pairdeque_create(&deque, 32);
	reflow_pairdeque_push(&deque, 1, 0);

	int j = 1, l = 0, peekx = 0, peeky = 0;
	unsigned char thishyphenpoint = 0;
	for(; j <= count; ++j)
	{
		l = deque.data[0].x;
		if(costfn(j - 1, j, minima, offsets.data, width, count) < costfn(l, j, minima, offsets.data, width, count))
		{
			minima[j] = costfn(j - 1, j, minima, offsets.data, width, count);
			breaks[j] = j - 1;
			reflow_pairdeque_clear(&deque);
			reflow_pairdeque_push(&deque, j - 1, j + 1);
		}
		else
		{
			thishyphenpoint = words.strings[j].bHyphenPoint;
			if(!thishyphenpoint || (thishyphenpoint && wsincelasthyphen >= (width)))
			{
				minima[j] = costfn(l, j, minima, offsets.data, width, count);
				breaks[j] = l;
				if(thishyphenpoint)
					wsincelasthyphen = 0;
				else
					wsincelasthyphen += width;
			}

			while(1)
			{
				peekx = reflow_pairdeque_peek_x(&deque);
				peeky = reflow_pairdeque_peek_y(&deque);
				if(costfn(j - 1, peekx, minima, offsets.data, width, count) >
					costfn(peekx, peeky, minima, offsets.data, width, count))
					break;
				reflow_pairdeque_popnoret(&deque);
			}

			reflow_pairdeque_push(&deque, j - 1, hfn(j - 1, reflow_pairdeque_peek_x(&deque), minima, offsets.data, width, count));
			if((j + 1) == deque.data[1].y)
			{
				reflow_pairdeque_popnoret(&deque);
			}
			else
			{
				++deque.data[0].y;
			}
		}
	}

	int i = 0;
	reflow_intstack_t revbreak;
	reflow_intstack_create(&revbreak, count);
	j = count;
	reflow_intstack_push(&revbreak, count);
	while(j > 0)
	{
		i = breaks[j];
		reflow_intstack_push(&revbreak, i);
		j = i;
	}
	i = reflow_intstack_pop(&revbreak);

	cv_t wordbuf;
	cv_init(&wordbuf, 64);
	do
	{
		j = reflow_intstack_pop(&revbreak);

		for(idx = i; idx < j; ++idx)
		{
			reflow_word_t* pword = &words.strings[idx];
			unsigned char spaceescapeflag = pword->bEscaped;
			if(!pword->bHyphenPoint)
			{
				//bit 1 set for any escaped word, bit 2 set for space on the left, bit 4 set for space on the right
				if(spaceescapeflag)
				{
					cv_sprintf(&wordbuf, "%*s%s%*s",
						!!(spaceescapeflag & 2), "", pword->string.data,
						!!(spaceescapeflag & 4), "");
					cv_swap(&wordbuf, &pword->string);
				}
				else if(!words.strings[idx + 1].bEscaped)
				{
					cv_strncat(&pword->string, " ", 2);
				}
			}

			cv_strncat(output, pword->string.data, pword->string.length);
		}
		if(words.strings[idx - 1].bHyphenPoint)
		{
			cv_strncat(output, "-", 2);
		}
		cv_strncat(output, "\n", 2);

		i = j;
	}
	while(j);

	cv_destroy(&wordbuf);
	reflow_intstack_destroy(&revbreak);
	free(minima);
	free(breaks);
	reflow_intstack_destroy(&offsets);
	reflow_strarray_destroy(&words);
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
