objects = main.o stefshreadline.o stefshparseline.o stefshexecargs.o \
					stefshshellstruct.o

stefsh : $(objects)
	gcc -Wall -o stefsh $(objects) -lpcre2-8 -g -I.

$(objects) : main.h

#.PHONY disassociates clean from a file
.PHONY : clean
clean :
	rm $(objects)
