DEPS = classes.h drats.h mapgen.h mapsl.h priorityq.h pathing.h ascii.h danger.h hero.h loader.h

FLAGS = -lm -lncurses #UNUSED

CC = g++

rlg327: rlg327.cpp $(DEPS)
	$(CC) rlg327.cpp -o rlg327 -lm -lncurses

all: rlg327 mapbuild pathfind ncursed testload colorful

colorful: colorful.cpp $(DEPS)
	$(CC) colorful.cpp -o colorful -lm -lncurses

testload: testload.cpp classes.h drats.h loader.h
	$(CC) testload.cpp -o testload -lncurses

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
	rm -f rlg327 colorful testload ncursed pathfind mapbuild *~
