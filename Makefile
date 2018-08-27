#CC=gcc
CFLAGS   = -Wall -Wextra -mtune=native `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lgif -lm

debug:helper.c SDL_gifwrap/SDL_gifwrap.c main.c
	$(CC) $(CFLAGS) -g $? -o gif $(LDFLAGS)
stable:helper.c SDL_gifwrap/SDL_gifwrap.c main.c
	$(CC) $(CFLAGS) $? -o gif $(LDFLAGS)
clean:
	rm -vfr *~ gif
