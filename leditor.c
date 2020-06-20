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
	size_t lineidx_count;
	size_t lineidx_reserved;
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
	le->lineidx_count = 0;
	le->lineidx_reserved = 128;
	return 0;
}

void LineEditor_Destroy(struct LineEditor* le)
{
	cv_destroy(&le->buffer);
	tfree(le->line_indices);
}

void LineEditor_RebuildLineIndices(struct LineEditor* le)
{
	const char* buf = le->buffer.data;
	int len = le->buffer.length;

	const char* p = buf;
	int offset = 0;
	le->lineidx_count = 0;
	while(p && offset < len)
	{
		p = memchr(buf + offset, '\n', len - offset);
		if(p)
		{
			if(le->lineidx_count >= le->lineidx_reserved)
			{
				le->lineidx_reserved <<= 1;
				le->line_indices = trealloc(le->line_indices, sizeof(size_t) * le->lineidx_reserved);
			}

			le->line_indices[le->lineidx_count++] = (p - buf);
			printf("Offset: %d\n", (p-buf));
			offset = (p - buf) + 1;
		}
		else
		{
			break;
		}
	}


}

void LineEditor_Insert(struct LineEditor* le, const char* data, size_t datalen)
{
	cv_appendstr(&le->buffer, data, datalen);
}

struct EditorCommand
{
	const char* name;
	int (*cmdfp)(struct LineEditor*, const char*);
};


int EditorCmdQuit(struct LineEditor* pLE, const char* line)
{
	return -1;
}

int EditorCmdPrint(struct LineEditor* pLE, const char* line)
{
	printf("%s\n", pLE->buffer.data);
	return 0;
}

int EditorCmdRebuildLines(struct LineEditor* pLE, const char* line)
{
	LineEditor_RebuildLineIndices(pLE);
	size_t idx = 0;
	for(; idx < pLE->lineidx_count; ++idx)
		printf("%lu\n", pLE->line_indices[idx]);
	return 0;
}

int EditorCmdCmp(const void* pA, const void* pB)
{
	struct EditorCommand* pCmd = ((struct EditorCommand*) pB);
	printf("Comparing %s to %s\n", (const char*) pA, pCmd->name);
	return strcmp((const char*) pA, pCmd->name);
}

int main(void)
{
	struct LineEditor editor;

	struct EditorCommand commands[] =
		{
			{"p", EditorCmdPrint},
			{"q", EditorCmdQuit},
			{"r", EditorCmdRebuildLines}
		};


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
				char* pNL = memchr(buf, '\n', linelimit);
				char cmdstr[256] = {0};
				if(pNL)
				{
					memcpy(cmdstr, buf + 1, pNL - buf - 1);
				}
				struct EditorCommand* pCmd = (struct EditorCommand*)
					bsearch(cmdstr, commands, 3, sizeof(struct EditorCommand),
						EditorCmdCmp);

				if(pCmd && pCmd->cmdfp(&editor, &buf[1]) < 0)
				{
					printf("*Quitting\n");
					break;
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
