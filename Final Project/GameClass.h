#pragma once

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "entity.h"
#include <SDL_mixer.h>
using namespace std;

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_WINNER, STATE_LOSER , STATE_NEXT_LEVEL};
class GameClass {
public:
	GameClass(int level);
	~GameClass();
	void Init();
	bool processEvents();
	void Render();
	void Update(float elapsed);
	GLuint LoadTexture(const char *image_path);
	void DrawText(const GLuint &fontTexture,
		const std::string &text,
		const float &x,
		const float &y,
		const float &size,
		const float &spacing,
		const float &r,
		const float &g,
		const float &b,
		const float &a);
	void readTileMap();
	void entityCollide(Entity* entity);
	void playerCollidesWithEnemy();
	void updateEnemy();
	void hitEnemy();
	bool readHeader(std::ifstream &stream);
	bool readLayerData(std::ifstream &stream);
	bool readEntityData(std::ifstream &stream);
	void RenderTiles();
	void getTileCoordinates(float tileX, float tileY, int *gridX, int *gridY);
	float mapCollisionX(float x, float y);
	float mapCollisionY(float x, float y);
	void entityCollisionX(Entity* entity);
	void entityCollisionY(Entity* entity);
	void placeEntity(string type, float placeX, float placeY);
	bool winner();
	void renderLevel();
	void FixedUpdate();
	Entity* player;
	Entity* enemy1;
	Entity* enemy2;
	Entity* enemy3;
	Entity* aimCursor;
	Entity* bullet;
	Mix_Music *music;
	Mix_Chunk *someSound;
	Mix_Chunk *deathSound;
	Mix_Chunk *fireSound;
	Mix_Chunk *hitSound;
	bool hit1;
	bool hit2;
	bool hit3;
	int levelSelect;
	vector <Entity*> stat;
	bool done;
	float lastFrameTicks;
	float timeLeftOver;
	SDL_Window* displayWindow;
	SDL_Event event;
	GLuint spriteSheet;
	int state;
	GLuint fontTexture;
	float translateX;
	float translateY;
	float aimX;
	float aimY;
	int mapWidth;
	int mapHeight;
	unsigned char **levelData;
};