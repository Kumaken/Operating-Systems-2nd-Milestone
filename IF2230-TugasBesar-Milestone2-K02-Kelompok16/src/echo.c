//echo.c:

//main program:
void main() {
    /* Variable Declaration */
        char currentdir; // variable menyimpan data penunjuk current directory
    
        char argc; // var menyimpan jumlah argumen
        //var menyimpan nilai-nilai argumen
        char argv[4][16];
        //variable menyimpan integer penunjuk success
        int success;
        
        char buffer[6]; // var menyimpan buffer string

        int i;

    /*NOTE: JANGAN DECLARE VARIABLE SETELAH INTERRUPT*/
    
    //Interrupt untuk getArgc:
    interrupt(0x21, 0x22, &argc, 0, 0);
    //Interrupt untuk getCurdir:
    interrupt(0x21, 0x21, &currentdir, 0, 0);

    //loop sebanyak jumlah argumen
    for (i = 0; i < argc; i++) {
        interrupt(0x21, 0x23, i, argv[i], 0);
    }


    if (argc >= 1) {
        //Print string untuk argV:
        interrupt(0x21, 0x0, argv[0], 0, 0);
        //Print string untuk newline:
        interrupt(0x21, 0x0, "\n", 0, 0);
    }

    //Shift 0x00 by 8 -> printString success memory address OR terminate program:
    interrupt(0x21, (0x00 << 8) | 0x07, &success, 0, 0);
}