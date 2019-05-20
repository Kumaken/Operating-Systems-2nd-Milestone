//Preprocessing Matters:
	#define ARGS_SECTOR 512
	#define MAX_BYTE 256
	#define SECTOR_SIZE 512
	#define MAX_FILES 32
	#define MAX_FILENAME 15
	#define MAX_SECTORS 16
	#define DIR_ENTRY_LENGTH 32
	#define MAP_SECTOR 0x100
	#define DIRS_SECTOR 0x101
	#define FILES_SECTOR 0x102
	#define SECTORS_SECTOR 0x103
	#define TRUE 1
	#define FALSE 0
	#define INSUFFICIENT_SECTORS 0
	#define NOT_FOUND -1
	#define ALREADY_EXISTS -2
	#define INSUFFICIENT_ENTRy -3
	#define EMPTY 0x00
	#define USED 0xFF
	#define SUCCESS 0
	#define NULL ((void*)0)

// Struct declaration:
	//1. Data structure untuk menyimpan objek dari list file dan directory
	struct Entity
	{
		char parent;
		char filename[MAX_FILENAME];
	};

	//2. Data structure untuk menyimpan sector
	struct sectorEntity
	{
		char sectors[MAX_SECTORS];
	};

// Function headers:
	void handleInterrupt21 (int AX, int BX, int CX, int DX);
	void printString(char *string);
	void readString(char *string);
	int mod(int a, int b);
	int div(int a, int b);
	void readSector(char *buffer, int sector);
	void writeSector(char *buffer, int sector);
	void readFile(char *buffer, char *path, int *result, char parentIndex);
	void checkPath(char* path, struct Entity* entry, int* result, int *parIdx, char* file, int* length);
	int readFileDirName(char* path, int* idx, int* length, char* filename, char* isDir);
	char checkFileName(int i,char parIdx, struct Entity* buf, char* file, int length);
	void checkDir(char* path, int* result, int* parIdx);
	void clear(char *buffer, int length);
	void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
	void executeProgram(char *path, int segment, int *result, char parentIndex);
	void terminateProgram(int *result);
	void makeDirectory(char *path, int *result, char parentIndex);
	void putArgs (char curdir, char argc, char **argv);
	void getCurdir (char *curdir);
	void getArgc (char *argc);
	void getArgv (char index, char *argv);
	void deleteFileByIdx(struct Entity *entry, struct sectorEntity *sectors, char* map, int dirIdx);
	void deleteFile(char *path, int *result, char parentIndex);
	void deleteDirectory(char *path, int *success, char parentIndex);
	void checkInsufficientEntry(struct Entity* entry, int* success, int* idx);

// Main Kernel program:
int main() {
	static char buffer[SECTOR_SIZE];
	int success;

	// To clear the windows
	interrupt(0x10, 0x0600, 0x0700, 0, 25 << 8 | 80);
	interrupt(0x10, 0x200, 0, 0, 0, 0);
	printString("Jesslyn Nathania: Hai cantiks, pencet apapun dulu sekarang <3\r\n");
	printString("Jesslyn Nathania: BTW just so you know, aku keterima intern lho!\r\n");
	printString("Jesslyn Nathania: di GDP <3\r\n");
	interrupt(0x16, 0, 0, 0, 0);
	makeInterrupt21();

	putArgs(0xFF, 0, 0);
	interrupt(0x21, 0xFF << 8 | 0x6, "shell", 0x2000, &success);
}

// Helper arithmetic functions:
	int div(int a, int b) {
		int q = 0;
		while (q * b <= a) {
			q = q + 1;
		} 
		return q - 1;
	}

	int mod(int a, int b) {
		while(a >= b){
			a = a - b;
		}
		return a;
	}
	
