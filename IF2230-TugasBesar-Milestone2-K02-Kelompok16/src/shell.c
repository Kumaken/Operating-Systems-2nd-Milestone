/*filename : shell.c*/

//Preprocessing Matters:
	#define NOT_FOUND -1
	#define FALSE 0
	#define TRUE 1
	#define EMPTY 0x00
	#define SUCCESS 0
	#define MAX_FILENAME 15
	#define INSUFFICIENT_ENTRIES -3
	#define ALREADY_EXISTS -2
	#define INSUFFICIENT_SECTORS 0
	#define USED 0xFF

//Header functions:
	extern int interrupt(int num, int ax, int bx, int cx, int dx);
	void testRead(char **argv, int argc);
	void readString(char *);
	int strcmp(char *a, char *b, int length);
	void splitCommand(char** argv, char* buffer, char* argc);
	void printString(char *);

int main() {
	// Variable
	char argc;
	char* argv[64];
	int success;
	char curdir, usedir;
	char buffer[512];
	
	interrupt(0x21, 0x21, &curdir,0,0);
	argc = 0;
	printString("\r\n$ ");
	// Read the buffer
	readString(buffer);
	printString("\r\n");
	// Split commands
	splitCommand(argv, buffer, &argc);
	argc--;

	// Put args
	if (strcmp(buffer, "cd", 2)) {
		if (argc == 0) {
			usedir = 0xFF;
			curdir = usedir;
			printString("Jesnat: Kamu ada di root <3\r\n");
			interrupt(0x21, 0x20, curdir,0,0);
		} else {
			if (argv[0][0] == '/') {
				usedir = 0xFF;
			} else {
				usedir = curdir;
			}
			interrupt(0x21, 0x30, argv[0], &success, &usedir);
			
			// Check if success
			if (success == SUCCESS) {
				printString("Jesnat: Kamyu sekarang ada di... \"");
				printString(argv[0]);
				printString("\" directory\r\n");
				curdir = usedir;
				interrupt(0x21, 0x20, curdir,0,0);
			} else {
				printString("Jesnat: Fatal Error, gagal pindah ke directory mu lahhh\r\n");
			}
		}
	} else if (buffer[0] != '\0') {
		/*interrupt(0x21, 0x0, "HHMMM\n",0, 0);
		interrupt(0x21, 0x20, curdir, argc, argv);

		interrupt(0x21, 0x0, "PARAM1\n",0, 0);
		interrupt(0x21, 0x0, curdir,0, 0);
		interrupt(0x21, 0x0, "PARAM2\n",0, 0);
		interrupt(0x21, 0x0, argc,0, 0);
		interrupt(0x21, 0x0, "PARAM3\n",0, 0);
		interrupt(0x21, 0x0, argv,0, 0);*/

		interrupt(0x21, 0x20, curdir, argc, argv);
		interrupt(0x21, 0xFF << 8 | 0x06, buffer, 0x2000, &success);
	}
	
	interrupt(0x21, 0x07, &success,0,0);
}

void printString(char *str) {
	interrupt(0x21, 0, str,0,0);
}

void readString(char *str) {
	interrupt(0x21, 1, str, 0, 0);
}

// Split command
void splitCommand(char *argv[], char *buffer, char* argc) {
	int idx;
	char c;

	while (*buffer != '\0') {
		while (*buffer == ' ') {
			*buffer++ = '\0';
		}
		// Get the first arg
		if (*argc > 0) {
			argv[*argc - 1] = buffer;
		}
		while (*buffer != '\0') {
			if (*buffer == ' ')
				break;
			buffer++;
		}
		(*argc)++;
	}
}

void testRead(char **argv, int argc) {
	int i = 0;
	while (i<argc) {
		printString(argv[i]);
		printString("\r\n");
		i++;
	}
}

// String comparison
int strcmp(char *a, char *b, int length){
	int idx = 0;
	while (idx<length){
		if (a[idx] == '\0' || b[idx] == '\0') {
			return 0;
		}
		if (a[idx] != b[idx]) {
			return 0;
		}
		idx++;
	}
	return 1;
}