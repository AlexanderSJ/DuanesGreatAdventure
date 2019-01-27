#include "Paddle.h"

using namespace Webfoot;

#define PADDLE_SPEED 800.0f
// The speed limit buffer reduces the max paddle speed for the AI.
// 300 - Easy
// 200 - Medium
// 100 - Hard
// 0 - Literally impossible
#define AI_PADDLE_SPEED_LIMIT_BUFFER 200.0f
#define PADDLE_MIN_SPEED 300.0f

Paddle::Paddle(){
	image = NULL;
}

void Paddle::Init(int pNum, bool dbg){
	SetPlayerNumber(pNum);
	debug = dbg;

	if (playerNumber == 1){
		if (!image){
			image = theImages->Load("paddle2");
		}
	}
	else {
		if (!image){
			image = theImages->Load("paddle1");
		}
	}
}

void Paddle::Deinit(){
	if (image){
		theImages->Unload(image);
		image = NULL;
	}
}

void Paddle::Update(unsigned int dt){
	MovePaddle(dt);
	UpdateCollisionBox();
}

void Paddle::MovePaddle(unsigned int dt){
	float dtSeconds = (float)dt / 1000.0f;
	float movement;
	yVelocity = 0;

	if (theKeyboard->KeyPressed(KEY_S) || theKeyboard->KeyPressed(KEY_DOWN)){
		yVelocity = 1;
	}

	if (theKeyboard->KeyPressed(KEY_W) || theKeyboard->KeyPressed(KEY_UP)){
		yVelocity = -1;
	}

	movement = ((PADDLE_SPEED + 200.0f) * yVelocity) * dtSeconds;

	if (CanMove(movement, yVelocity)){
		position.y += movement;
	}

}

void Paddle::Draw(){
	image->Draw(position);
	if (debug){
		DebugDraw();
	}
}

void Paddle::DebugDraw(){
	if (debug){
		theScreen->LineDraw(Point2F::Create(collisionBox.x,collisionBox.y), Point2F::Create(collisionBox.WidthGet(),collisionBox.y), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(collisionBox.WidthGet(), collisionBox.y), Point2F::Create(collisionBox.WidthGet(), collisionBox.HeightGet()), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(collisionBox.WidthGet(), collisionBox.HeightGet()), Point2F::Create(collisionBox.x, collisionBox.HeightGet()), COLOR_RGBA8_GREEN);
		theScreen->LineDraw(Point2F::Create(collisionBox.x, collisionBox.HeightGet()), Point2F::Create(collisionBox.x, collisionBox.y), COLOR_RGBA8_GREEN);
	}
}

void Paddle::UpdateCollisionBox(){
	collisionBox = Box2F::Create(position.x, position.y, image->WidthGet() + position.x, image->HeightGet() + position.y);
}

bool Paddle::CanMove(float movement, float yVelocity){
	if ((position.y + movement > theScreen->HeightGet() - image->HeightGet() && yVelocity > 0.0f) || (position.y + movement < 0 && yVelocity < 0.0f)){
		return false;
	}
	return true;
}

void Paddle::SetPlayerNumber(int n){
	if (n < 0){
		playerNumber = 0;
	}
	else if (n > 1){
		playerNumber = 1;
	}
	else {
		playerNumber = n;
	}
}

void Paddle::SetImage(Image* i){
	image = i;
}

void Paddle::SetPosition(float x, float y){
	position.x = x;
	position.y = y;
}

void Paddle::SetPosition(Point2F pos){
	position = pos;
}

Box2F const Paddle::GetCollisionBox() {
	return collisionBox;
}

Image* const Paddle::GetImage(){
	return image;
}

int const Paddle::GetPlayerNumber(){
	return playerNumber;
}

float const Paddle::GetYVelocity(){
	return yVelocity;
}

// **************************************************

AiPaddle::AiPaddle(){
	image = NULL;
	yVelocity = 1;
}

void AiPaddle::Update(unsigned int dt, Point2F bPosition, Point2F bVelocity){
	Point2F directionVector = (bPosition + bVelocity);
	MovePaddle(dt, directionVector);
	UpdateCollisionBox();
	
}

// This function handles the movement of the AI paddle. It takes in a float, which is the ball's y position
void AiPaddle::MovePaddle(unsigned int dt, Point2F y){
	float dtSeconds = (float)dt / 1000.0f;
	float halfHeight = (image->SizeGet().y / 2);

	// Set the yVelocity variable. Make sure it's not too fast, nor too slow.
	yVelocity = ((position.y + halfHeight) - y.y); //  + ((2*halfHeight) * (FrogMath::RandomF() - 0.5f)
	if (yVelocity > PADDLE_SPEED - AI_PADDLE_SPEED_LIMIT_BUFFER){
		yVelocity = PADDLE_SPEED - AI_PADDLE_SPEED_LIMIT_BUFFER;
	}
	else if (yVelocity < -PADDLE_SPEED + AI_PADDLE_SPEED_LIMIT_BUFFER){
		yVelocity = -PADDLE_SPEED + AI_PADDLE_SPEED_LIMIT_BUFFER;
	}
	else if (yVelocity < PADDLE_MIN_SPEED && yVelocity > 0.0f){
		yVelocity = PADDLE_MIN_SPEED;
	}
	else if (yVelocity > -PADDLE_MIN_SPEED && yVelocity < 0.0f){
		yVelocity = -PADDLE_MIN_SPEED;
	}

	// Adjust the position. Subtracting due to how the yVelocity is calculated.
	position.y -= ((yVelocity)* dtSeconds);

	// CanMove is not functioning properly with the AI paddle's movement. So do out-of-bounds checks here.
	if (position.y < 0.0f){
		position.y = 0.0f;
	}
	else if (position.y + image->HeightGet() > theScreen->SizeGet().y){
		position.y = theScreen->SizeGet().y - image->HeightGet();
	}
}

// This is just a debug statement to view the vectors between the ball and the paddle.
void AiPaddle::Test(Point2F velocity, Point2F bPosition){
	theScreen->LineDraw(Point2F::Create(position.x, position.y + image->SizeGet().y /2), bPosition, COLOR_RGBA8_CYAN);
	theScreen->LineDraw(bPosition, bPosition + velocity, COLOR_RGBA8_ORANGE);
	// I want to move closer and closer to the y position of bPosition + velocity.
	// The magnitude of bPosition + velocity is how fast I want to get there.
	// When I'm at bPosition + velocity, I want to stop.
	theScreen->LineDraw(bPosition + velocity, position, COLOR_RGBA8_MAGENTA, 2.0f,0);
}

// This is a debug statement to view the vectors between the ball and the paddle, and to draw the paddle.
void AiPaddle::Draw(Point2F v, Point2F bp){
	Inherited::Draw();
	if (debug){
		Test(v, bp);
	}
}