// Kernel Functions:
	// Determines interrupt type & what function to call based on AL param:
	void handleInterrupt21 (int AX, int BX, int CX, int DX){
		char AL, AH;
		AL = (char) (AX);
		AH = (char) (AX >> 8);
		switch (AL) {
		case 0x00:
			printString(BX);
			break;
		case 0x01:
			readString(BX);
			break;
		case 0x02:
			readSector(BX, CX);
			break;
		case 0x03:
			writeSector(BX, CX);
			break;
		case 0x04:
			readFile(BX, CX, DX, AH);
			break;
		case 0x05:
			writeFile(BX, CX, DX, AH);
			break;
		case 0x06:
			executeProgram(BX, CX, DX, AH);
			break;
		case 0x07:
			terminateProgram(BX);
			break;
		case 0x08:
			makeDirectory(BX, CX, AH);
			break;
		case 0x09:
			deleteFile(BX, CX, AH);
			break;
		case 0x0A:
			deleteDirectory(BX, CX, AH);
			break;
		case 0x20:
			putArgs(BX, CX, DX);
			break;
		case 0x21:
			getCurdir(BX);
			break;
		case 0x22:
			getArgc(BX);
			break;
		case 0X23:
			getArgv(BX, CX);
			break;
		case 0x30:
			checkDir(BX, CX, DX);
			break;
		default:
			printString("Invalid interrupt");
		}
	}

	void printString(char *string) {
		int i = 0;
		char c;
		while(*(string+i) != '\0'){
			c = *(string+i);
			interrupt(0x10, 0xE00+c, 0, 0, 0);
			//if (c == '\r') interrupt(0x10,0xe*0x100+10,0x0,0x0,0x0);
			i++;
		}
	}

	void readString(char *string) {
		char input=0x0;
		int i = 0;
		while(1){
			input = interrupt(0x16,0,0,0,0);
			//interrupt(0x10,0xe*0x100+in,0x0,0x0,0x0);
			if (input == '\r'){              
				*(string+i) = 0x0;
				//printString("\r\n");
				interrupt(0x10,0xe*0x100+13,0x0,0x0,0x0);
				interrupt(0x10,0xe*0x100+10,0x0,0x0,0x0);
				return;
			}
			else if (input == '\b'){
				if (i >= 1){
					*(string+i) = 0x0;
					interrupt(0x10,0xE00+'\b',0,0,0);
					interrupt(0x10,0xE00+'\0',0,0,0);
					interrupt(0x10,0xE00+'\b',0,0,0);
					i--;
				}
			}
			else{
				*(string+i) = input;
				interrupt(0x10, 0xE00+input, 0x0, 0x0, 0x0);
				i++;
			}     
		}
	}

	//Sector I/0 functions:
		void readSector(char *buffer, int sector) {
			interrupt(0x13, 0x201, buffer, div(sector, 36) << 8 | (mod(sector, 18) + 1), mod(div(sector, 18), 2) << 8);
		}

		void writeSector(char* buffer, int sector) {
			interrupt(0x13, 0x301, buffer, div(sector, 36) << 8 | (mod(sector, 18) + 1), mod(div(sector, 18), 2) << 8);
		}

	//Program functions:
		void executeProgram(char *path, int segment, int *result, char parentIndex) {
			char buffer[MAX_SECTORS * SECTOR_SIZE];
			int idxBuf;
			// Read the file first
			readFile(buffer, path, result, parentIndex);
			
			if (*result != SUCCESS) {
				return;
			}
			// Success : putInMemory
			for (idxBuf = 0; idxBuf < sizeof(buffer); idxBuf++) {
				putInMemory(segment, idxBuf, buffer[idxBuf]); 
			}
			// Launch it
			launchProgram(segment);
			*result = SUCCESS;
		}


		char checkFileName(int indexEntry, char parIdx, struct Entity* entry, char* file, int length) {
			int i; //index of file name

			if (parIdx == entry[indexEntry].parent) {
				// Check the folder's name
				for (i = 0; i < length; i++) {
					if (file[i] != entry[indexEntry].filename[i]) {
						return FALSE;
					}
				}
			} else {
				return FALSE;
			}

			if (entry[indexEntry].filename[i] == '\0')
				return TRUE;

			return FALSE;
		}


		void terminateProgram (int *result) {
			// Variable
			char curdir;
			char shell[6];

			shell[0] = 's';
			shell[1] = 'h';
			shell[2] = 'e';
			shell[3] = 'l';
			shell[4] = 'l';
			shell[5] = '\0';

			getCurdir(&curdir);
			putArgs(curdir, 0, 0);
			executeProgram(shell, 0x2000, result, 0xFF);
		}

	//Directory functions:
		int readFileDirName(char* fullPath, int* idx, int* length, char* filename, char* isDirectory) {
			// Testing
			while ((fullPath[*idx] != '\0') && (fullPath[*idx] != '/') && (*length < MAX_FILENAME)) {
				filename[*length] = fullPath[*idx];
				(*length)++;
				(*idx)++;
			}


			if (*length >= MAX_FILENAME) {
				*length = 0;
				return FALSE;
			} else {
				// Check if the type is directory
				*isDirectory = (fullPath[*idx] != '\0'); 
				filename[*length] = '\0';
				return TRUE;
			}
		}


		void clear(char *buffer, int length) {
			int i;
			for(i = 0; i < length; ++i) {
				buffer[i] = EMPTY;
			}
		}

		//Error Handling:
		void checkInsufficientEntry(struct Entity* entry, int* success, int* idx) {
			// Find the sufficient entry

			for (*idx = 0; *idx < MAX_FILES; (*idx)++) {
				if (entry[*idx].filename[0] == '\0') {
					return;
				}
			}
			if ((*idx) == MAX_FILES) {
				*success = INSUFFICIENT_ENTRy;
			}
		}

	// To read the file at a path
	void readFile(char *buffer, char *path, int *result, char parentIndex) {
		// Variable
		struct Entity entry[MAX_FILES];
		struct sectorEntity sector[MAX_FILES];
		int idxPath, dirSec, dirIdx;
		int length = 0;
		char file[MAX_FILENAME];
		char parIdx = parentIndex;

		// Initialize
		idxPath = 0;
		*result = SUCCESS;
		// Read the sector
		readSector(entry, DIRS_SECTOR);
		// Check the path
		checkPath(path, entry, result, &parIdx, file, &length);
		if (*result == NOT_FOUND) {return;}
		
		// Time to read the file
		readSector(entry, FILES_SECTOR);
		// Red the file name
		for (dirIdx = 0; dirIdx < MAX_FILES; dirIdx++) {
			// Check the parent and the name of the file
			if (checkFileName(dirIdx,parIdx, entry, file, length)) {
				// Read the Sector
				readSector(sector, SECTORS_SECTOR);
				for (dirSec = 0; dirSec < MAX_SECTORS && sector[dirIdx].sectors[dirSec] != 0; dirSec++) {
					readSector(buffer + dirSec * SECTOR_SIZE, sector[dirIdx].sectors[dirSec]);
				}
				// SUCCESS!!!
				*result = SUCCESS;
				return;
			}
		}
		// Fail to find -> NOT_FOUND
		*result = NOT_FOUND;
	}


	// To write the file to a path
	void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
		// Variable
		char map[SECTOR_SIZE];
		char file[MAX_FILENAME];
		char parIdx = parentIndex;
		struct sectorEntity sector[MAX_FILES];
		struct Entity entry[MAX_FILES];
		int length = 0, idx = 0;
		int idxPath, idxFile = 0; 
		
		// Read the sector of map and file
		readSector(map, MAP_SECTOR);
		readSector(entry, FILES_SECTOR);
		// Count the zero sector in the map
		for (idx = 0; idx < MAX_BYTE; idx++) {
			if (map[idx] == EMPTY) {
				length++;
			}
		}
		// Check if there are no sufficient sectors
		if (length < *sectors) {
			*sectors = INSUFFICIENT_SECTORS;
			return;
		}
		// Check insufficient entry
		checkInsufficientEntry(entry, sectors, &idxFile);
		if (*sectors == INSUFFICIENT_ENTRy) {return;}
		// Read the dirs sector and check the path
		readSector(entry, DIRS_SECTOR);
		length = 0;
		checkPath(path, entry, sectors, &parIdx, file, &length);
		
		if (*sectors == NOT_FOUND) {return;}
		// Time to read the file 
		readSector(entry, FILES_SECTOR);
		// Check the name if it is exists
		for (idx = 0; idx < MAX_FILES; idx++) {
			// Check the parent
			if (checkFileName(idx, parIdx, entry, file, length)) {
				*sectors = ALREADY_EXISTS;
				return;
			}
		}
		// Clear the entry at index idxFile
		clear(&entry[idxFile], 16);
		entry[idxFile].parent = parIdx;
		// Write to entry file
		for (idx = 0; idx < length; idx++) {
			entry[idxFile].filename[idx] = file[idx];
		}
		// Read the sectors
		readSector(sector, SECTORS_SECTOR);
		idxPath = 0;
		// Write to sectors
		for (idx = 0; idx < MAX_BYTE && idxPath < *sectors; idx++) {
			if (map[idx] == EMPTY) {
				sector[idxFile].sectors[idxPath] = idx;
				map[idx] = USED;
				writeSector(buffer + idxPath * SECTOR_SIZE, idx);
				idxPath++;
			}
		}
		// Write to Sector
		writeSector(entry, FILES_SECTOR);
		writeSector(map, MAP_SECTOR);
		writeSector(sector, SECTORS_SECTOR);
	}

	// If all true : Return the parentIdx of the last file, The name of the file, The length of the name file
	// Result == NOT_FOUND is false
	void checkPath(char* path, struct Entity* entry, int* result, int *parIdx, char* file, int* length) {
		// Variable
		int indexPath;
		int i; //index entry of directory file name
		int isDir = TRUE;
		indexPath = 0;
		// Check the first character
		if (path[indexPath] == '/') {
			indexPath++;
			*parIdx = 0xFF;
		} 

		// Find the folder and move until the file
		while (readFileDirName(path, &indexPath, length, file, &isDir) && (isDir)) {
			if (file[0] == '.' && *(length) == 1) {
				indexPath++;
				*length = 0;
				continue;
			}
			// Check if the file name is correct
			for (i = 0; i < MAX_FILES; i++) {
				// Check the parent and the name of the file
				// If true, stop the iteration
				if (checkFileName(i, *parIdx, entry, file, *length)) {
					*parIdx = (char) i;
					*length = 0;
					break;
				}
			}
			// If not found
			if (i == MAX_FILES) {
				*result = NOT_FOUND;
				return;
			}
			// concatenate the indexPath
			indexPath++;
		} 


		if (isDir) {
			*result = NOT_FOUND;
		}
	}


	void checkDir(char* fullPath, int* result, int* parentIndex) {
		// Variable
		char file[MAX_FILENAME]; 
		struct Entity entry[MAX_FILES];
		int length = 0, dirIdx;

		readSector(entry, DIRS_SECTOR);
		// Check the path
		checkPath(fullPath, entry, result, parentIndex, file, &length);
		if (*result == NOT_FOUND) {return;}
		for (dirIdx = 0; dirIdx < MAX_FILES; dirIdx++) {
			// Check the parent
			if (checkFileName(dirIdx, *parentIndex, entry, file, length)) {
				// Read the sectors sector
				*parentIndex = dirIdx;
				// Return success 
				*result = SUCCESS;
				return;
			}
		}
		*result = NOT_FOUND;
	}

	
	void makeDirectory(char *path, int *result, char parentIndex) {
		// Variable
		char file[MAX_FILENAME];
		char parIdx = parentIndex;
		struct Entity entry[MAX_FILES];
		int dirIdx = 0, idx = 0, length = 0;
		// Read the dirs sector

		readSector(entry, DIRS_SECTOR);
		checkInsufficientEntry(entry, result, &dirIdx);

		if (*result == INSUFFICIENT_ENTRy) {return;}
		// Check the path
		checkPath(path, entry, result, &parIdx, file, &length);

		if (*result == NOT_FOUND) {return;}
		// Check if it is exists
		for (idx = 0; idx < MAX_FILES; idx++) {
			// Check the parent
			if (checkFileName(idx, parIdx, entry, file, length)) {
				*result = ALREADY_EXISTS;
				return;
			}
		} 
		// Read to dir sector
		clear(&entry[dirIdx], 16);
		entry[dirIdx].parent = parIdx;
		for (idx = 0; idx < length; idx++) {
			entry[dirIdx].filename[idx] = file[idx];
		}
		writeSector(entry, DIRS_SECTOR);
		*result = SUCCESS;
	}

