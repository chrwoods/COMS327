all: monstermap mapandpath mapbuild pathfind

monstermap: monstermap.c
	gcc monstermap.c -o monstermap -lm

mapandpath: mapandpath.c
	gcc mapandpath.c -o mapandpath -lm

mapbuild: mapbuild.c
	gcc mapbuild.c -o mapbuild -lm

pathfind: pathfind.c
	gcc pathfind.c -o pathfind -lm

clean:
	rm -f monstermap mapandpath mapbuild pathfind *~
