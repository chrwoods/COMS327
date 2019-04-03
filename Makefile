DEPS = classes.h drats.h mapgen.h mapsl.h priorityq.h pathing.h ascii.h danger.h hero.h

FLAGS = -lm -lncurses #UNUSED

CC = g++

testload: testload.cpp classes.h drats.h loader.h
	$(CC) testload.cpp -o testload -lncurses

#foggy: foggy.cpp $(DEPS)
#	$(CC) foggy.cpp -o foggy -lm -lncurses

rlg327: rlg327.cpp $(DEPS)
	$(CC) rlg327.cpp -o rlg327 -lm -lncurses

all: rlg327 mapbuild pathfind ncursed #foggy

ncursed: ncursed.cpp $(DEPS)
	$(CC) ncursed.cpp -o ncursed -lm -lncurses

#monstermap: monstermap.c DEPRECATED
#	gcc monstermap.c -o monstermap -lm

#mapandpath: mapandpath.c DEPRECATED
#	gcc mapandpath.c -o mapandpath -lm

pathfind: pathfind.cpp $(DEPS)
	$(CC) pathfind.cpp -o pathfind -lm

mapbuild: mapbuild.cpp $(DEPS)
	$(CC) mapbuild.cpp -o mapbuild -lm

clean:
	rm -f rlg327 ncursed pathfind mapbuild *~
