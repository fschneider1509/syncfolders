all: syncfolders clean

syncfolders: main.c
	gcc -g main.c gtk_gui.c consoleprint.c readfolder.c comparefolders.c copyfile.c copyfolder.c -o syncfolders-gtk `pkg-config --libs --cflags gtk+-2.0`

clean:
	#do nothing