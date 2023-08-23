#include <time.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#define TERM_ELEM {0, 0, 0, 0}

int main(int argc, char **argv)
{
	opterr = 0;
	int m_flag = 0;
	struct option options[] = {
		{.name = "local", .has_arg = no_argument, .flag = &m_flag, .val = 'l'},
		{.name = "gm", .has_arg = no_argument, .flag = &m_flag, .val = 'g'},
		TERM_ELEM
	};

	int c;
	int count = 0;
	while ((c = getopt_long(argc, argv, "lg", options, NULL)) != -1){
		if (m_flag){
			++count;
			continue;
		}
		switch (c){
		case 'l':
			m_flag = 'l';
			break;
		case 'g':
			m_flag = 'g';
			break;
		case '?':
			fprintf(stderr, "invalid option %c\n", optopt);
			return 1;
		}
	}

	if (count > 1){
		fprintf(stderr, "Please don't enter more than one flag\n");
		return 1;
	}

	if (optind != argc){
		fprintf(stderr, "No argument required\n");
		return 1;
	}

	struct tm * (*converter)(const time_t *timer) = m_flag == 'g' ? &gmtime : &localtime;

	char str[21] = {0};
	while (1){
		memcpy(str, asctime(converter(&(time_t){time(NULL)})), 20);
		printf("%s\r", str);
		fflush(stdout);
		sleep(1);
	}

	return 0;
}