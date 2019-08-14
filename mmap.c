#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


#define handle_error_(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PROGRAM_NAME_INDEX 		(0)
#define MODE_INDEX 				(PROGRAM_NAME_INDEX + 1)
#define CYCLE_COUNT_INDEX 		(MODE_INDEX + 1)
#define FILE_NAME_INDEX 		(CYCLE_COUNT_INDEX + 1)
#define OFFSET_INDEX    		(FILE_NAME_INDEX + 1)
#define LENTH_INDEX     		(OFFSET_INDEX + 1)

#define MAX_ARG_CNT (LENTH_INDEX + 1)


static void handle_error(int error_code, char *message)
{
	if (error_code == -1)
	{
		handle_error_(message);
	}
}


int	main(int argc, char *argv[])
{
	char *addr;
	int fd;
	struct stat sb;
	off_t offset, pa_offset;
	size_t length;
	ssize_t s;
	int mode;
	int cycle_count;

	struct timespec time_1_ns, time_2_ns;


	if (argc != MAX_ARG_CNT)
	{
		fprintf(stderr, "%s file mode try_count offset length\n", argv[PROGRAM_NAME_INDEX]);
		exit(EXIT_FAILURE);
	}

	mode = atoi(argv[MODE_INDEX]);
	printf("Mode %d\n", mode);

	cycle_count = atoi(argv[CYCLE_COUNT_INDEX]);
	printf("Cycle %d\n", cycle_count);

	clock_gettime(CLOCK_REALTIME, &time_1_ns);

	length = atoi(argv[LENTH_INDEX]);

	switch (mode)
	{
	case 1:
    	for (int i=0; i<cycle_count; i++)
    	{

			fd = open(argv[FILE_NAME_INDEX], O_RDONLY);
			handle_error(fd, "open");

			//handle_error(fstat(fd, &sb),"fstat"); /* To obtain file size */

			offset = atoi(argv[OFFSET_INDEX]);
			pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
			/* offset for mmap() must be page aligned */

		/*	if (offset >= sb.st_size)
			{
				fprintf(stderr, "offset is past end of file\n");
				exit(EXIT_FAILURE);
			}
*/
			//if (argc == MAX_ARG_CNT)
/*			{

				if (offset + length > sb.st_size)
				{
					length = sb.st_size - offset;	// Can't display bytes past end of file
				}
			}*/
			//else
			//{
			//	length = sb.st_size - offset;  /* No length arg ==> display to end of file */
			//}

			addr = mmap(NULL, length + offset - pa_offset, PROT_READ, MAP_PRIVATE, fd, pa_offset);

			//if (addr == MAP_FAILED)
			//{
			//	handle_error_("mmap");
			//}

		//	s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
/*
			char text [length];
			memcpy(text, addr, length);
			printf("%s", text);
*/
/*
			if (s != length)
			{
				handle_error_("write");
				fprintf(stderr, "partial write");
				exit(EXIT_FAILURE);
			}
*/

			munmap(addr, length + offset - pa_offset);
			close(fd);
    	}
	    break;
	case 2:
		{
		    if (argc == MAX_ARG_CNT)
		    {
		    	for (int i=0; i<cycle_count; i++)
		    	{

					//length = atoi(argv[LENTH_INDEX]);
					char str[100];

					FILE *fp = fopen(argv[FILE_NAME_INDEX], "r");
					if( fgets (str, length, fp)!=NULL ) {
					/* writing content to stdout */
					//	printf("%s", str);
					}
					fclose(fp);
		    	}
		    }
		}
		break;

	}


	clock_gettime(CLOCK_REALTIME, &time_2_ns);


	long int nsec_diff = time_2_ns.tv_nsec - time_1_ns.tv_nsec;
	long int sec_diff = time_2_ns.tv_sec - time_1_ns.tv_sec;
	long int ns_diff;


	if (nsec_diff < 0)
	{
		sec_diff--;
		nsec_diff += 1000000000L;
	}


	printf("\n");
	printf("TIME: %ld s %ld ns\n", sec_diff, nsec_diff);
	printf("TIME: %ld s %ld ms %ld us %ld ns\n", sec_diff, nsec_diff / 1000000, (nsec_diff % 1000000) / 1000, (nsec_diff % 1000));

    exit(EXIT_SUCCESS);
}
