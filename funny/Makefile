objects = ds.o eval.o funny.o init.o read.o write.o

funny : $(objects)
	cc -g -o funny $(objects)

ds.o : ds.h
eval.o : eval.h init.h read.h
funny.o : ds.h init.h read.h eval.h write.h
init.o : init.h read.h eval.h write.h
read.o : read.h init.h
write.o : write.h init.h

.PHONY : clean
clean :
	rm funny $(objects)
