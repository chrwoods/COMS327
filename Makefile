rlg327: rlg327.c
	gcc rlg327.c -o rlg327 -lm -lncurses

all: rlg327 monstermap mapandpath mapbuild pathfind ncursed

ncursed: ncursed.c
	gcc ncursed.c -o ncursed -lm -lncurses

monstermap: monstermap.c
	gcc monstermap.c -o monstermap -lm

mapandpath: mapandpath.c
	gcc mapandpath.c -o mapandpath -lm

mapbuild: mapbuild.c
	gcc mapbuild.c -o mapbuild -lm

pathfind: pathfind.c
	gcc pathfind.c -o pathfind -lm

clean:
	rm -f ncursed monstermap mapandpath mapbuild pathfind *~
