
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window *displayWindow;

struct Paddle
{
	float left;
	float right;
	float top;
	float bottom;

	Paddle(float lt, float rt, float tp, float bt) : left(lt), right(rt), top(tp), bottom(bt) {}
};

struct Ball
{
	float position_x = 0.0f;
	float position_y = 0.0f;
	float speed = 0.0009f;
	float acceleration = 0.000001f;
	float max_speed = .000905f;
	float direction_x = (float)rand();
	float direction_y = (float)rand();
	int skip = 0;

	Ball(float px, float py, float sp, float ac, float dx, float dy) : position_x(px), position_y(py), speed(sp), acceleration(ac), direction_x(dx), direction_y(dy) {}
	void reset()
	{
		position_x = 0.0f;
		position_y = 0.0f;
		speed = 0.0009f;
		direction_x = (float)rand();
		direction_y = (float)rand();
	}
	void move()
	{
		position_x += (speed * direction_x);
		position_y += (speed * direction_y);
	}
};

// texture loading
GLuint load_texture_rgb(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

ShaderProgram setup()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Purple Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1280, 720);



	return ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
}

int main(int argc, char *argv[])
{
	
	ShaderProgram program = setup();
	Mix_Music *music;
	music = Mix_LoadMUS("Dream Land.mp3");
	Mix_FadeInMusic(music, -1, 1000);
	Mix_Chunk *someSound;
	someSound = Mix_LoadWAV("Shell Hit 2.wav");
	Mix_Chunk *someSound2;
	someSound2 = Mix_LoadWAV("Bumper Hit.wav");
	Mix_Chunk *cheer;
	cheer = Mix_LoadWAV("Crowd Cheering.wav");
	Matrix paddle_left_matrix;
	Matrix paddle_right_matrix;
	Matrix ball_matrix;
	Matrix view_matrix;
	Matrix projection_matrix;

	GLuint chartreuse_texture_ID = load_texture_rgb("purple-suede.jpg");

	projection_matrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	SDL_Event event;
	bool done = false;
	bool game_started = false;

	float lastFrameTicks = 0.0f;

	Paddle paddle_left(-3.5f, -3.4f, 0.5f, -0.5f);
	Paddle paddle_right(3.4f, 3.5f, 0.5f, -0.5f);
	Ball ball(0.0f, 0.0f, 0.05f, 0.0025f, (float)rand(), (float)rand());
	
	while (!done)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Controls for the game
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
				done = true;
			if (event.type == SDL_KEYDOWN)
			{
				// left paddle
				if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					if (paddle_left.top < 2.0f){
						paddle_left.top += 0.3f;
						paddle_left.bottom += 0.3f;
						paddle_left_matrix.Translate(0.0f, 0.3f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					if (paddle_left.bottom > -2.0f){
						paddle_left.top -= 0.3f;
						paddle_left.bottom -= 0.3f;
						paddle_left_matrix.Translate(0.0f, -0.3f, 0.0f);
					}
				}

				// right paddle
				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					if (paddle_right.top < 2.0f){
						paddle_right.top += 0.3f;
						paddle_right.bottom += 0.3f;
						paddle_right_matrix.Translate(0.0f, 0.3f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					if (paddle_right.bottom > -2.0f){
						paddle_right.top -= 0.3f;
						paddle_right.bottom -= 0.3f;
						paddle_right_matrix.Translate(0.0f, -0.3f, 0.0f);
					}
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE || event.key.keysym.scancode == SDL_SCANCODE_RETURN)
				{
					if (!game_started)
						game_started = true;
				}
			}
		}
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		program.setModelMatrix(paddle_left_matrix);
		program.setViewMatrix(view_matrix);
		program.setProjectionMatrix(projection_matrix);

		glUseProgram(program.programID);

		float texture_coords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f }; // global texture coordinates

		// left paddle
		float paddle_left_coords[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddle_left_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, chartreuse_texture_ID);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		// right paddle
		program.setModelMatrix(paddle_right_matrix);
		float paddle_right_coords[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddle_right_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, chartreuse_texture_ID);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		// ball
		program.setModelMatrix(ball_matrix);
		float ball_coords[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, chartreuse_texture_ID);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		if (game_started)
		{
			//Mix_Chunk *start;
			//start = Mix_LoadWAV("Start.wav");
			//Mix_PlayChannel(-1, start, 0);
			

			// right wins
			if (ball.position_x - 0.1f <= paddle_left.left)
			{
				
				Mix_PlayChannel(-1, cheer, 0);
				
				game_started = false;
				ball_matrix.Translate(-ball.position_x, -ball.position_y, 0.0f);
				ball.position_x = 0.0f; // for some reason reset() doesn't work :-(
				ball.position_y = 0.0f;
				ball.direction_x = (float)rand() / (float)RAND_MAX;
				ball.direction_y = (float)rand() / (float)RAND_MAX;
				ball.speed = 0.0009f;
				std::cout << "Right player wins!\n"; 

			}

			// left wins
			else if (ball.position_x + 0.1f >= paddle_right.right)
			{


				
				game_started = false;
				ball_matrix.Translate(-ball.position_x, -ball.position_y, 0.0f);
				ball.position_x = 0.0f;
				ball.position_y = 0.0f;
				ball.direction_x = (float)rand() / (float)RAND_MAX;
				ball.direction_y = (float)rand() / (float)RAND_MAX;
				ball.speed = 0.0009f;
				if (ball.skip > 0)
				{
					Mix_PlayChannel(-1, cheer, 0);
					std::cout << "Left player wins!\n";
				}
				ball.skip++;
			}

			// hits top or bottom of screen
			else if (ball.position_y + 0.1f >= 2.0f || ball.position_y - 0.1f <= -2.0f)
			{
				
				Mix_PlayChannel(-1, someSound, 0);
				ball.direction_y *= -1;
				if (ball.speed < ball.max_speed)
				{
					ball.speed += ball.acceleration;
				}
				ball.move();
				ball_matrix.Translate(ball.speed * ball.direction_x, ball.speed * ball.direction_y, 0.0f);
			}

			// hits a paddle
			else if ((ball.position_x - 0.1f <= paddle_left.right && ball.position_y - 0.1f <= paddle_left.top && ball.position_y + 0.1f >= paddle_left.bottom) ||
				(ball.position_x + 0.1f >= paddle_right.left && ball.position_y - 0.1f <= paddle_right.top && ball.position_y + 0.1f >= paddle_right.bottom))
			{
				
				Mix_PlayChannel(-1, someSound2, 0);
				ball.direction_x *= -1;
				ball.speed += ball.acceleration;
				ball.move();
				ball_matrix.Translate((ball.speed * ball.direction_x), (ball.speed * ball.direction_y), 0.0f);
			}

			// general movement
			else
			{
				ball.move();
				ball_matrix.Translate((ball.speed * ball.direction_x), (ball.speed * ball.direction_y), 0.0f);
			}


		}
		
		
		SDL_GL_SwapWindow(displayWindow);
	}

	Mix_FreeChunk(someSound);
	Mix_FreeChunk(someSound2);
	Mix_FreeChunk(cheer);

	Mix_FreeMusic(music);
	SDL_Quit();
	return 0;
}