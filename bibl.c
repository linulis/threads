#include <stdio.h>
#include <time.h>

static struct timespec start_time;
static struct timespec end_time;


void debug_start_time(void)
{
	clock_gettime(CLOCK_REALTIME, &start_time);
}

void debug_end_time(void)
{
	clock_gettime(CLOCK_REALTIME, &end_time);
}

void debug_pritn_time_diff(void)
{
	long int nsec_diff = end_time.tv_nsec - start_time.tv_nsec;
	long int sec_diff = end_time.tv_sec - start_time.tv_sec;
	long int ns_diff;

	if (nsec_diff < 0)
	{
		sec_diff--;
		nsec_diff += 1000000000L;
	}
	printf("A TIME: %ld s %ld ms %ld us %ld ns\n", sec_diff, nsec_diff / 1000000, (nsec_diff % 1000000) / 1000, (nsec_diff % 1000));

}





void print_hello_world (void)
{
	printf("Hello world");
}
