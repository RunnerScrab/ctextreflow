#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "talloc.h"
#include "charvector.h"
#include "cmdlexer.h"

char* strnstr(const char*, const char*, size_t);

struct TextLine
{
	size_t start, length;
};

struct LineEditor
{
	cv_t buffer;

	struct TextLine* lines;
	size_t lines_reserved;
	size_t lines_count;

};

int LineEditor_Init(struct LineEditor* le)
{
	cv_init(&le->buffer, 512);
	le->lines_reserved = 128;
	le->lines = (struct TextLine*) talloc(sizeof(struct TextLine) * le->lines_reserved);
	memset(le->lines, 0, sizeof(struct TextLine) * le->lines_reserved);
	le->lines_count = 0;

	return 0;
}

void LineEditor_Destroy(struct LineEditor* le)
{
	tfree(le->lines);
	cv_destroy(&le->buffer);
}

void LineEditor_RebuildLineIndices(struct LineEditor* le, size_t from_idx);

void LineEditor_Append(struct LineEditor* le, const char* data, size_t datalen)
{
	cv_appendstr(&le->buffer, data, datalen);
	LineEditor_RebuildLineIndices(le, (le->lines_count > 1) ? (le->lines_count - 1) : 0);
}

void LineEditor_RebuildLineIndices(struct LineEditor* le, size_t from_idx)
{
	printf("Rebuilding from line %lu\n", from_idx);
	const char* buf = le->buffer.data;
	size_t len = le->buffer.length;

	const char* p = buf;


	if(from_idx > le->lines_count)
	{
		return;
	}

	size_t offset = le->lines[from_idx].start;

	le->lines_count = from_idx;

	while(p && offset < len)
	{
		p = memchr(buf + offset, '\n', len - offset);
		if(p)
		{
			if(le->lines_count >= le->lines_reserved)
			{
				le->lines_reserved <<= 1;
				le->lines = trealloc(le->lines, sizeof(struct TextLine) * le->lines_reserved);
			}

			le->lines[le->lines_count].start = (0 == le->lines_count) ? 0 :
				(le->lines[le->lines_count - 1].start + le->lines[le->lines_count - 1].length + 1);
			le->lines[le->lines_count].length = (p - buf) - offset;

			++le->lines_count;
			offset = (p - buf) + 1;
		}
	}
}

void LineEditor_InsertAt(struct LineEditor* le, size_t line_idx, const char* data, size_t datalen)
{

	struct LexerResult lexresult;

	if(line_idx > le->lines_count)
	{
		LineEditor_Append(le, data, datalen);
		LexerResult_Destroy(&lexresult);
		return;
	}

	size_t startidx = le->lines[line_idx].start;
	el_t* copystart = &le->buffer.data[startidx];
	size_t copylen = le->buffer.length - startidx + 1;
	el_t* temp = talloc(copylen + 1);
	memcpy(temp, copystart, copylen);
	memset(copystart, 0, copylen);
	LineEditor_Append(le, data, datalen);
	LineEditor_Append(le, temp, copylen + 1);
	LineEditor_RebuildLineIndices(le, line_idx);


	tfree(temp);
}

struct EditorCommand
{
	const char* name;
	int (*cmdfp)(struct LineEditor*, struct LexerResult*);
};


int EditorCmdQuit(struct LineEditor* pLE, struct LexerResult* plr)
{
	return -1;
}

int EditorCmdPrint(struct LineEditor* pLE, struct LexerResult* plr)
{
	printf("Print called.\n");
	size_t idx = 0;
	size_t linebuf_reserved = 512;
	char* linebuf = talloc(linebuf_reserved);
	for(; idx < pLE->lines_count; ++idx)
	{
		size_t linelen = pLE->lines[idx].length;
		if(linebuf_reserved < linelen)
		{
			linebuf_reserved <<= 1;
			linebuf = trealloc(linebuf, linebuf_reserved);
		}
		linebuf[linelen] = 0;
		memcpy(linebuf, &pLE->buffer.data[pLE->lines[idx].start], linelen);
		printf("%02lu]%s\n", idx, linebuf);
	}
	tfree(linebuf);
	return 0;
}

