Doing GTK+ development with Eclipse-cdt

# Introduction #

In my programmer life i've tested a lot of IDE's and for me Eclipse is the best one. With Eclipse I
can use GDB and GCC here with my Gentoo Linux machine. The debbuging view is really great.

This little guide shows you how to develop GTK+ guis with Eclipse-CDT.

# Guide #

1.) Create a new project or use an exisiting one.

2.) Go to **"Project"** > **"Properties"** > **"C/C++ General"** > **"Paths and Symbols"** > Mark **"GNU C"** under **"Langauges"** > **"Add..."**

3.) Then add here each path which is shown by the output of:
  * "pkg-config --cflags gtk+-2.0" 

> This is for example what is shown on my workstation:

> _-pthread -DQT\_SHARED -I/usr/include/gtk-2.0 -I/usr/lib64/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng15 -I/usr/include/qt4 -I/usr/include/qt4/QtGui -I/usr/include/libdrm -I/usr/include/qt4/QtCore_

4.) Now you will be able to use the _autocomplete_ with **Ctrl+Space**.

5.) The onyl problem is, that you have the settings for every GTK+ project. If somebody knows a better way please let me know :).