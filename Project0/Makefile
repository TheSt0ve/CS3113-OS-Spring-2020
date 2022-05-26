all: project0
	gcc -o all project0.c

project0: project0.c
	gcc project0.c -o project0

clean:
	rm all project0

zip:
	zip project0.zip *.c Makefile README.md