int EditorCmdRebuildLines(struct LineEditor* pLE, struct LexerResult* plr)
{
	LineEditor_RebuildLineIndices(pLE, 0);
	size_t idx = 0;
	for(; idx < pLE->lines_count; ++idx)
		printf("%lu %lu\n", pLE->lines[idx].start, pLE->lines[idx].length);
	return 0;
}

int EditorCmdCmp(const void* pA, const void* pB)
{
	struct EditorCommand* pCmd = ((struct EditorCommand*) pB);
	printf("Comparing %s to %s\n", (const char*) pA, pCmd->name);
	return strcmp((const char*) pA, pCmd->name);
}

int EditorCmdInsert(struct LineEditor* pLE, struct LexerResult* plr)
{
	size_t tokencount = LexerResult_GetTokenCount(plr);
	if(tokencount < 2)
	{
		printf("*Too few arguments.\n");
		return 0;
	}

	char* pos_str = LexerResult_GetTokenAt(plr, 1);
	size_t insert_idx = atoi(pos_str);
	printf("Insert idx was %lu, parsed from '%s'\n", insert_idx, pos_str);

	if(tokencount > 2)
	{

		char* insertstr = LexerResult_GetStringAfterToken(plr, 2);
		size_t insertstr_len = strlen(insertstr);
		char* temp = malloc(insertstr_len + 2);
		memset(temp, 0, insertstr_len + 2);
		strncat(temp, insertstr, insertstr_len + 2);
		strncat(temp, "\n", insertstr_len + 2);
		printf("Performing insert of '%s'.\n", temp);
		LineEditor_InsertAt(pLE, insert_idx, temp, insertstr_len + 2);
		free(temp);
	}
	return 0;
}

int main(void)
{
	struct LineEditor editor;

	struct EditorCommand commands[] =
		{
			{".i", EditorCmdInsert},
			{".p", EditorCmdPrint},
			{".q", EditorCmdQuit},
			{".r", EditorCmdRebuildLines}
		};


	LineEditor_Init(&editor);

	const size_t linelimit = 256; //A hard limit we impose on the user
	size_t linelen = 256; //A flexible limit used by getline()

	char* buf = talloc(linelimit);
	memset(buf, 0, linelimit);
	struct LexerResult lexresult;
	LexerResult_Prepare(&lexresult);
	for(;;)
	{
		ssize_t bread = getline(&buf, &linelen, stdin);

		buf[bread] = 0;

		if(bread >= 0)
		{

			if(bread > linelimit)
			{
				printf("Input exceeds buffer limit.\n");
				buf = trealloc(buf, linelimit);
				memset(buf, 0, linelimit);
				continue;
			}

			if(buf[0] == '.')
			{
				buf[bread - 1] = 0;
				LexerResult_Clear(&lexresult);
				LexerResult_LexString(&lexresult, buf, bread);

				char* cmdstr = LexerResult_GetTokenAt(&lexresult, 0);
				printf("cmdstr is '%s'\n", cmdstr);
				struct EditorCommand* pCmd = (struct EditorCommand*)
					bsearch(cmdstr, commands, 4, sizeof(struct EditorCommand),
						EditorCmdCmp);

				if(pCmd && pCmd->cmdfp(&editor, &lexresult) < 0)
				{
					printf("*Quitting\n");
					break;
				}
 			}
			else
			{
				LineEditor_Append(&editor, buf, strnlen(buf, 256));
			}
		}
		else
		{
			printf("Getline failure.\n");
			break;
		}
	}
	tfree(buf);
	LineEditor_Destroy(&editor);
	LexerResult_Destroy(&lexresult);
	return 0;
}
