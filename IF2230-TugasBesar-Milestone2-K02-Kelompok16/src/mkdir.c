#include "kernel.h"
#define EXISTS -2
//Jessnat in charge pergi sana
//Merusak kode aja kerjaanya -3-


char cmpArray(char * arr1, char * arr2, int length);

void main() {
    char buffer[MAX_FILENAME + 1];
    char argv[4][32];
    char currentdir;
    char argc;
    char mode = 0;
    int i,success;
    char dest;
    interrupt(0x21, 0x21, &currentdir, 0, 0);
    interrupt(0x21, 0x22, &argc, 0, 0);


    for (i = 0; i < argc; i++) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }
    //printString("STRESSSSSSY");
    if (argc > 0) {
        interrupt(0x21, (currentdir << 8) | 0x08, argv[0], &success, 0);
        if (success == EXISTS) {
            interrupt(0x21, 0x0, "Directory exists !!!!\n", 0, 0);         
        } else if (success == NOT_FOUND) {
            interrupt(0x21, 0x0, "Directory not found !!!\n", 0, 0);
        }
    }


    interrupt(0x21, (0x00 << 8) | 0x07, &success, 0, 0);
}