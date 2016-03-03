#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>

class Sprite {
public:
	Sprite();
	Sprite(float x, float y, float u, float v, float width, float height, bool visible, bool isAlive, const char* image_path);

	void draw(float scale);
	void setX(float newX);
	void setY(float newY);
	void setU(float newU);
	void setV(float newV);


	float x;
	float y;
	float u;
	float v;
	float width;
	float height;
	float speed = 0.01f;
	bool visible = false;
	bool isAlive;
	GLuint texture;
};