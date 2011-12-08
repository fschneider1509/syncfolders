all: readfolder consoleprint copyfile copyfolder comparefolders syncfolders clean

syncfolders: main.c
	gcc -W -O2 main.c readfolder.o consoleprint.o copyfile.o copyfolder.o comparefolders.o -o syncfolders

readfolder: readfolder.c
	gcc -W -O2 readfolder.c -c

comparefolders: comparefolders.c
	gcc -W -O2 comparefolders.c -c

copyfile: copyfile.c
	gcc -W -O2 copyfile.c -c
	
copyfolder: copyfolder.c
	gcc -W -O2 copyfolder.c -c
	
consoleprint: consoleprint.c
	gcc -W -O2 consoleprint.c -c
	
clean:
	rm *.o