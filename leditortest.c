#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "talloc.h"
#include "charvector.h"
#include "cmdlexer.h"
#include "textutil.h"
#include "leditor.h"

int main(void)
{
	struct LineEditor editor;

	LineEditor_Init(&editor);

	const size_t linelimit = 4096; //A hard limit we impose on the user
	size_t linelen = 256; //A flexible limit used by getline()

	char* buf = talloc(linelimit);
	memset(buf, 0, linelimit);
	for(;;)
	{
		printf("%02lu>", editor.lines_count);
		ssize_t bread = getline(&buf, &linelen, stdin);

		buf[bread] = 0;

		if(bread >= 0)
		{
			/*		if(bread > linelimit)
			{
				printf("Input exceeds buffer limit.\n");
				buf = trealloc(buf, linelimit);
				memset(buf, 0, linelimit);
				continue;
				}*/
			//if(buf[0] == '.')
			//buf[bread - 1] = 0;

			if(LineEditor_ProcessInput(&editor, buf, bread) < 0)
			{
				break;
			}
		}
		else
		{
			printf("Getline failure.\n");
			break;
		}
	}

	if(editor.bSaveResult)
	{
		printf("Saving result:\n%s",
			editor.buffer.data);
	}


	tfree(buf);
	LineEditor_Destroy(&editor);
	return 0;
}
