#include "kernel.h"

//Main Program:
void main() {
    //Variable Declaration:
        int i;

        //Variable char list buffer:
        char buffer[MAX_FILENAME + 1];
        //var menyimpan nilai-nilai argumen
        char argv[4][32];
        char currentdir; // variable menyimpan data penunjuk current directory
        char argc; // var menyimpan jumlah argumen
        char args; // var argumen
        //variable menyimpan integer penunjuk success
        int success;

    //Interrupt untuk getCurdir:
    interrupt(0x21, 0x21, &currentdir, 0, 0);
    //Interrupt untuk getArgc:
    interrupt(0x21, 0x22, &argc, 0, 0);

    //loop sebanyak jumlah argumen
    i = 0;
    while (i < argc){
        interrupt(0x21, 0x23, i, argv[i], 0);
        i++;
    }

    if (argc >= 1) {
         
        interrupt(0x21, (currentdir << 8) | 0x09, argv[0], &success, 0);
        //If fails:
        if (!(success == SUCCESS)) {
            interrupt(0x21, (currentdir << 8) | 0x0A, argv[0], &success, 0);
        }

        if (success != SUCCESS) {
            interrupt(0x21, 0x0, "Jesnat : Delete gagal pek!\n", 0, 0);
        }
        else if (success == NOT_FOUND) {
            interrupt(0x21, 0x0, "Jesnat gak bisa menemukan file-nya mas broh.\n", 0, 0);      
        }

    //Shift 0x00 by 8 -> printString success memory address OR terminate program:
    interrupt(0x21, (0x00 << 8) | 0x07, &success, 0, 0);
    }
}