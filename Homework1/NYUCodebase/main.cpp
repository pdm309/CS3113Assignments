#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path){
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void DrawSprite(GLint texture, float x, float y, float rotation) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 1.0);

	GLfloat quad[] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	bool done = false;

	SDL_Event event;

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	float lastFrameTicks = 0.0f;

	float myRotation = 0.0f;
	float xTran = -1.2f;
	float yTran = -1.2f;

	GLuint pikachu = LoadTexture("pikachu.png");
	GLuint mario = LoadTexture("mario.png");
	GLuint falcon = LoadTexture("falcon.png");
	GLuint marth = LoadTexture("marth.png");

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glMatrixMode(GL_MODELVIEW);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		myRotation += 120.0f * elapsed;
		xTran += 0.2f * elapsed;
		yTran += 0.2f * elapsed;

		//background color
		glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glDisableClientState(GL_COLOR_ARRAY);
		if (xTran <= 0.09) DrawSprite(pikachu, xTran, yTran, myRotation);
		if (xTran >= 0.09 && xTran <= 0.33){
			DrawSprite(falcon, 0.2, 0.4, 0);
			GLfloat marioColors[] = { 1.0, 0.5, 0.0, 1.0, 0.0, 0.0, 1.0, 0.5, 0.0, 1.0, 0.0, 0.0 };
			glColorPointer(4, GL_FLOAT, 0, marioColors);
			glEnableClientState(GL_COLOR_ARRAY);
			DrawSprite(mario, 0.8, 0.7, myRotation);
		}
		else if (xTran <= 0.33){
			glDisableClientState(GL_COLOR_ARRAY);
			DrawSprite(mario, 0.6, 0.5, 0.0);
		}
		else if (xTran > 0.33 && xTran <= 0.62){
			DrawSprite(falcon, 0.6, 0.5, 0);
		}
		else{
			DrawSprite(marth, -0.4, -0.3, 0);
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}