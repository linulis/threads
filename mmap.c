#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define handle_error_(msg) \
		do { perror(msg); exit(EXIT_FAILURE); } while (0)

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

	if (argc < 3 || argc > 4)
	{
		fprintf(stderr, "%s file offset [length]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	handle_error(fd, "open");

	handle_error(fstat(fd, &sb),"fstat"); /* To obtain file size */

	offset = atoi(argv[2]);
	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	/* offset for mmap() must be page aligned */

	if (offset >= sb.st_size)
	{
		fprintf(stderr, "offset is past end of file\n");
		exit(EXIT_FAILURE);
	}

    if (argc == 4)
    {
    	length = atoi(argv[3]);
        if (offset + length > sb.st_size)
        {
        	length = sb.st_size - offset;	/* Can't display bytes past end of file */
        }
        else
        {
        	length = sb.st_size - offset;  /* No length arg ==> display to end of file */
        }

        addr = mmap(NULL, length + offset - pa_offset, PROT_READ, MAP_PRIVATE, fd, pa_offset);

        if (addr == MAP_FAILED)
        {
        	handle_error_("mmap");
        }

        s = write(STDOUT_FILENO, addr + offset - pa_offset, length);
        if (s != length)
        {
        	handle_error(s, "write");
        	fprintf(stderr, "partial write");
            exit(EXIT_FAILURE);
        }

        munmap(addr, length + offset - pa_offset);
        close(fd);

        exit(EXIT_SUCCESS);
    }
}
