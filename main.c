//BEGIN PROGRAM HEAD
//BEGIN DESCRIPTION
/*
 * loads a gif and show is frames
 * left mouse button flips pause
 * 
 */
//END   DESCRIPTION

//BEGIN INCLUDES
//local headers
#include "helper.h"
#include <gif_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL_gifwrap/SDL_gifwrap.h"
//END   INCLUDES

//BEGIN CPP DEFINITIONS
//END   CPP DEFINITIONS

//BEGIN DATASTRUCTURES
struct vec2{
	double x;
	double y;
};

struct rect{
	struct vec2 position;
	struct vec2 size;
};

struct area{
	SDL_Texture *Texture;
	SDL_Rect dst;
	SDL_Rect src;
	struct rect rect_d;
	struct rect rect_d_old;
};

struct area gif;
struct area gif_canvas;

SDL_Rect clip_rect;
SDL_Rect fill_rect;
//END	DATASTRUCTURES

//BEGIN GLOBALS
int WW;
int WH;

int DSH;			//Display height
int DSW;			//Display widht

int IARH;  			//Image Aspect Ratio horizontal
int IARW;  			//Image Aspect Ratio vertical

int GO;				//Gif Orientation 1=Landscape, 2=Portrait, 3=Rectangle
int WO;				//Win Orientation 1=Landscape, 2=Portrait, 3=Rectangle

GIF_Image *gifA=NULL;

Uint16 i=0;			//gif frame

int gcd_;
int br;
int bg;
int bb;
double current_scale;
int logging=0;
//END   GLOBALS

//BEGIN FUNCTION PROTOTYPES
//BEGIN INIT
void check_display(void);
void center_window(void);
void win_aspect_ratio(void);
//END INIT
//BEGIN SCALING
int  gcd(int, int);
void scale_all(void);
void gif_aspect_ratio(void);
void center_gif_canvas(void);
void scale_gif_canvas(struct area *s_entity);
void scale_gif(struct area *s_entity);
void scale_gif_canvas(struct area *s_entity);
//END SCALING

void draw_gif_canvas(void);
void draw_current_gif_frame(void);
//END	FUNCTION PROTOTYPES

//BEGIN DOCS
//BEGIN GOALS
/* DEFINED PROGRESS GOALS
 * 
 * Getting Background color somehow
 *	- til now it is hardcoded
 * 
 */
//END GOALS

//BEGIN NOTES
/* NOTE
 * gif lib is deep stuff
 * takes some times to understand that completely
 * 
 */
//END NOTES
//END DOCS

//END PROGRAM HEAD

//BEGIN MAIN FUNCTION
int main(int argc, char *argv[])
{

(void)argc;
(void)argv;

//BEGIN INIT
init();
check_display();
center_window();
//BEGIN GIF
int error;
// GifFileType * gif2 = DGifOpenFileName("c-exec.gif", &error);
char gifname[50];
if (argc<2)
	sprintf(gifname, "sine.gif");
else
	sprintf(gifname, argv[1]);

GifFileType * gif2 = DGifOpenFileName(gifname, &error);

if(DGifSlurp(gif2) == GIF_ERROR){
	DGifCloseFile(gif2, &error);
}


// // SDL_Log("%d",gif2->SColorMap->Colors->Red);
br=gif2->SColorMap->Colors->Red;
// SDL_Log("%d",gif2->SColorMap->Colors->Green);
bg=gif2->SColorMap->Colors->Green;
// SDL_Log("%d",gif2->SColorMap->Colors->Blue);
bb=gif2->SColorMap->Colors->Blue;

if ((gifA=GIF_LoadImage(gifname)))
	i=0;
if (gifA->use_background_color){
	if (logging)
		SDL_Log("TRUE");
}

if (logging){
	SDL_Log("frames: %d",gifA->num_frames);
	SDL_Log("delay 0=%d",gifA->frames[0]->delay);
	SDL_Log("delay max=%d",gifA->frames[gifA->num_frames-1]->delay);
}
gif_canvas.rect_d.size.y=(double)gifA->height;
gif_canvas.rect_d.size.x=(double)gifA->width;
if (logging){
	SDL_Log("canvas size x: %.2f",gif_canvas.rect_d.size.x);
	SDL_Log("canvas size y: %.2f",gif_canvas.rect_d.size.y);
}
gif_canvas.Texture = SDL_CreateTextureFromSurface(Renderer, gifA->frames[0]->surface);
gif_aspect_ratio();
//END GIF

SDL_SetWindowTitle(Window, "SDL 2 - Animated GIF");
SDL_ShowWindow(Window);
//clear Background
SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
SDL_RenderClear(Renderer);

SDL_Event event;
int running = 1;
int pause = 0;

//BEGIN TIMING
int delay;
unsigned int startTime, currentTime, frameTime = 0;
startTime=SDL_GetTicks();
delay=gifA->frames[i]->delay;
//END TIMING

//END   INIT


//BEGIN MAIN LOOP
while(running){

//BEGIN EVENT LOOP
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			running = 0;
		}
		if(event.type == SDL_WINDOWEVENT){
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				WW = event.window.data1;
				WH = event.window.data2;
				scale_all();
			}
		}
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if(event.button.button == SDL_BUTTON_RIGHT){
				running=0;
			}
			if(event.button.button == SDL_BUTTON_LEFT){
				pause=pause^1; //XOR - flip var
				if (pause){
					SDL_SetWindowTitle(Window, "- PAUSED -");
				}else{
					SDL_SetWindowTitle(Window, "SDL 2 - Animated GIF");
				}
			}
		}
	}
