
#include "entity.h"
#define PI 3.14159265
Entity::Entity(float x, float y, int index, int col, int rows, float scale, GLuint texture)
	:x(x), y(y), scale(scale), texture(texture) {
	
	u = (float)(index % col) / (float) col;
	v = (float)(index / col) / (float) rows;
	width = (1.0 / col);
	height = (1.0 / rows);
	velocity_x = 0.0f;
	velocity_y = 0.0f;
	acceleration_x = 0.0f;
	acceleration_y = 0.0f;
	gravity = 0.0f;
	friction_x = 0.39f;
	friction_y = 0.59f;
	angle = 0;
	angle2 = 0;
	scale = 1.0f;
	
}
Entity::Entity(){};



void Entity::DrawRectangle(float x, float y, float r1, float r2){
	GLfloat quad[] = { (x - r1), (y + r2),
		(x - r1), (y - r2),
		(x + r1), (y - r2),
		(x + r1), (y + r2) };

	glLoadIdentity();
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawArrays(GL_QUADS, 0, 4);
	//glEnd();

}

void Entity::Draw(const float &translateX, const float &translateY) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glTranslatef(translateX, translateY, 0.0);
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width * -1.0f * scale,
		height * scale,
		width * -1.0f * scale,
		height * -1.0f * scale,
		width * scale,
		height * -1.0f * scale,
		width * scale,
		height * scale };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void Entity::DrawText(const GLuint &fontTexture,
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



void Entity::render(){
	DrawRectangle(x, y, width, height);
}

bool Entity::collidesWith(Entity *entity){
	if (x + (width/4) > (entity->x - ((entity->width))) &&
		(x - (width/4)) < (entity->x + (entity->width)) &&
		(y + (height/4)) > (entity->y - (entity->height)) &&
		(y - (height/4)) < (entity->y + (entity->height)))
	{
		return true;
	}
	return false;
}
float Entity::lerp(float v0, float v1, float t){
	return (1.0 - t)* v0 + t*v1;

}

void Entity::jump(){
	if (collidedBottom){
		velocity_y = .80f;
		y += .001;
		collidedBottom = false;
	}

}
void Entity::FixedUpdate(){

	velocity_y = lerp(velocity_y, 0.0, friction_y * FIXED_TIMESTEP);
	velocity_y += acceleration_y * FIXED_TIMESTEP;
	velocity_y += FIXED_TIMESTEP;
	y += velocity_y*FIXED_TIMESTEP;
	velocity_x = lerp(velocity_x, 0.0, friction_x * FIXED_TIMESTEP);
	velocity_x += acceleration_x * FIXED_TIMESTEP;
	x += velocity_x * FIXED_TIMESTEP;
}

void Entity::movement(){
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) {
		acceleration_x = -3.0;
		velocity_x = -1.0;
	}
	if (keys[SDL_SCANCODE_D] && !keys[SDL_SCANCODE_A]) {
		acceleration_x = 3.0;
		velocity_x = 1.0;

	}
	if (keys[SDL_SCANCODE_S] && !keys[SDL_SCANCODE_W]) {
		acceleration_y = -3.0;
		velocity_y = -1.0;

	}
	if (keys[SDL_SCANCODE_W] && !keys[SDL_SCANCODE_S]) {
		acceleration_y = 3.0;
		velocity_y = 1.0;
	}

	//if no arrows pressed
	if (!keys[SDL_SCANCODE_A] && !keys[SDL_SCANCODE_D]) {
		acceleration_x = 0;
		velocity_x = 0.0;
	}
	if (!keys[SDL_SCANCODE_W] && !keys[SDL_SCANCODE_S]){
		acceleration_y = 0;
		velocity_y = 0.0;
	}
	/*
	//if up and down pressed, no vertical movement
	if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_S]) {
	//acceleration_x = 0;
	acceleration_y = 0;
	velocity_y = 0.0;
	velocity_x = 0.0;
	}
	//if left and right pressed, no horizontal movement
	if (keys[SDL_SCANCODE_A] && keys[SDL_SCANCODE_D]) {
	acceleration_x = 0;
	//acceleration_y = 0;
	velocity_y = 0.0;
	velocity_x = 0.0;
	}
	*/
}
void Entity::aimMovement(){
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_J] && !keys[SDL_SCANCODE_L]) {
		angle -= 5.0*(PI / 180);
	}
	else if ((keys[SDL_SCANCODE_L] && !keys[SDL_SCANCODE_J])) {
		angle += 5.0*(PI / 180);
	}
}

void Entity::shoot(Mix_Chunk* sound){
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_K]) {
		velocity_x = 0.001;
		velocity_y = 0.001;
		angle2 = angle;
		Mix_PlayChannel(-1, sound, 0);
	}
}

void Entity::behaviorAI(){

	/*if (alive){
		velocity_y = 0.005;
		velocity_x = 0.00;
		if (collidedTop){
			velocity_y = 0.0;
			velocity_x = 0.01;
			if (collidedRight){
				velocity_x = -0.01;
				velocity_y = 0.0;
				if (collidedLeft) {
					velocity_x = 0.0;
					velocity_y = -0.005;
				}
			}
		}
		else if (collidedBottom && collidedLeft && collidedRight && collidedTop) {
			collidedTop = false;
			collidedRight = false;
			collidedLeft = false;
			collidedBottom = false;
		}
	}*/

	
	//go down
	//go right
	/*else  if (!collidedRight) {
		velocity_x = 0.001;
		velocity_y = 0.0;
	}
	//go left
	else if (!collidedLeft) {
		velocity_y = 0.0;
		velocity_x = -0.004;
	}
	*/
	//go top
	if (collidedTop == false) {
		velocity_y = 0.005;
		//velocity_x = 0.0;
	}
	else if (collidedBottom == false) {
		velocity_y = -0.005;
		//velocity_x = 0.0;
	}
	else if (collidedTop == true && collidedBottom == true) {
		//velocity_x = 0.0;
		velocity_y = 0.0;
		collidedBottom = false;
		collidedTop = false;
	}
}
