#Makefile: Sun Jul 22 00:24:15 EST 2012 pknowles */

DEBUG = -g
#OPTIMISE = -O3

CFLAGS = `sdl-config --cflags` $(DEBUG) $(OPTIMISE)
LDFLAGS = `sdl-config --libs` -framework OpenGL -framework GLUT 
EXE = assign1
OBJECTS = sdlbase.o util.o torus.o sphere.o assign1.o 

#default target
all: $(EXE)

#executable
$(EXE) : $(OBJECTS)
	gcc -o $@ $(LDFLAGS) $(OBJECTS)

#general object file rule
%.o : %.c
	gcc -c -o $@ $(CFLAGS) $<

#additional dependencies
sdlbase.o : sdlbase.h
assign1.o : sdlbase.h util.h

#clean (-f stops error if files don't exist)
clean:
	rm -f $(EXE) \
	      $(OBJECTS)

