#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define handle_error_en_(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
    int 	  timeout;
    long int  time_ss;
	struct timespec time_now_ns;

};

static void handle_error_en(int error_code, char *message)
{
	if (error_code != 0)
	{
		handle_error_en_(error_code, message);
	}
}


/* Thread start function: display address near top of our stack,
and return upper-cased copy of argv_string */

static void * thread_start(void *arg)
{
	struct thread_info *tinfo = arg;
    char *uargv, *p;
	struct timespec ts, time_now_ns;

	printf("Thread %d (%s). Top of stack near %p. Task wait time %d ms.\n", tinfo->thread_num, tinfo->argv_string, &p, tinfo->timeout);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
    {
    	handle_error("strdup");
    }
    for (p = uargv; *p != '\0'; p++)
    {
    	*p = toupper(*p);
    }

    while(1)
    {
//    	long int time_now = time(NULL );

    	clock_gettime(CLOCK_REALTIME, &time_now_ns);

    	long int nsec_diff = time_now_ns.tv_nsec - tinfo->time_now_ns.tv_nsec;
    	long int sec_diff = time_now_ns.tv_sec - tinfo->time_now_ns.tv_sec;
    	long int ms_diff;
    	double proc;

    	if (nsec_diff < 0)
    	{
    		sec_diff--;
    		nsec_diff += 1000000000L;
    	}

    	ms_diff = sec_diff * 1000 + nsec_diff / 1000;


    	proc = (  ((double) ms_diff - (double) tinfo->timeout) / ((double)tinfo->timeout) ) * 100;


    	printf("Thread %d (%s). Task wait time %d ms. Time diff %ld. %.1f%%\n", tinfo->thread_num, tinfo->argv_string, tinfo->timeout, ms_diff, proc) ;

    	tinfo->time_now_ns.tv_sec = time_now_ns.tv_sec;
    	tinfo->time_now_ns.tv_nsec = time_now_ns.tv_nsec;


    	ts.tv_sec = tinfo->timeout / 1000;
    	ts.tv_nsec = (tinfo->timeout % 1000) * 1000;

    	nanosleep(&ts, NULL);
    }

    return uargv;
}

int main(int argc, char *argv[])
{
	int s, tnum, opt, num_threads;
	struct thread_info *tinfo;
	pthread_attr_t attr;
	int stack_size;
	void *res;

    /* The "-s" option specifies a stack size for our threads */

    stack_size = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
    	switch (opt) {
        	case 's':
        		stack_size = strtoul(optarg, NULL, 0);
                break;

            default:
            	fprintf(stderr, "Usage: %s [-s stack-size] TASK_NAME TASK_SLEEP_TINE...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
	}

	num_threads = (argc - optind) / 2;

	/* Initialize thread creation attributes */
	s = pthread_attr_init(&attr);
   	handle_error_en(s, "pthread_attr_init");

	if (stack_size > 0)
	{
		s = pthread_attr_setstacksize(&attr, stack_size);
		handle_error_en(s, "pthread_attr_setstacksize");
	}

	/* Allocate memory for pthread_create() arguments */
	tinfo = calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL)
    {
    	handle_error("calloc");
    }

    /* Create one thread for each command-line argument */
	for (tnum = 0; tnum < num_threads; tnum++)
	{
		char *ptr;
		tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum * 2];
        tinfo[tnum].timeout = strtol( argv[optind + tnum * 2 + 1], &ptr, 10);

        /* The pthread_create() call stores the thread ID into
    	corresponding element of tinfo[] */
        s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start, &tinfo[tnum]);
   		handle_error_en(s, "pthread_create");
	}


     /* Destroy the thread attributes object, since it is no
     longer needed */
	 s = pthread_attr_destroy(&attr);
   	 handle_error_en(s, "pthread_attr_destroy");

     /* Now join with each thread, and display its returned value */
     for (tnum = 0; tnum < num_threads; tnum++)
     {
    	 s = pthread_join(tinfo[tnum].thread_id, &res);
       	 handle_error_en(s, "pthread_join");

         printf("Joined with thread %d; returned value was %s\n", tinfo[tnum].thread_num, (char *) res);
         free(res);      /* Free memory allocated by thread */
     }

     free(tinfo);
     exit(0);
}
