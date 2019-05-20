echo "Jesnat compiles your O.S.!!!"
echo " "

echo "JESNAT MESSAGE: Part 1: dds"
echo " "
dd if=/dev/zero of=floppya.img bs=512 count=2880 
nasm bootload.asm 
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc 
dd if=map.img of=floppya.img bs=512 count=1 seek=256 conv=notrunc 
dd if=files.img of=floppya.img bs=512 count=1 seek=258 conv=notrunc 
dd if=sectors.img of=floppya.img bs=512 count=1 seek=259 conv=notrunc 
echo " "
echo "JESNAT MESSAGE: Compile kernel.c"
echo " "
bcc -Mc -ansi -c -o kernel.o -A-l -Alisting kernel.c 
as86 kernel.asm -o kernel_asm.o 
ld86 -o kernel -d kernel.o kernel_asm.o 
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=1 
echo " "
echo "JESNAT MESSAGE: Compile syscalls .c files"
echo " "
as86 lib.asm -o lib_asm.o 
bcc -ansi -c -o shell.o shell.c
ld86 -o shell -d shell.o lib_asm.o
# bcc -ansi -c -A-l -Als.listing -o ls.o ls.c
bcc -ansi -c -o ls.o ls.c
ld86 -o ls -d ls.o lib_asm.o
bcc -ansi -c -o echo.o echo.c
ld86 -o echo -d echo.o lib_asm.o
#bcc -ansi -c -o argtest.o argtest.c
#ld86 -o argtest -d argtest.o lib_asm.o
bcc -ansi -c -o mkdir.o mkdir.c
ld86 -o mkdir -d mkdir.o lib_asm.o  
bcc -ansi -c -o cat.o cat.c
ld86 -o cat -d cat.o lib_asm.o
bcc -ansi -c -o rm.o rm.c
ld86 -o rm -d rm.o lib_asm.o
echo " "
echo "JESNAT MESSAGE: Load all files, including keyproc2"
echo " "
gcc loadFile.c -std=gnu89 -o loadFile 
#./loadFile test.txt 
./loadFile keyproc2
./loadFile shell
./loadFile ls
./loadFile echo
#./loadFile argtest
./loadFile mkdir
./loadFile cat
./loadFile rm