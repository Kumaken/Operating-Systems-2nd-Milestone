#include "kernel.h"

void main() {
    int i;
    char curdir;
    char argc;
    char argv[4][16];
    int succ;
    char buff[MAX_FILENAME + 1];
    char dirs[SECTOR_SIZE];
    int j;
    char files[SECTOR_SIZE];

    interrupt(0x21, 0x21, &curdir, 0, 0); // interrupt untuk getCurdir
    interrupt(0x21, 0x22, &argc, 0, 0); // interrupt untuk getArgc
    i = 0;
    while (i<argc) {
        interrupt(0x21, 0x23, i, argv[i], 0); // interrupt untuk getArgv
        i++;
    }
    interrupt(0x21, 0x02, dirs, DIRS_SECTOR);
    interrupt(0x21, 0x02, files, FILES_SECTOR);
    interrupt(0x21, 0x0, "directories:\n", 0, 0);

    // looping terhadap entry list di sector dirs
    for (i = 0; i < MAX_DIRS; i++) {
        if ((dirs[i * ENTRY_LENGTH + 1] != '\0') && (dirs[i * ENTRY_LENGTH] == curdir)) {
            j = 0;
            while ((j < MAX_FILENAME) && (dirs[i * ENTRY_LENGTH + 1 + j] != '\0')) {
                buff[j] = dirs[i * ENTRY_LENGTH + 1 + j];
                j++;
            }
            buff[j] = '\0';
            interrupt(0x21, 0x0, " - ", 0, 0);
            interrupt(0x21, 0x0, buff, 0, 0);
            interrupt(0x21, 0x0, "\n", 0, 0);
        }
    }
    // printString
    interrupt(0x21, 0x0, "files:\n", 0, 0); 
    // looping terhadap entry list di sector files
    for (i = 0; i < MAX_FILES; i++) {
        if ((files[i * ENTRY_LENGTH + 1] != '\0') && (files[i * ENTRY_LENGTH] == curdir)) {
            j = 0;
            while ((j < MAX_FILENAME) && (files[i * ENTRY_LENGTH + 1 + j] != '\0')) {
                buff[j] = files[i * ENTRY_LENGTH + 1 + j];
                j++;
            }
            buff[j] = '\0';
            interrupt(0x21, 0x0, " - ", 0, 0);
            interrupt(0x21, 0x0, buff, 0, 0);
            interrupt(0x21, 0x0, "\n", 0, 0);
        }
    }
    //Shift 0x00 by 8 -> printString success memory address OR terminate program:
    interrupt(0x21, (0x00 << 8) | 0x07, &succ, 0, 0);
}
