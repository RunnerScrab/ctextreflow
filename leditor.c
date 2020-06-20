#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "talloc.h"
#include "charvector.h"

char* strnstr(const char*, const char*, size_t);


struct LineEditor
{
	cv_t buffer;
	size_t* line_indices;
/*
  Implement a simple gap buffer.
  Line endings are known after every insertion, except after a reflow.
  After a reflow, recalculation is performed.

  Insertions go at the end by default and always are one line regardless of
  length.


*/
};

int LineEditor_Init(struct LineEditor* le)
{
	cv_init(&le->buffer, 512);
	le->line_indices = talloc(sizeof(size_t) * 128);
}

void LineEditor_Destroy(struct LineEditor* le)
{
	cv_destroy(&le->buffer);
	tfree(le->line_indices);
}

void LineEditor_RebuildLineIndices(struct LineEditor* le)
{
	const char* buf = le->buffer.data;
	size_t len = le->buffer.length;

	char* p = 0;
	do
	{
		p = memchr(buf, '\n', len);
		if(p)
		{
			printf("%d\n", p - buf);
		}
	}
	while(p);

}

void LineEditor_Insert(struct LineEditor* le, const char* data, size_t datalen)
{
	cv_appendstr(&le->buffer, data, datalen);
}

int main(void)
{
	struct LineEditor editor;
	LineEditor_Init(&editor);

	size_t linelimit = 256;
	char* buf = malloc(linelimit);
	size_t sz = 0;

	for(;;)
	{
		memset(buf, 0, linelimit);
		ssize_t bread = getline(&buf, &sz, stdin);

		if(bread >= 0)
		{

			if(sz > linelimit)
			{
				printf("Input exceeds buffer limit.\n");
				continue;
			}

			if(buf[0] == '.')
			{
				if(strnstr(buf, ".exit", 256))
				{
					break;
				}
				else if(strnstr(buf, ".p", 256))
				{
					printf("%s\n", editor.buffer.data);
					continue;
				}
			}
			else
			{
				LineEditor_Insert(&editor, buf, strnlen(buf, 256));
			}
		}
		else
		{
			printf("Getline failure.\n");
			break;
		}
	}
	free(buf);
	LineEditor_Destroy(&editor);
	return 0;
}
