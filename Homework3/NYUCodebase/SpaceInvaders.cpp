#include "SpaceInvaders.h"


SpaceInvaders::SpaceInvaders(){
	setup();
}

void SpaceInvaders::drawText(const char* image_path, string text, float x, float y, float size, float r, float g, float b, float a) {
	// changed the drawText function to accept the image_path which then lets it do load texture and drawtext together
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	vector<float> vertexData;
	vector<float> textCoordData;
	vector<float> colorData;
	float textureSize = 1.0 / 16.0f;
	for (int i = 0; i <text.size(); i++) {
		float textX = (float)(((int)text[i]) % 16) / 16.0f;
		float textY = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(
			vertexData.end(), { size*i + -1 * size, size, size*i + -1 * size, -1 * size, size*i + size, -1 * size, size*i + size, size });
		colorData.insert(
			colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		textCoordData.insert(
			textCoordData.end(), { textX, textY, textX, textY + textureSize, textX + textureSize, textY + textureSize, textX + textureSize, textY });
	}
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, textCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4.0);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


GLuint SpaceInvaders::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	SDL_FreeSurface(surface);
	return textureID;
}
void SpaceInvaders::setup(){
	state = STATE_MAIN_MENU;
	bool done = false;
	score = 0;

	//sets up the window and normal stuff 
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Paul Merritt's Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_MODELVIEW);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// make the spaceship/player from the sprite sheet
	player = Sprite(0.0f, -0.95f, 36.0f / 55.0f, 32.0f / 55.0f, 18.0f / 55.0f, 8.0f / 55.0f, true, true, "sheet.png");


	//makes 26 aliens and psuhes them on the alien vector from a sprite sheet
	for (size_t i = 0; i < 26; i++) {
		aliens.push_back(Sprite(-0.9f, 0.6f, 0.0 / 55.0f, 0.0f, 26.0 / 55.0f, 23.0 / 55.0f, true, true, "sheet.png"));
	}

	int sprites = 0;
	//sets the aliens into two rows
	for (size_t i = 0; i < aliens.size(); i++) {
		if (sprites < 13){
			aliens[i].setX(aliens[i].x + sprites*0.15);
			sprites++;
		}

		else if (aliens[i].visible){
			sprites = 0;
			for (size_t i = 13; i < aliens.size(); i++) {
				aliens[i].setY(aliens[i].y - 0.2f);
			}

		}
	}
	//makes the player bullets (max 30 bullets)
	for (size_t i = 0; i < 30; i++) {
		Sprite playerBullet = Sprite(0.0, -0.8, 46.0 / 55.0f, 8.0 / 55.0f, 10.0 / 55.0f, 5.0 / 55.0f, false, true, "sheet.png");
		playerBullets.push_back(playerBullet);
	}
	// makes the enemy bullets (max 60 bullets)
	for (size_t i = 0; i < 30; i++) {
		Sprite enemyBullet = Sprite(0.0, -0.8, 45.0 / 55.0f, 8.0 / 55.0f, 10.0 / 55.0f, 4.0 / 55.0f, false, true, "sheet.png");
		enemyBullets.push_back(enemyBullet);
	}


}

void SpaceInvaders::playerShoot(Sprite player){

	//sets a timer for how fast the player can shoot or reload 
	if (playerReload > 500) {
		//makes the bullet invisble if it pass through the screen edge
		for (int i = 0; i < playerBullets.size(); i++) {
			if (playerBullets[i].y > 1.0f) {
				playerBullets[i].visible = false;
			}
		}
		//increment the bullets
		int counter = 0;
		while ((playerBullets[counter].visible) && counter < 32) {
			counter++;
			if (counter == 32) {
				counter = 0;
			}
		}
		//sets the bullet to the position of the player sprite
		playerBullets[counter].visible = true;
		playerBullets[counter].setX(player.x + (player.width / 5));
		playerBullets[counter].setY(player.y + 0.05f);
		playerReload = 0;
	}

}