//END   EVENT LOOP
	currentTime = SDL_GetTicks();
	frameTime = currentTime-startTime;
	startTime = currentTime;
	if (!pause){
		if (delay<=0){
			i++;
			if (i>gifA->num_frames-1)
				i=0;
			delay=gifA->frames[i]->delay;
		}
		delay-=frameTime;
	}
//BEGIN RENDERING
	draw_current_gif_frame();
//END   RENDERING
	SDL_Delay(33);
}

//END   MAIN LOOP

//BEGIN CLEAN UP
if (gif.Texture!=NULL)
	SDL_DestroyTexture(gif.Texture);
if (gifA!=NULL)
	GIF_FreeImage(gifA);
exit_();
//END CLEAN UP
return EXIT_SUCCESS;

}
//END   MAIN FUNCTION

//BEGIN FUNCTIONS
//BEGIN INIT
void check_display(void)
{
	SDL_DisplayMode current;
	static int disp=-1;
	disp = SDL_GetWindowDisplayIndex(Window);
	if (disp>=0){
		if (logging)
			SDL_Log("Display #%d: \n", disp);
	}
	
	int should_be_zero = SDL_GetCurrentDisplayMode(disp, &current);
	
	if(should_be_zero != 0){
		// In case of error...
		if (logging)
			SDL_Log("Could not get display mode for video display #%d: %s", disp, SDL_GetError());
	}
	else{
		if (logging){
			// On success, print the current display mode.
			SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. \n", disp, current.w, current.h, current.refresh_rate);
			SDL_Log("Display #%d:  %d %d \n", disp, current.w, current.h);
		}
	}
	DSH=current.h;
	DSW=current.w;
	
}

void center_window(void)
{
//BEGIN WINDOW SIZE AND POS
WW=DSW/2;
WH=(WW/16)*9;
int x_offset_win=DSW/4;
int y_offset_win=(DSH-WH)/2;

SDL_SetWindowPosition(Window,x_offset_win,y_offset_win);
SDL_SetWindowSize(Window,WW,WH);
//END WINDOW SIZE AND POS
}

void win_aspect_ratio(void)
{
	gcd_= gcd(WW,WH);
	if (logging)
		SDL_Log("Aspect Ratio of Window: %d:%d",WW/gcd_,WH/gcd_);
	
	if (WW/gcd_ > WH/gcd_){
		if (logging)
			SDL_Log("landscape Window");
		WO=1;
	}
	if (WW/gcd_ < WH/gcd_){
		if (logging)
			SDL_Log("portrait Window");
		WO=2;
	}
	if (WW/gcd_ == WH/gcd_){
		if (logging)
			SDL_Log("rectangle Window");
		WO=3;
	}
	if (logging)
		SDL_Log("WO: %d ",WO);
	
}
//END INIT
//BEGIN SCALING
int gcd (int a, int b)
{
	
return (b == 0) ? a : gcd (b, a%b);
	
}

void scale_all(void)
{

win_aspect_ratio();
scale_gif_canvas(&gif_canvas);
current_scale=gif_canvas.rect_d.size.x/(double)gifA->width;
if (logging)
	SDL_Log("current_scale: %f",current_scale);
center_gif_canvas();

// if (i>0)
// 	scale_gif(&gif);


}

void gif_aspect_ratio(void)
{
//	int IARH;  //Image Aspect Ratio horizontal
//	int IARW;  //Image Aspect Ratio vertical

gcd_= gcd(gifA->width,gifA->height);
IARH=gifA->height/gcd_;
IARW=gifA->width/gcd_;
if (logging)
	SDL_Log("Aspect Ratio of gif: %d:%d",IARW,IARH);

if (IARW > IARH){
	if (logging)
		SDL_Log("landscape gif");
	GO=1;
}
if (IARW < IARH){
	if (logging)
		SDL_Log("portrait gif");
	GO=2;
}
if (IARW == IARH){
	if (logging)
		SDL_Log("rectangle gif");
	GO=3;
}
if (logging)
	SDL_Log("GO: %d ",GO);

}
void center_gif_canvas(void)
{

	gif_canvas.rect_d.position.x=((double)WW-gif_canvas.rect_d.size.x)/2;
	gif_canvas.rect_d.position.y=((double)WH-gif_canvas.rect_d.size.y)/2;
	gif_canvas.dst.x=round( gif_canvas.rect_d.position.x );
	gif_canvas.dst.y=round( gif_canvas.rect_d.position.y );

}

