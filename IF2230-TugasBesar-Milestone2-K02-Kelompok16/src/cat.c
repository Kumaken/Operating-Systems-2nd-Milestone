#include "kernel.h"

// membandingkan elemen array, mengembalikan TRUE (1) jika sama
char cmpArray(char * arr1, char * arr2, int length);
// mengimplementasikan div, yaitu hasil bilangan bulat pembagian antara a dibagi b
int div(int a, int b);

void main() {
    // Variable Declaration:
    int i, j, k;
    char buff[SECTOR_SIZE * MAX_SECTORS];
    int sectors;
    char curdir;
    int succ;
    char argv[4][32];
    char argc;
    char l_buffer[128];
    
    char mode = 0;
    
    // Main algorithm:
    interrupt(0x21, 0x21, &curdir, 0, 0); // interrupt untuk getCurdir
    interrupt(0x21, 0x22, &argc, 0, 0); // interrupt untuk getArgc
    i = 0;
    while (i<argc){
        interrupt(0x21, 0x23, i, argv[i], 0); // interrupt untuk getArgv
        i++;
    }
    if (argc > 0) {
        if (argc > 1) {
            // If command -w exists, change to file writing mode
            if (cmpArray("-w", argv[1], 2)) {
                mode = 1;
            }
        }
        if (mode == 0) {
            // Output the content of a file.
            interrupt(0x21, (curdir << 8) | 0x04, buff, argv[0], &succ);
            if (succ == SUCCESS) {
                interrupt(0x21, 0x0, buff, 0, 0);      
                interrupt(0x21, 0x0, "\n", 0, 0);      
            } else {
                interrupt(0x21, 0x0, "Jesnat: file-mu not found di hatiku <3\n", 0, 0);
            }
        } else if (mode == 1) {
            // Inputs to a file
            for (i = 0; i < SECTOR_SIZE * MAX_SECTORS; i++) {
                buff[i] = '\0';
            }
            interrupt(0x21, 0x0, "Jesnat writing service: tekan enter ya untuk slesai <3\n", 0, 0);
            k = 0;
            interrupt(0x21, 0x01, l_buffer, 0, 0);
            while (l_buffer[0] != '\0') {
                j = 0;
                while (l_buffer[j] != '\0') {
                    buff[k] = l_buffer[j];
                    j++;
                    k++;
                }
                buff[k] = '\n';
                k++;
                interrupt(0x21, 0x01, l_buffer, 0, 0);
            }
            buff[k - 1] = '\0';
            interrupt(0x21, 0x0, "\nJesnat writing service: teks-mu sudah tersaved <3\n", 0, 0);
            sectors = div(k, SECTOR_SIZE) + 1;
            interrupt(0x21, (curdir << 8) | 0x05, buff, argv[0], &sectors);
        }
    }
    //Shift 0x00 by 8 -> printString success memory address OR terminate program:
    interrupt(0x21, (0x00 << 8) | 0x07, &succ, 0, 0);
}

int div(int a, int b) {
    int q = 0;
    while(q*b <=a) {
        q++;
    }
    return q-1;
}

char cmpArray(char * arr1, char * arr2, int length) {
    int i = 0;
    char equal = TRUE;
    while ((equal) && (i < length)) {
        equal = arr1[i] == arr2[i];
        if (equal) {
            if (arr1[i] == '\0') {
                i = length;
            }
        }
        i++;
    }
    return equal;
}