void SpaceInvaders::enemyShoot(Sprite enemy){
	// sets the time for the enemy to shoot/reload
	if (enemyReload > 700 && enemy.visible == true) {
		//if the buttlet goes off the edge then it is to the be invisble 
		for (int i = 0; i < enemyBullets.size(); i++) {
			if (enemyBullets[i].y < -1.0f) {
				enemyBullets[i].visible = false;
			}
		}
		// checks if the enemy is still alive and then incremenets the bullet
		int counter = 0;
		while ((enemyBullets[counter].visible) && counter < 32) {
			counter++;
			if (counter == 32) {
				counter = 0;
			}
		}
		//sets the bullet posotion to the enemy postions
		enemyBullets[counter].visible = true;
		enemyBullets[counter].setX(enemy.x);
		enemyBullets[counter].setY(enemy.y - 0.05f);
		enemyReload = 0;
	}

}

void SpaceInvaders::processEvents(){
	keys = SDL_GetKeyboardState(NULL);
	float movement = 0.00065f;
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
	// player movement set to tbe either rght or left 
	else if (state == STATE_GAME_LEVEL){
		if (keys[SDL_SCANCODE_RIGHT]){
			if ((player.x + movement) < 1.0f){
				player.setX(player.x + movement);
			}
		}
		else if (keys[SDL_SCANCODE_LEFT]){
			if ((player.x - movement) > -1.0f){
				player.setX(player.x - movement);
			}
		}
		//player can shoot usuing Space Bar
		else if (keys[SDL_SCANCODE_SPACE]){
			SpaceInvaders::playerShoot(player);

		}
		//can quit game with esc
		else if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;

		}
	}
	// quit game using esc
	else if (state == STATE_GAME_OVER){
		if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
	}
	//win screen quit using esc
	else if (state == STATE_WINNER){
		if (keys[SDL_SCANCODE_ESCAPE]){
			done = true;
		}
	}
}
void SpaceInvaders::collisons(){

	float alienSpacing = 0.35;
	float playerSpacing = 0.1;
	//prcoess to check if the player bullet has hit an enemy
	for (int k = 0; k < playerBullets.size(); k++) {
		for (int j = 0; j < aliens.size(); j++){

			if (((playerBullets[k].x > aliens[j].x && playerBullets[k].x < aliens[j].x + alienSpacing*aliens[j].width) ||
				(playerBullets[k].x + alienSpacing*playerBullets[k].width < aliens[j].x + alienSpacing*aliens[j].width
				&& playerBullets[k].x + alienSpacing*playerBullets[k].width > aliens[j].x)) && ((playerBullets[k].y > aliens[j].y
				&& playerBullets[k].y < aliens[j].y + alienSpacing*aliens[j].height) ||
				(playerBullets[k].y + alienSpacing*playerBullets[k].height < aliens[j].y + alienSpacing*aliens[j].height
				&& playerBullets[k].y + alienSpacing*playerBullets[k].height > aliens[j].y)) && playerBullets[k].visible && aliens[j].visible) {

				//increase score since alien was killed
				score += 100;
				//erase the alien from the vector 
				aliens.erase(aliens.begin() + j);
				//debug testing
				//cout << aliens.size() << endl;
				//makes the bullet that hit the enemy false
				playerBullets[k].visible = false;

				j--;
				// if the vector is empty then player won since all enemy are dead go to winner state
				if (aliens.size() == 0) state = STATE_WINNER;
			}
		}
	}

	//checks if the enemy bullet has hit the player
	for (int k = 0; k < enemyBullets.size(); k++) {

		if (((enemyBullets[k].x > player.x && enemyBullets[k].x < player.x + playerSpacing*player.width) ||
			(enemyBullets[k].x + playerSpacing*enemyBullets[k].width < player.x + playerSpacing*player.width
			&& enemyBullets[k].x + playerSpacing*enemyBullets[k].width > player.x)) && ((enemyBullets[k].y > player.y
			&& enemyBullets[k].y < player.y + playerSpacing*player.height) ||
			(enemyBullets[k].y + playerSpacing*enemyBullets[k].height < player.y + playerSpacing*player.height
			&& enemyBullets[k].y + playerSpacing*enemyBullets[k].height > player.y)) && enemyBullets[k].visible && player.visible) {
			//if the player was killed then game over
			state = STATE_GAME_OVER;
		}
	}


}
//main menu screen 
void SpaceInvaders::renderMenu(){
	drawText("font2.png", "Aliens are invading and attacking our planet!", -0.95f, 0.0f, 0.04f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "Press [SPACE] to Fight Back", -0.95f, -0.2f, 0.06f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "Press [ESC] to Give Up", -0.95f, -0.4f, 0.06f, 1.0f, 1.0f, 1.0f, 1.0f);
}

