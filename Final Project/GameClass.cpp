#include "GameClass.h"
#include "entity.h"

#define SHEET_SPRITE_COLUMNS 16
#define SHEET_SPRITE_ROWS 8
#define TILE_SIZE 0.08f
GameClass::GameClass() {
	Init();
	done = false;
	state = STATE_MAIN_MENU;
	lastFrameTicks = 0.0f;
	timeLeftOver = 0.0f;
	fontTexture = LoadTexture("font1.png");
	spriteSheet = LoadTexture("sprites.png");
	readTileMap();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("background.mp3");
	Mix_PlayMusic(music, -1); // repeat it indefinitely
	
	someSound = Mix_LoadWAV("cheer.wav");
	deathSound = Mix_LoadWAV("falling.wav");
	
}

void GameClass::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Paul Merritt's Game", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

}

void GameClass::DrawText(const GLuint &fontTexture,
	const std::string &text,
	const float &x,
	const float &y,
	const float &size,
	const float &spacing,
	const float &r,
	const float &g,
	const float &b,
	const float &a) {
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float texture_size = 1.0 / 16.0;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	std::vector<float> colorData;

	for (size_t i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		vertexData.insert(vertexData.end(),
		{ ((size + spacing) * i) + (-0.5f * size),
		0.5f * size,
		((size + spacing) * i) + (-0.5f * size),
		-0.5f * size,
		((size + spacing) * i) + (0.5f * size),
		-0.5f * size,
		((size + spacing) * i) + (0.5f * size),
		0.5f * size });
		texCoordData.insert(texCoordData.end(),
		{ texture_x,
		texture_y,
		texture_x,
		texture_y + texture_size,
		texture_x + texture_size,
		texture_y + texture_size,
		texture_x + texture_size,
		texture_y });
	}
	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

GLuint GameClass::LoadTexture(const char *imagePath) {
	// load the texture
	SDL_Surface *surface = IMG_Load(imagePath);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		4,
		surface->w,
		surface->h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID; 
}

GameClass::~GameClass() {
	Mix_FreeMusic(music);
	Mix_FreeChunk(someSound);
	Mix_FreeChunk(deathSound);
	SDL_Quit();
}

void GameClass::readTileMap(){
	ifstream infile("level1.txt");
	string line;
	while (getline(infile, line)) {
		if (line == "[header]") {
			if (!readHeader(infile)) {
				return;
			}
		}
		else if (line == "[layer]") {
			readLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			readEntityData(infile);
		}
	}
}

bool GameClass::readHeader(std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while
		(getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}
	if
		(mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else { // allocate our map data
		levelData = new unsigned char*[mapHeight];
		for
			(int i =
			0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}}bool GameClass::readLayerData(std::ifstream &stream) {
	std::string line;
	while (std::getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				std::getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int x = 0; x < mapWidth; x++) {
					std::getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						// be careful, the tiles in this format are indexed from 1 not 0
						levelData[y][x] = val - 1;
					}
					else {
						levelData[y][x] = 12;
					}
				}
			}
		}
	}
	return true;
}

bool GameClass::readEntityData(std::ifstream &stream) {
	std::string line, type;
	while (std::getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			std::istringstream lineStream(value);
			std::string xPosition, yPosition;
			std::getline(lineStream, xPosition, ',');
			std::getline(lineStream, yPosition, ',');
			float placeX = atoi(xPosition.c_str()) / 16 * 0.08f + 0.08f;
			float placeY = atoi(yPosition.c_str()) / 16 * - 0.08f;
			placeEntity(type, placeX, placeY);
		}
	}
	return true;
}
void GameClass::placeEntity(string type, float placeX, float placeY){	if (type == "player"){		player = new Entity(placeX+.19, placeY-0.995, 98, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);	}	if (type == "enemy1"){		enemy1 = new Entity(placeX+0.4, placeY-0.8, 81, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);		enemy1->collidedBottom = false;
		enemy1->collidedLeft = false;
		enemy1->collidedRight = false;
		enemy1->collidedTop = false;	}	if (type == "enemy2"){		enemy2 = new Entity(placeX+3.75, placeY-1.45, 81, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);		enemy2->collidedBottom = false;
		enemy2->collidedLeft = false;
		enemy2->collidedRight = false;
		enemy2->collidedTop = false;	}	if (type == "aimCursor"){		aimCursor = new Entity(0.270000011, -0.995, 50, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);	}	if (type == "enemy3"){		enemy3 = new Entity(placeX, placeY-0.2, 81, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);		enemy3->collidedBottom = false;
		enemy3->collidedLeft = false;
		enemy3->collidedRight = false;
		enemy3->collidedTop = false;	}	if (type == "bullet"){		bullet = new Entity(placeX + .19, placeY - 0.995, 8, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);	}}
