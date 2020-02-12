#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "textutil.h"
#include "charvector.h"
#include <sys/time.h>
#include <time.h>

const char* teststr = "Each iteration of the dynamic `red`programming scheme`default` can also be seen as filling in a matrix,\nwhere a cell adds up the least overall cost to a subproblem (a column minimum) and a penalty.\nA concave weight function implies that the matrix is totally monotone and in 1987 Shor, Moran, Aggarwal, Wilber and Klawe devised an algorithm which finds the row maxima of such matrix in linear time.\n\nEven though SMAWK can be modified to find column minima instead, it is not possible to apply it directly to this \"on-line\" matrix as it might try to evaluate a not \"available\" cell, i.e. a cell dependent on some yet unknown column minimum. However, Wilber came up with an algorithm in 1988 which \"pretends\" to know the minima and still runs in O(n) time. An \"ordered\" algorithm which obeys the availability of the matrix as presented by Aggarwal and Tokuyama in 1998 follows.`default`\n\nShe is asleep: good wench, let's sit down quiet,\nFor fear we wake her: softly, gentle Patience.\n\nThe vision. Enter, solemnly tripping one after another, six personages, clad in white robes, wearing on their heads garlands of bays, and golden vizards on their faces; branches of bays or palm in their hands. They first congee unto her, then dance; and, at certain changes, the first two hold a spare garland over her head; at which the other four make reverent curtsies; then the two that held the garland deliver the same to the other next two, who observe the same order in their changes, and holding the garland over her head: which done, they deliver the same garland to the last two, who likewise observe the same order: at which, as it were by inspiration, she makes in her sleep signs of rejoicing, and holdeth up her hands to heaven: and so in their dancing vanish, carrying the garland with them. The music continues.Each iteration of the dynamic `red`programming scheme`default` can also be seen as filling in a matrix,\nwhere a cell adds up the least overall cost to a subproblem (a column minimum) and a penalty.\nA concave weight function implies that the matrix is totally monotone and in 1987 Shor, Moran, Aggarwal, Wilber and Klawe devised an algorithm which finds the row maxima of such matrix in linear time.\n\nEven though SMAWK can be modified to find column minima instead, it is not possible to apply it directly to this \"on-line\" matrix as it might try to evaluate a not \"available\" cell, i.e. a cell dependent on some yet unknown column minimum. However, Wilber came up with an algorithm in 1988 which \"pretends\" to know the minima and still runs in O(n) time. An \"ordered\" algorithm which obeys the availability of the matrix as presented by Aggarwal and Tokuyama in 1998 follows.`default`\n\nShe is asleep: good wench, let's sit down quiet,\nFor fear we wake her: softly, gentle Patience.\n\nThe vision. Enter, solemnly tripping one after another, six personages, clad in white robes, wearing on their heads garlands of bays, and golden vizards on their faces; branches of bays or palm in their hands. They first congee unto her, then dance; and, at certain changes, the first two hold a spare garland over her head; at which the other four make reverent curtsies; then the two that held the garland deliver the same to the other next two, who observe the same order in their changes, and holding the garland over her head: which done, they deliver the same garland to the last two, who likewise observe the same order: at which, as it were by inspiration, she makes in her sleep signs of rejoicing, and holdeth up her hands to heaven: and so in their dancing vanish, carrying the garland with them. The music continues.";

const char* teststr2 = "The national PCUSA governing rules allowed for a form of \"attack from afar\", in which an out-of-region antagonist could simply mail or fax allegations of church constitutional violations to the local church governing body, forcing pastors to be put on trial for heresy – the \"heresy\" here typically one of \"you’re openly gay\" or \"you officiated at a same-sex wedding\". Attacks like this were an attack on people's jobs and physical security: if you can strip a reform-oriented pastor of their PCUSA ordination, you also strip them of their employment and their health insurance, because you can’t serve as a Presbyterian pastor if you’ve been stripped of your ordination as the result of a church judicial action.";


float TimeDiffSecs(struct timespec* b, struct timespec* a)
{
	return (b->tv_sec - a->tv_sec) + (b->tv_nsec - a->tv_nsec)/1000000000.0;
}

int main(int argc, char** argv)
{

	const int TESTS = 1000;
	cv_t reflowed;
	cv_init(&reflowed, strlen(teststr) + 1);

	size_t idx = 0;

	struct timespec timebegin, timeend;
	float sptime = 0.f, bintime = 0.f;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timebegin);
	for(idx = 0; idx < TESTS; ++idx)
	{
		cv_clear(&reflowed);
		ReflowText(teststr, strlen(teststr), &reflowed, 80, 0);
	}
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeend);
	printf("%s", reflowed.data);
	sptime = TimeDiffSecs(&timeend, &timebegin);


	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timebegin);
	for(idx = 0; idx < TESTS; ++idx)
	{
		cv_clear(&reflowed);
		ReflowTextBinary(teststr, strlen(teststr), &reflowed, 80, 0);
	}
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeend);
	printf("%s", reflowed.data);
	bintime = TimeDiffSecs(&timeend, &timebegin);

	printf("Shortest-paths time: %f\n"
		"Binary-search time : %f\n", sptime, bintime);
	cv_destroy(&reflowed);
	return 0;
}
