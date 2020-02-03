#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "textutil.h"
#include "charvector.h"

const char* teststr =
	"\n8. computerized tomography, computed tomography, CT, computerized axial tomography,\ncomputed axial tomography, CAT -- (a method of examining body organs by scanning them with X rays\n and using a computer to construct a series of cross-sectional scans along a single axis)\n\n\nOverview of verb cat\n\n\n\n";

const char* teststr2 = "Each iteration of the dynamic programming scheme can also be seen as filling in a matrix, where a cell adds up the least overall cost to a subproblem (a column minimum) and a penalty. A concave weight function implies that the matrix is totally monotone and in 1987 Shor, Moran, Aggarwal, Wilber and Klawe devised an algorithm which finds the row maxima of such matrix in linear time. Even though SMAWK can be modified to find column minima instead, it is not possible to apply it directly to this \"on-line\" matrix as it might try to evaluate a not \"available\" cell, i.e. a cell dependent on some yet unknown column minimum. However, Wilber came up with an algorithm in 1988 which \"pretends\" to know the minima and still runs in O(n) time. An \"ordered\" algorithm which obeys the availability of the matrix as presented by Aggarwal and Tokuyama in 1998 follows.";

int main(void)
{
	size_t teststrlen = strlen(teststr2);
	size_t outlen = teststrlen;
	char* out = (char*) malloc(sizeof(char) * (outlen + 1));
	memset(out, 0, sizeof(char) * outlen);
	StripNewline(teststr2, teststrlen, out, outlen);

	printf("Length: %lu\n", teststrlen);
	struct intstack paragraphbounds;
	intstack_create(&paragraphbounds, 8);

	cv_t buffer;
	cv_init(&buffer, teststrlen);
	printf("Outlen: %lu\n", outlen);

	FindParagraphs(out, outlen, &paragraphbounds);

	size_t idx = 0;
	for(; idx < paragraphbounds.length; idx += 2)
	{
		int start = paragraphbounds.data[idx];
		int end = ((idx + 1) < paragraphbounds.length) ? paragraphbounds.data[idx + 1] : teststrlen;
		//printf("bound: %d - %d\n", start, end);
		cv_clear(&buffer);
		ReflowText(&out[start], end - start, 80, &buffer, 0);

		printf("%s", buffer.data);

	}

	intstack_destroy(&paragraphbounds);

	cv_destroy(&buffer);
	free(out);
	return 0;
}