void GameClass::Update(float elapsed) {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		
		if (event.type == SDL_MOUSEBUTTONDOWN){
			aimCursor->shoot();
		}
		/*if (event.type == SDL_MOUSEMOTION){
			float aimx = (((float)event.motion.x / 800.0f) * 2.66f) - 1.33f;
			aimCursor->x = event.motion.x;
			printf("aim x = %d\n", aimCursor->x);
			float aimy = (((float)(600 - event.motion.y) / 600.0f) * 2.0f) - 1.0f;
			aimCursor->y = event.motion.y;
			printf("aim y = %d\n", aimCursor->y);
		}*/
		
	}
}



void GameClass::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	// render stuff
	switch (state) {
	case STATE_MAIN_MENU:
		DrawText(fontTexture, "Welcome to My Game!", -1.1f, 0.25f, 0.09f, 0.005f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText(fontTexture, "Use WASD and Q and E and Space", -1.1f, -0.25f, 0.095f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText(fontTexture, "Space to Start", -1.1f, -0.45f, 0.1f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case STATE_GAME_LEVEL:
		renderLevel();
		break;
	case STATE_LOSER:
		DrawText(fontTexture, "You lost.", -0.9f, 0.3f, 0.09f, 0.005f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText(fontTexture, "Press R to retry.", -0.8f, -0.4f, 0.1f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	case STATE_WINNER:
		Mix_PlayChannel(-1, someSound, 0);
		DrawText(fontTexture, "Congratulations!", -0.9f, 0.3f, 0.09f, 0.005f, 1.0f, 1.0f, 1.0f, 1.0f);
		DrawText(fontTexture, "Press ESC to quit.", -0.8f, -0.4f, 0.1f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
	SDL_GL_SwapWindow(displayWindow);



}
bool GameClass::processEvents() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;


	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		player->collidedTop = false;
		player->collidedBottom = false;
		player->collidedLeft = false;
		player->collidedRight = false;
		aimCursor->collidedTop = false;
		aimCursor->collidedBottom = false;
		aimCursor->collidedLeft = false;
		aimCursor->collidedRight = false;
		bullet->collidedBottom = false;
		bullet->collidedLeft = false;
		bullet->collidedRight = false;
		bullet->collidedTop = false;

		
		
		
		entityCollisionX(enemy1);
		entityCollisionY(enemy1);
		entityCollisionX(enemy2);
		entityCollisionY(enemy2);
		entityCollisionX(enemy3);
		entityCollisionY(enemy3);

		entityCollisionY(player);
		entityCollisionX(player);
		//entityCollisionX(aimCursor);
		//entityCollisionY(aimCursor);
		entityCollisionY(bullet);
		entityCollisionX(bullet);
		Render();
		FixedUpdate();
		
		Update(elapsed);
		
		
	}
	timeLeftOver = fixedElapsed;
	

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	
	//if the state is main menu press space to start the game
	if (state == STATE_MAIN_MENU){
		if (keys[SDL_SCANCODE_SPACE]){
			state = STATE_GAME_LEVEL;
		}
		// if you want to quit press esc
		else if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
	}
	else if (state == STATE_GAME_LEVEL ){
		if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
		if (player->collidesWith(enemy1)){
			//coin = new Entity(coin->x, coin->y, 12, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);
			//Mix_PlayChannel(-1, someSound, 0);
			hit1 = true;
		}
		if (player->collidesWith(enemy2)){
			//enemy1 = new Entity(enemy1->x, enemy1->y, 12, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);
			//Mix_PlayChannel(-1, someSound, 0);
			hit2 = true;
		}
		if (player->collidesWith(enemy3)){
			//door = new Entity(door->x, door->y, 12, SHEET_SPRITE_COLUMNS, SHEET_SPRITE_ROWS, 0.5, spriteSheet);
			//Mix_PlayChannel(-1, someSound, 0);
			hit3 = true;
		}
		if (bullet->collidesWith(enemy1) && bullet->velocity_x > 0.0 && bullet->velocity_y > 0.0){
			bullet->collidedTop = true;
			enemy1->alive = false;
			enemy1->x -= 100;
			enemy1->y -= 100;
		}
		else {
			enemy1->behaviorAI();
		}
		if (bullet->collidesWith(enemy2) && bullet->velocity_x > 0.0 && bullet->velocity_y > 0.0){
			bullet->collidedTop = true;
			enemy2->alive = false;
			enemy2->x -= 100;
			enemy2->y -= 100;
		}
		else {
			enemy2->behaviorAI();
		}
		if (bullet->collidesWith(enemy3) && bullet->velocity_x > 0.0 && bullet->velocity_y > 0.0){
			bullet->collidedTop = true;
			enemy3->alive = false;
			enemy3->x -= 100;
			enemy3->y -= 100;
		}
		else {
			enemy3->behaviorAI();
		}
		if (bullet->collidedTop || bullet->collidedBottom || bullet->collidedLeft || bullet->collidedRight){
			bullet->velocity_x = 0.0;
			bullet->velocity_y = 0.0;
		}
		if (bullet->velocity_x == 0.0 || bullet->velocity_y == 0.0) {
			bullet->x = player->x;
			bullet->y = player->y;
		}
		if ((hit1 || hit2 || hit3)){
			Mix_PlayChannel(-1, deathSound, 0);
			player->scale -= .05;
			if (player->scale <= 0){
				state = STATE_LOSER;
			}
		}
		if ((!enemy1->alive && !enemy2->alive && !enemy3->alive)) {
			state = STATE_WINNER;
		}
	}
	else if (state == STATE_WINNER){
		if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
	}
	else if (state == STATE_LOSER) {
		if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
		else if (keys[SDL_SCANCODE_R]){
			done = false;
			hit1 = false;
			hit2 = false;
			hit3 = false;
			enemy1->alive = true;
			enemy2->alive = true;
			enemy3->alive = true;
			readTileMap();
			state = STATE_GAME_LEVEL;
		}
	}
	return done;

}



float lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t*v1;
}


void GameClass::FixedUpdate(){
	if (hit1 == false && hit2 == false && hit3 == false) {
		player->movement();
		player->velocity_x += player->gravity * FIXED_TIMESTEP;
		player->velocity_y += player->gravity * FIXED_TIMESTEP;
		player->velocity_x = lerp(player->velocity_x, 0.0f, FIXED_TIMESTEP * player->friction_x);
		player->velocity_y = lerp(player->velocity_y, 0.0f, FIXED_TIMESTEP * player->friction_y);
		player->velocity_x += player->acceleration_x * FIXED_TIMESTEP;
		player->velocity_y += player->acceleration_y * FIXED_TIMESTEP;
		player->x += player->velocity_x * FIXED_TIMESTEP;
		player->y += player->velocity_y * FIXED_TIMESTEP;



		enemy1->x += enemy1->velocity_x;
		enemy1->y += enemy1->velocity_y;

		enemy2->x += enemy2->velocity_x;
		enemy2->y += enemy2->velocity_y;

		enemy3->x += enemy3->velocity_x;
		enemy3->y += enemy3->velocity_y;

		aimCursor->aimMovement();
		aimCursor->x = player->x + sin(aimCursor->angle) * 0.5;
		aimCursor->y = player->y + cos(aimCursor->angle) * 0.5;
		bullet->angle = aimCursor->angle;
		//if (bullet->velocity_x == 0.0 && bullet->velocity_y == 0.0) {
		bullet->shoot();
		//}
		if (bullet->velocity_x > 0.0) {
			bullet->x += sin(bullet->angle2) * .05;
		}
		else {
			bullet->x = player->x;
			//bullet->velocity_x = 0.0;
		}
		if (bullet->velocity_y > 0.0) {
			bullet->y += cos(bullet->angle2) * .05;
		}
		else {
			bullet->y = player->y;
			//bullet->velocity_y = 0.0;
		}
	}
		
	
}

void GameClass::getTileCoordinates(float tileX, float tileY, int *gridX, int *gridY) {
	*gridX = (int)(tileX / TILE_SIZE);
	*gridY = (int)(-tileY / TILE_SIZE);
}

float GameClass::mapCollisionX(float x, float y){
	int gridX, gridY;
	getTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 64 || gridY < 0 || gridY > 32){
		return 0.0f;
	}

	if ((levelData[gridY][gridX] == 0) || (levelData[gridY][gridX] == 3)){
		float xCoordinate = (gridX * TILE_SIZE); // -(TILE_SIZE * 1.0f);
		return -x - xCoordinate;
	}
	return 0.0f;
}

float GameClass::mapCollisionY(float x, float y){
	int gridX, gridY;
	getTileCoordinates(x, y, &gridX, &gridY);
	if (gridX < 0 || gridX > 64 || gridY < 0 || gridY > 32){
		return 0.0f;
	}

	if ((levelData[gridY][gridX] == 0) || (levelData[gridY][gridX] == 3)){
		float yCoordinate = (gridY * TILE_SIZE); // -(TILE_SIZE * 1.0);
		return -y - yCoordinate;
	}
	return 0.0f;
}

void GameClass::entityCollisionX(Entity* entity){
	//check left
	float buffer = mapCollisionX(entity->x - entity->width * 0.5, entity->y);
	if (buffer != 0.0f){
		entity->x += 0.0175f;
		entity->velocity_x = 0.0f;
		entity->collidedLeft = true;
	}
	//check right
	buffer = mapCollisionX(entity->x + entity->width*0.5, entity->y);
	if (buffer != 0.0f){
		entity->x -= 0.0175f;
		entity->velocity_x = 0.0f;
		entity->collidedRight = true;
	}
}

void GameClass::entityCollisionY(Entity* entity){
	//check bottom
	float buffer = mapCollisionY(entity->x, entity->y - entity->height*0.5);
	if (buffer != 0.0f){
		entity->y += buffer;
		entity->velocity_y = 0.0f;
		entity->collidedBottom = true;
	}
	//check top
	buffer = mapCollisionY(entity->x, entity->y + entity->height*0.5);
	if (buffer != 0.0f){
		entity->y += buffer - TILE_SIZE;
		entity->velocity_y = 0.0f;
		entity->collidedTop = true;
	}
}

void GameClass::RenderTiles() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, spriteSheet);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			float u = (float)((int)levelData[y][x] % SHEET_SPRITE_COLUMNS) / (float)SHEET_SPRITE_COLUMNS;
			float v = (float)((int)levelData[y][x] / SHEET_SPRITE_COLUMNS) / (float)SHEET_SPRITE_ROWS;
			float spriteWidth = 1.0 / (float)SHEET_SPRITE_COLUMNS;
			float spriteHeight = 1.0 / (float)SHEET_SPRITE_ROWS;
			vertexData.insert(vertexData.end(), {
				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
			});
			texCoordData.insert(texCoordData.end(), {
				u, v,
				u, v + spriteHeight,
				u + spriteWidth, v + spriteHeight,
				u + spriteWidth, v
			});
		}
	}
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, mapHeight * mapWidth * 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void GameClass::renderLevel(){

	glLoadIdentity();
	translateX = (-player->x);
	translateY = (-player->y);
	glTranslatef(translateX, translateY, 0.0);

	// draw the world
	RenderTiles();

	// draw the players
	player->Draw(translateX, translateY);
	if (enemy1->alive) {
		enemy1->Draw(translateX, translateY);
	}
	if (enemy2->alive) {
		enemy2->Draw(translateX, translateY);
	}
	if (enemy3->alive) {
		enemy3->Draw(translateX, translateY);
	}
	aimCursor->Draw(translateX, translateY);
	if (bullet->velocity_x != 0.0 && bullet->velocity_y != 0.0){
		bullet->Draw(translateX, translateY);
	}
	
}