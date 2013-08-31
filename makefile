all: srv cli
srv: srv.c
	gcc srv.c -o srv
cli: cli.c
	gcc cli.c -o cli
clean:
	rm ./cli ./srv
