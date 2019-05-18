#include "sdlbase.h"

/* default SDL window arguments */
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_DEPTH 32
#define DEFAULT_FLAGS (SDL_OPENGL | SDL_RESIZABLE)

#define MILLI_SECOND_MULTIPLIER 1000 // 1s = 1,000 milliseconds
#define NANO_SECOND_MULTIPLIER 1000000 // 1ms = 1,000,000 nanoseconds
#define IDEAL_FRAME_RATE 30.00f

static int quitFlag = 0;
static int wantRedisplay = 0;

const float idealFrameTime = (1.00f/IDEAL_FRAME_RATE)*MILLI_SECOND_MULTIPLIER;

void quit()
{
	/* stop running main loop */
	quitFlag = 1;
}

void idle(float frameTime)
{
	if (frameTime < idealFrameTime) {
		float sleepTime = idealFrameTime - frameTime;

		struct timespec req, rem;
		req.tv_sec = 0;
		req.tv_nsec = 0;

		if (sleepTime > MILLI_SECOND_MULTIPLIER)
			req.tv_sec = (int)(sleepTime/MILLI_SECOND_MULTIPLIER);
		req.tv_nsec = (long)(((int)sleepTime - (int)(req.tv_sec*MILLI_SECOND_MULTIPLIER)) * NANO_SECOND_MULTIPLIER);

		int res;
		while ((res = nanosleep(&req, &rem)) && errno == EINTR) {
			req.tv_sec = rem.tv_sec;
			req.tv_nsec = rem.tv_nsec;
		};

		if (res != 0) {
			perror("nanosleep");
			exit(1);
		}
	}
}

void eventReshape(int w, int h)
{
	/* update viewport and projection - define in client app */
	reshape(w, h);
}

void processEvents()
{
	static int lastMouseX = 0;
	static int lastMouseY = 0;
	SDL_Event event;
	SDL_Surface *screen;
	int videoFlags = DEFAULT_FLAGS;

	/* route all events to specific event handlers */
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				quit();
				break;
			case SDL_VIDEORESIZE:
				screen = SDL_SetVideoMode(event.resize.w, event.resize.h,	
						DEFAULT_DEPTH, videoFlags);
				eventReshape(screen->w, screen->h);
				break;
			case SDL_KEYDOWN:
				keyDown(event.key.keysym.sym);
				break;
			case SDL_KEYUP:
				keyUp(event.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouseDown(event.button.button, 1, lastMouseX, lastMouseY);
				break;
			case SDL_MOUSEBUTTONUP:
				mouseDown(event.button.button, 0, lastMouseX, lastMouseY);
				break;
			case SDL_MOUSEMOTION:
				lastMouseX = event.motion.x;
				lastMouseY = event.motion.y;
				mouseMove(lastMouseX, lastMouseY);
				break;
			default:
				break;
		}
	}
}

int main(int argc, char** argv)
{
	SDL_Surface *screen;
	int videoFlags = DEFAULT_FLAGS;
	unsigned int lastTime, startTime, deltaTime;
#ifdef STEADYFPS
	unsigned int currentTime;
#endif

	/* initialize sdl and opengl */
	videoFlags = DEFAULT_FLAGS;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	screen = SDL_SetVideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT, 
			DEFAULT_DEPTH, videoFlags);
	glEnable(GL_DEPTH_TEST);

	/* initialize the sdl app */
	init();
	eventReshape(screen->w, screen->h);

	glClear(GL_ACCUM_BUFFER_BIT);

	/* the main loop */
	lastTime = SDL_GetTicks();
	while (!quitFlag) {
		/* calculate elapsed time */
		startTime = SDL_GetTicks();
		deltaTime = startTime - lastTime;
		lastTime = startTime;

		/* read sdl events */
		processEvents();

		/* cpu-side logic, movement/animation etc */
		update((float)deltaTime * 0.001);

		/* render results */
		display();

		SDL_GL_SwapBuffers();

#ifdef STEADYFPS
		currentTime = SDL_GetTicks();
		idle((float)(currentTime-startTime));
#endif

	}

	cleanup();
	return 0;
}
