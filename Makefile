all: readfolder gui consoleprint copyfile copyfolder comparefolders syncfolders clean

syncfolders: main.c
	gcc -W -g main.c readfolder.o consoleprint.o copyfile.o copyfolder.o comparefolders.o gtk_gui.o -o syncfolders-gtk `pkg-config --libs --cflags gtk+-2.0`

gui: gtk_gui.c
	gcc -W -g gtk_gui.c -c `pkg-config --libs --cflags gtk+-2.0`	

readfolder: readfolder.c
	gcc -W -g readfolder.c -c

comparefolders: comparefolders.c
	gcc -W -g comparefolders.c -c `pkg-config --libs --cflags gtk+-2.0`

copyfile: copyfile.c
	gcc -W -g copyfile.c -c `pkg-config --libs --cflags gtk+-2.0`
	
copyfolder: copyfolder.c
	gcc -W -g copyfolder.c -c
	
consoleprint: consoleprint.c
	gcc -W -g consoleprint.c -c
	
clean:
	rm *.o