//draws the aliens and the player for the game also draws the bullets for both enemy and player
void SpaceInvaders::renderGame(){
	player.draw(0.2f);
	for (int i = 0; i < aliens.size(); i++){
		if (aliens[i].visible){
			aliens[i].draw(0.2f);
		}
	}

	for (int i = 0; i < playerBullets.size(); i++){
		if (playerBullets[i].visible){
			playerBullets[i].draw(0.2f);
		}
	}
	for (int i = 0; i < enemyBullets.size(); i++){
		if (enemyBullets[i].visible){
			enemyBullets[i].draw(0.2f);
		}
	}
}

// game over screen
void SpaceInvaders::renderGameOver(){
  	glClearColor(0.0, 0.0, 0.0, 0.0);
	drawText("font2.png", "You were defeated, but you fought admirably", -0.95f, 0.0f, 0.04f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "Your SCORE is: " + to_string(score), -0.95f, -0.2f, 0.06f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "Press [ESC] to Quit.", -0.95f, -0.4f, 0.06f, 1.0f, 1.0f, 1.0f, 1.0f);


}

//winner screen
void SpaceInvaders::renderWinner(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	drawText("font2.png", "The Earth stands! Incredible work!", -0.95f, 0.0f, 0.04f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "Your FINAL SCORE is: " + to_string(score), -0.95f, -0.2f, 0.04f, 1.0f, 1.0f, 1.0f, 1.0f);
	drawText("font2.png", "PRESS [ESC] to go home and partyyyyy", -0.95f, -0.4f, 0.04f, 1.0f, 1.0f, 1.0f, 1.0f);


}

//used to swap states and calls the renders
void SpaceInvaders::Render(){
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state){
	case STATE_MAIN_MENU:
		renderMenu();
		break;
	case STATE_GAME_LEVEL:
		renderGame();
		break;
	case STATE_GAME_OVER:
		renderGameOver();
		break;
	case STATE_WINNER:
		renderWinner();
	}

	SDL_GL_SwapWindow(displayWindow);
}

bool SpaceInvaders::Update(){
	//timing variables
	float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	//increament the reload 
	playerReload++;
	enemyReload++;
	float decimal_of_sec = elapsed - (long)elapsed;
	//std::cout << "Decimal #";
	//std::cout << decimal_of_sec;
	//std::cout << "\n";
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}
		
	processEvents();
	collisons();

	//the enemies began to fire really fast when there were few left. Setting it so that they only fire during 1/10 of a second makes it much less stressful on the player
	if (decimal_of_sec > 0.9)
	{
		//int index = 5;
		//picks a random enemy to shoot by modding how many enemies are left
		if (aliens.size() != 0){
			int RandIndex = rand() % aliens.size();
			enemyShoot(aliens[RandIndex]);
		}
	}
	//sets the speed and player bullets
	float speed = 0.0025f;
	for (int i = 0; i < playerBullets.size(); i++){
		if (playerBullets[i].visible){
			playerBullets[i].setY(playerBullets[i].y + speed);
		}
	}
	//sets the speed and enemy bullets
	for (int i = 0; i < enemyBullets.size(); i++){
		if (enemyBullets[i].visible){
			enemyBullets[i].setY(enemyBullets[i].y - speed);
		}
	}
	Render();
	return done;

}