//Deletion functions:
	void deleteFileByIdx(struct Entity *entry, struct sectorEntity *sectors, char* map, int dirIdx) {
		//Variable
		int idx = 0;
		// Set the map to empty
		while (sectors[dirIdx].sectors[idx] != 0) {
			// Set the map to empty
			map[sectors[dirIdx].sectors[idx]] = EMPTY;
			idx++;
		}
		// Clear the file entry
		entry[dirIdx].filename[0] = '\0';
		//clear(&entry[dirIdx], 16);
	}

	void deleteFile(char *path, int *result, char parentIndex) {
		//Variable
		char parIdx = parentIndex;
		char file[MAX_FILENAME];
		char map[SECTOR_SIZE];
		struct Entity entry[MAX_FILES];
		struct sectorEntity sectors[MAX_FILES];
		int dirIdx, idxPath = 0, length = 0;

		// Read the dirs sector and map sector
		readSector(entry, DIRS_SECTOR);
		readSector(map, MAP_SECTOR);
		// Check the path of the file
		checkPath(path, entry, result, &parIdx, file, &length);
		if (*result == NOT_FOUND) {return;}
		// Read the files sector
		readSector(entry, FILES_SECTOR);
		
		for (dirIdx = 0; dirIdx < MAX_FILES; dirIdx++) {
			// Check the parent
			if (checkFileName(dirIdx, parIdx, entry, file, length)) {
				// Read the sectors sector
				readSector(sectors, SECTORS_SECTOR);
				deleteFileByIdx(entry, sectors, map, dirIdx);
				// write to the sector
				writeSector(map, MAP_SECTOR);
				writeSector(entry, FILES_SECTOR);
				// Return success 
				*result = SUCCESS;
				return;
			}
		}
		// If all fail
		*result = NOT_FOUND;
	}

	void deleteDirAndFile(char parIdx, struct Entity* filEntry, struct Entity* dirEntry, struct sectorEntity* sectors, char* map) {
		//Variable
		int idx;
		// Delete the file first
		for (idx = 0; idx < MAX_FILES; idx++) {
			if (filEntry[idx].parent == parIdx) {
				deleteFileByIdx(filEntry, sectors, map, idx);
			}
		}
		// Delete the folder and then do recursive
		for (idx = 0; idx < MAX_FILES; idx++) {
			if (dirEntry[idx].parent == parIdx && dirEntry[idx].filename[0] != '\0') {
				dirEntry[idx].parent = 0;
				dirEntry[idx].filename[0] = '\0';
				deleteDirAndFile(idx, filEntry, dirEntry, sectors, map);
			}
		}
	}

	void deleteDirectory(char *path, int *success, char parentIndex) {
		//Variable
		char map[SECTOR_SIZE];
		char file[MAX_FILENAME];
		char parIdx = parentIndex;
		int dirIdx, idx, idxPath = 0, length = 0;
		// Struct
		struct Entity dirEntry[MAX_FILES];
		struct Entity fileEntry[MAX_FILES];
		struct sectorEntity sectors[MAX_FILES];
		// Read the dirs sector
		readSector(dirEntry, DIRS_SECTOR);
		checkPath(path, dirEntry, success, &parIdx, file, &length);
		if (*success == NOT_FOUND) {return;}
		// Read the sector
		readSector(fileEntry, FILES_SECTOR);
		readSector(map, MAP_SECTOR);
		readSector(sectors, SECTORS_SECTOR);
		for (idx = 0; idx < MAX_FILES; idx++) {
			// Check the parent and the file
			if (checkFileName(idx, parIdx, dirEntry, file, length)) {
				//printString(file);
				dirEntry[idx].parent = 0;
				dirEntry[idx].filename[0] = '\0';
				deleteDirAndFile(idx, fileEntry, dirEntry, sectors, map);
				writeSector(map, MAP_SECTOR);
				writeSector(dirEntry, DIRS_SECTOR);
				writeSector(fileEntry, FILES_SECTOR);
				*success = SUCCESS;
				return;
			}
		}
		*success = NOT_FOUND;
	}

	void putArgs (char curdir, char argc, char **argv) {
		// Variable
		char args[SECTOR_SIZE];
		int i= 0, j = 0, p;
		// Clear
		clear(args, SECTOR_SIZE);

		args[0] = curdir;
		args[1] = argc;
		i = 0; j = 0;
		for (p = 2; p < ARGS_SECTOR && i < argc; ++p) {
			args[p] = argv[i][j];
			if (argv[i][j] == '\0') {
				++i;
				j = 0;
			}
			else {
				++j;
			}
		}
		writeSector(args, ARGS_SECTOR);
	}
	
//Getter functions:
	//Get current directory:
	void getCurdir (char *curdir) {
		char args[SECTOR_SIZE];
		readSector(args, ARGS_SECTOR);
		*curdir = args[0];
	}

	//Get argument count:
	void getArgc (char *argc) {
		char args[SECTOR_SIZE];
		readSector(args, ARGS_SECTOR);
		*argc = args[1];
	}
	
	//Get argument lists:
	void getArgv (char index, char *argv) {
		char args[SECTOR_SIZE];
		int i, j, p;
		readSector(args, ARGS_SECTOR);

		i = 0;
		j = 0;
		for (p = 2; p < ARGS_SECTOR; ++p) {
			if (i == index) {
				argv[j] = args[p];
				++j;
			}
			if (args[p] == '\0') {
				if (i == index) {
					break;
				}
				else {
					++i;
				}
			}
		}
	}