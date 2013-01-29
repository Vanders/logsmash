all: logsmash.c
	$(CC)$(CFLAGS) logsmash.c -o logsmash

clean:
	-@rm logsmash