void scale_gif_canvas(struct area *s_entity)
{
	//	int IARH;  //Image Aspect Ratio horizontal
	//	int IARW;  //Image Aspect Ratio vertical
	//BEGIN GIF SIZE
	switch(WO) //Win Orientation 1=Landscape, 2=Portrait, 3=Rectangle
	{
		case 1:
			s_entity->rect_d.size.y=WH;
			s_entity->rect_d.size.x=(s_entity->rect_d.size.y/(double)IARH)*(double)IARW;
			if (s_entity->rect_d.size.x>WW){
				s_entity->rect_d.size.x=WW;
				s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			}
			break;
		case 2:
			s_entity->rect_d.size.x=WW;
			s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			if (s_entity->rect_d.size.y>WH){
				s_entity->rect_d.size.y=WH;
				s_entity->rect_d.size.x=(s_entity->rect_d.size.y/(double)IARH)*(double)IARW;
			}
			break;
		case 3:
			s_entity->rect_d.size.x=WW;
			s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			break;
		default:
			SDL_Log("oops, something went utterly wrong!");
	}

	s_entity->src.w=(int)s_entity->rect_d.size.x;
	s_entity->src.h=(int)s_entity->rect_d.size.y;

	s_entity->dst.w=s_entity->src.w;
	s_entity->dst.h=s_entity->src.h;
	//END GIF SIZE
	
}

void scale_gif(struct area *s_entity)
{
	//	int IARH;  //Image Aspect Ratio horizontal
	//	int IARW;  //Image Aspect Ratio vertical
	//BEGIN GIF SIZE
	switch(WO) //Win Orientation 1=Landscape, 2=Portrait, 3=Rectangle
	{
		case 1:
			s_entity->rect_d.size.y=WH;
			s_entity->rect_d.size.x=(s_entity->rect_d.size.y/(double)IARH)*(double)IARW;
			if (s_entity->rect_d.size.x>WW){
				s_entity->rect_d.size.x=WW;
				s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			}
			break;
		case 2:
			s_entity->rect_d.size.x=WW;
			s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			if (s_entity->rect_d.size.y>WH){
				s_entity->rect_d.size.y=WH;
				s_entity->rect_d.size.x=(s_entity->rect_d.size.y/(double)IARH)*(double)IARW;
			}
			break;
		case 3:
			s_entity->rect_d.size.x=WW;
			s_entity->rect_d.size.y=(s_entity->rect_d.size.x/(double)IARW)*(double)IARH;
			break;
		default:
			SDL_Log("oops, something went utterly wrong!");
	}
	
	s_entity->src.w=(int)s_entity->rect_d.size.x;
	s_entity->src.h=(int)s_entity->rect_d.size.y;
	
	s_entity->dst.w=s_entity->src.w;
	s_entity->dst.h=s_entity->src.h;
	//END GIF SIZE
	
}

void draw_gif_canvas(void)
{
	clip_rect.x=gif_canvas.dst.x;
	clip_rect.y=gif_canvas.dst.y;
	clip_rect.w=gif_canvas.dst.w;
	clip_rect.h=gif_canvas.dst.h;
	SDL_RenderSetClipRect(Renderer, &clip_rect);
	SDL_SetRenderDrawColor(Renderer, 0, 77, 143, 255);
	fill_rect=clip_rect;
	SDL_RenderFillRect(Renderer, &fill_rect);
	SDL_RenderCopy(Renderer, gif_canvas.Texture, NULL, &gif_canvas.dst);
	if (logging){
		SDL_Log("x: %d",gif_canvas.dst.x);
		SDL_Log("y: %d",gif_canvas.dst.y);
		SDL_Log("w: %d",gif_canvas.dst.w);
		SDL_Log("h: %d",gif_canvas.dst.h);
	}
}

void draw_current_gif_frame(void)
{
	gif.dst.x=(gifA->frames[i]->left_offset * current_scale)+gif_canvas.dst.x;
	gif.dst.y=(gifA->frames[i]->top_offset  * current_scale)+gif_canvas.dst.y;
	gif.dst.w=(gifA->frames[i]->width       * current_scale);
	gif.dst.h=(gifA->frames[i]->height      * current_scale);
	
	clip_rect=gif.dst;
	SDL_RenderSetClipRect(Renderer, &clip_rect);
	
	SDL_SetRenderDrawColor(Renderer, br, bg, bb, 255);
	fill_rect=clip_rect;
	SDL_RenderFillRect(Renderer, &fill_rect);
	gif.Texture = SDL_CreateTextureFromSurface(Renderer, gifA->frames[i]->surface);
	SDL_RenderCopy(Renderer, gif.Texture, NULL, &gif.dst);
	SDL_RenderPresent(Renderer);
}
//END SCALING
//END   FUNCTIONS
