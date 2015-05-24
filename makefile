objects = basic.o command.o ds.o funny.o memory.o operator.o scheme.o
funny : $(objects)
	cc -g -o funny $(objects) -lm

basic.o : ds.h memory.h basic.h
command.o : command.h
ds.o : ds.h memory.h
funny.o : funny.h memory.h scheme.h
memory.o : memory.h basic.h
operator.o : operator.h basic.h memory.h command.h
scheme.o : scheme.h operator.h basic.h memory.h

.PHONY : clean
clean :
	rm funny $(objects)
