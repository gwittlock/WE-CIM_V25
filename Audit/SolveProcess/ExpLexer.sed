1,$ s/exit(.*)/return/g
1,$ s/yy/exp_yy/g
1,$ s/YY/EXP_YY/g
1,$ s/(void) fprintf( stderr,/TRACE(/g
1,$ s/	printf/	TRACE/g
1,$ s/ printf/ TRACE/g
1,$ s/extern int isatty/\/\/ extern int isatty/g
1,$ s/isatty( fileno(file) )/_isatty( _fileno(file) )/g
1,$ s/#include <unistd.h>/\/\/ #include <unistd.h>/g
