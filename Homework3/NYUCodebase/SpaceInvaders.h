#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <algorithm>
#include "Sprite.h"
#include <iostream>
using namespace std;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER, STATE_WINNER };

class SpaceInvaders {
public:
	SpaceInvaders();
	void drawText(const char* image_path, string text, float x, float y, float size, float r, float g, float b, float a);
	void setup();
	void playerShoot(Sprite obj);
	void enemyShoot(Sprite obj);
	GLuint LoadTexture(const char *image_path);
	void processEvents();
	void collisons();
	void renderMenu();
	void renderGame();
	void renderGameOver();
	void renderWinner();
	void Render();
	bool Update();
private:
	bool done;
	int state;
	int bulletIndex;
	int score;
	float playerReload = 0;
	float enemyReload = 0;
	float elapsed;
	float gameclock;

	Sprite player;
	vector<Sprite> playerBullets;
	vector<Sprite> enemyBullets;
	vector<Sprite> aliens;
	SDL_Event event;
	SDL_Window* displayWindow;
	const Uint8 *keys;
	GLuint font;

};