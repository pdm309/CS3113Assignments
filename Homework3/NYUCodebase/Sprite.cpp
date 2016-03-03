#include "Sprite.h"
#include <iostream>

Sprite::Sprite(){};
Sprite::Sprite(float xPos, float yPos, float uVal, float vVal, float swidth, float sheight, bool visibility, bool alive, const char* image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	x = xPos;
	y = yPos;
	u = uVal;
	v = vVal;
	width = swidth;
	height = sheight;
	visible = visibility;
	isAlive = isAlive;
	texture = textureID;
}

void Sprite::draw(float scale) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width*-1.0f*scale, height*scale, width*-1.0f*scale, height*-1.0f*scale, width*scale, height*-1.0f*scale, width*scale, height*scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);

}

void Sprite::setX(float newX) {
	x = newX;
}

void Sprite::setY(float newY) {
	y = newY;
}

void Sprite::setU(float newU) {
	u = newU;
}

void Sprite::setV(float newV) {
	v = newV;
}