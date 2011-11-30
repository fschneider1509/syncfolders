all: readfolder consoleprint comparefolders copyfile syncfolders clean

syncfolders: main.c
	gcc -W -g main.c readfolder.o consoleprint.o comparefolders.o copyfile.o -o syncfolders

readfolder: readfolder.c
	gcc -W -g readfolder.c -c

comparefolders: comparefolders.c
	gcc -W -g comparefolders.c -c

copyfile: copyfile.c
	gcc -W -g copyfile.c -c
	
consoleprint: consoleprint.c
	gcc -W -g consoleprint.c -c
	
clean:
	rm *.o