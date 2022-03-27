all:
	gcc alloc.h alloc.c -o alloc
cmnts :
	doxygen -g conf 
	doxygen conf 
