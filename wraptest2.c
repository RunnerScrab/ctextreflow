#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "textutil.h"
#include "charvector.h"

const char* teststr = "Each iteration of the dynamic `red`programming scheme`default` can also be seen as filling in a matrix, where a cell adds up the least overall cost to a subproblem (a column minimum) and a penalty. A concave weight function implies that the matrix is totally monotone and in 1987 Shor, Moran, Aggarwal, Wilber and Klawe devised an algorithm which finds the row maxima of such matrix in linear time.\n\nEven though SMAWK can be modified to find column minima instead, it is not possible to apply it directly to this \"on-line\" matrix as it might try to evaluate a not \"available\" cell, i.e. a cell dependent on some yet unknown column minimum. However, Wilber came up with an algorithm in 1988 which \"pretends\" to know the minima and still runs in O(n) time. An \"ordered\" algorithm which obeys the availability of the matrix as presented by Aggarwal and Tokuyama in 1998 follows.`default`";

void ReflowText(const char* input, size_t len, cv_t* output, unsigned char bBinary)
{
	char* nlstrippedbuf = (char*) malloc(sizeof(char) * (len + 1));
	memset(nlstrippedbuf, 0, sizeof(char) * (len + 1));
	StripNewline(input, len + 1, nlstrippedbuf, len + 1);

	reflow_intstack_t paragraphbounds;
	reflow_intstack_create(&paragraphbounds, 8);

	cv_t buffer;
	cv_init(&buffer, len);

	FindParagraphs(nlstrippedbuf, len, &paragraphbounds);

	size_t idx = 0;
	for(; idx < paragraphbounds.length; idx += 2)
	{
		int start = paragraphbounds.data[idx];
		int end = ((idx + 1) < paragraphbounds.length) ? paragraphbounds.data[idx + 1] : len;

		cv_clear(&buffer);
		switch(bBinary)
		{
		case 1:
			ReflowParagraphBinary(&nlstrippedbuf[start], end - start + 1, 80, &buffer, 2);
			break;
		case 0:
			ReflowParagraph(&nlstrippedbuf[start], end - start + 1, 80, &buffer, 2);
			break;
		}
		cv_strcat(output, buffer.data);
	}

	reflow_intstack_destroy(&paragraphbounds);
	cv_destroy(&buffer);
	free(nlstrippedbuf);
}

int main(void)
{

	cv_t reflowed;
	cv_init(&reflowed, strlen(teststr) + 1);
	ReflowText(teststr, strlen(teststr), &reflowed);
	printf("%s", reflowed.data);
	cv_destroy(&reflowed);
	return 0;
}
