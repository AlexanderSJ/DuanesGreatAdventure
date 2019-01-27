#include "Frog.h"
#include "MainGame.h"
#include "MainUpdate.h"
#include "Paddle.h"


using namespace Webfoot;

/// Speed of the ball along a given axis in pixels per second.
#define BALL_AXIS_SPEED 400.0f
#define BALL_MIN_SPEED 300.0f
#define BALL_MAX_SPEED 900.0f

// Percentage of how far inward the goal is on the screen.
#define LEFT_GOAL 0.02f
#define RIGHT_GOAL 0.98f

// A buffer to give the player some extra space when going for the ball.
#define GOAL_BUFFER 40

// Debug mode
#define DEBUG_MODE false


MainGame MainGame::instance;

//==============================================================================

/// Main GUI
#define GUI_LAYER_NAME "MainGame"

//-----------------------------------------------------------------------------

MainGame::MainGame()
{
   ball = NULL;
   paddle = NULL;
   aiPaddle = NULL;
   p1ScoreSprite = NULL;
   p2ScoreSprite = NULL;
   music = NULL;
   background = NULL;
}

//-----------------------------------------------------------------------------

void MainGame::Init()
{
   Inherited::Init();
  
   gameState = STATE_PAUSED;

   powerUpState = PWR_UP_STATE_NONE;

   rightGoal = theScreen->SizeGet().x * RIGHT_GOAL;
   leftGoal = theScreen->SizeGet().x * LEFT_GOAL;

   // Create and initialize the ball.
   ball = frog_new Ball();
   ball->Init();

   // Create and initialize the player paddle.
   paddle = frog_new Paddle();
   paddle->Init(1, DEBUG_MODE);

   // Create and initialize the AI paddle.
   aiPaddle = frog_new AiPaddle();
   aiPaddle->Init(0, DEBUG_MODE);
  
   // Set the positions of the player and AI paddles.
   // They should be just in front of the goal, with enough space to give the player time to react if the ball has gone a little past the ball. (That's what the GOAL_BUFFER is for)
   rightPaddlePosition = Point2F::Create(rightGoal - paddle->GetImage()->SizeGet().x - GOAL_BUFFER, (theScreen->SizeGet().y / 2.0f) - (paddle->GetImage()->HeightGet() / 2));
   leftPaddlePosition = Point2F::Create(leftGoal + GOAL_BUFFER, (theScreen->SizeGet().y / 2.0f) - (aiPaddle->GetImage()->HeightGet() / 2));

   // Set the paddle positions
   paddle->SetPosition(rightPaddlePosition);
   aiPaddle->SetPosition(leftPaddlePosition);

   // Create the score sprites
   p1ScoreSprite = frog_new Sprite();
   p2ScoreSprite = frog_new Sprite();

   // Initialize the score sprites
   InitializeScores(rightPaddlePosition, leftPaddlePosition);

   // Initialize the duane storm powerup sprites.
   for (int i = 0; i < 10; i++){
	   duaneSprite[i] = frog_new Duane();
	   duaneSprite[i]->Init();
   }

   // Initialize THE Duane.
   theDuane = frog_new Duane();
   theDuane->Init();
   
   // Initialize the background.
   background = frog_new AnimatedBackground();
   background->Init("AnimatedBackgrounds/background");

   // Initialize the song playing.
   music = theSounds->Load("Duane's Song");
   music->Play(0,true,Sound::USAGE_DEFAULT,50);
   
}

//-----------------------------------------------------------------------------

void MainGame::Deinit()
{

	// Deinitialize the end game text ("WIN" or "LOSE" text)
	if (endGameText){
		theImages->Unload(endGameText);
		endGameText = NULL;
	}

	// Deinitialize the music
	if (music){
		theSounds->Unload(music);
		music = NULL;
	}
	
	// Deinitialize the animated background
	if (background){
		background->Deinit();
		frog_delete background;
		background = NULL;
	}

	// Deinitialize THE Duane
	if (theDuane){
		theDuane->Deinit();
		frog_delete theDuane;
		theDuane = NULL;
	}

	// Deinitialize all the Duanes
	for (int i = 0; i < 10; i++){
		if (duaneSprite[i]){
			duaneSprite[i]->Deinit();
			frog_delete duaneSprite[i];
			duaneSprite[i] = NULL;
		}
	}

	// Deinitializing the player score sprites
	if (p2ScoreSprite){
		p2ScoreSprite->Deinit();
		frog_delete p2ScoreSprite;
		p2ScoreSprite = NULL;
	}

	if (p1ScoreSprite){
		p1ScoreSprite->Deinit();
		frog_delete p1ScoreSprite;
		p1ScoreSprite = NULL;
	}

   // Deinitialize and delete the ball.
   if(ball)
   {
      ball->Deinit();
      frog_delete ball;
      ball = NULL;
   }
   
   // Deinit the player paddle.
   if (paddle){
	   paddle->Deinit();
	   frog_delete paddle;
	   paddle = NULL;
   }

   // Deinit the AI paddle.
   if (aiPaddle){
	   aiPaddle->Deinit();
	   frog_delete aiPaddle;
	   aiPaddle = NULL;
   }
 

   Inherited::Deinit();
}

//-----------------------------------------------------------------------------

const char* MainGame::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainGame::Update()
{
   Inherited::Update();
   unsigned int dt = theClock->LoopDurationGet();

   // Update the animated background
   background->Update(dt);

   // Update THE Duane
   theDuane->Update(dt);

   // Update the Duane Storm only if the power up is active
   if (powerUpState == PWR_UP_STATE_DUANE){
	   for (int i = 0; i < 10; i++){
		   duaneSprite[i]->Update(dt);
	   }
   }

   CheckEndGame();

   if (gameState == STATE_SCORED || gameState == STATE_PAUSED){
	   endGameText = theImages->Load("readytext");
	   if (theKeyboard->KeyJustPressed(KEY_W) || theKeyboard->KeyJustPressed(KEY_S) || theKeyboard->KeyJustPressed(KEY_UP) || theKeyboard->KeyJustPressed(KEY_DOWN)){
		   endGameText = NULL;
		   gameState = STATE_PLAYING;
	   }
   }

   // If we're currently playing the game...
   if (gameState == STATE_PLAYING){

	   ball->Update(dt);

	   // After we've updated the ball's position, let's check if it's hitting the paddles...
	   CheckCollision(paddle);
	   CheckCollision(aiPaddle);

	   aiPaddle->Update(dt, ball->GetPosition(), ball->GetVelocity());
	   // Check to see if the ball has passed the goals.
	   CheckGoal(dt);
   }

   // Free the player to move unless the game is not paused (they can move even when the state is scored of game over)
   if (gameState != STATE_PAUSED){
	   paddle->Update(dt);
   }

   // Check for post-game input. Leaving this outside of an if statement
   // In case of further key presses needing to be called.
   GetInput();

   // Return to the previous menu if the escape key is pressed.
   if(!theStates->StateChangeCheck() && theKeyboard->KeyJustPressed(KEY_ESCAPE))
   {
      theMainGame->StateChangeTransitionBegin(true);
      theStates->Pop();
   }

}

//-----------------------------------------------------------------------------

void MainGame::Draw()
{
	background->Draw();

	if (powerUpState == PWR_UP_STATE_DUANE){
		for (int i = 0; i < 10; i++){
			duaneSprite[i]->Draw();
		}
	}

	theDuane->Draw();

	p1ScoreSprite->Draw();
	p2ScoreSprite->Draw();


	paddle->Draw();
	aiPaddle->Draw(ball->GetVelocity(),ball->GetPosition());
	
	ball->Draw();
	
	if (endGameText){
		endGameText->Draw(Point2F::Create((theScreen->SizeGet().x / 2) - (endGameText->SizeGet().x / 2), (theScreen->SizeGet().y / 2) - 1.5*(endGameText->SizeGet().y)));
	}

	if (DEBUG_MODE){
		DebugDrawGoals();
	}
}

// This function will check to see if the ball has passed the goal points of both players
// If it has, it will update the game's state accordingly, and reset the game.
void MainGame::CheckGoal(unsigned int dt){

	if (playerScore1 < 10 && playerScore2 < 10){
		// Check to see if the ball has passed the left player's goal.
		// If it has, update the right player's score.
		if (ball->GetPosition().x <= leftGoal){
			playerScore1++;
			p1ScoreSprite->TimeSet(30 * (playerScore1+1));
			if (DEBUG_MODE){
				DebugPrintf("Player 2 scored! \nP1: %d | P2: %d || %d\n", playerScore1, playerScore2, p1ScoreSprite->TimeGet());
			}
			
			gameState = STATE_SCORED;
			ResetRound();
		}

		// Check to see if the ball has passed the right player's goal.
		// If it has, update the left player's score.
		if (ball->GetPosition().x >= rightGoal){
			playerScore2++;
			p2ScoreSprite->TimeSet(30 * (playerScore2+1));
			if (DEBUG_MODE){
				DebugPrintf("Player 1 scored! \nP1: %d | P2: %d || %d\n", playerScore1, playerScore2, p2ScoreSprite->TimeGet());
			}
			gameState = STATE_SCORED;
			ResetRound();
		}
	}

}

// This function resets the ball, and paddles, and sets the game state back to playing. This is called whenever someone scores.
void MainGame::ResetRound(){
	ball->Init();
	paddle->SetPosition(rightPaddlePosition);
	aiPaddle->SetPosition(leftPaddlePosition);
	//gameState = STATE_PLAYING;
}

// Resets the game after the max score has been reached.
void MainGame::ResetGame(){
	playerScore1 = 0;
	playerScore2 = 0;

	InitializeScores(rightPaddlePosition, leftPaddlePosition);

	powerUpState = PWR_UP_STATE_NONE;

	endGameText = NULL;

	ResetRound();
}

// This function will draw the goals of both players. It will only do so if DEBUG_MODE is true.
void MainGame::DebugDrawGoals(){
	if (DEBUG_MODE){
		theScreen->LineDraw(Point2F::Create(leftGoal, 0.0f), Point2F::Create(leftGoal, theScreen->HeightGet()), COLOR_RGBA8_RED);
		theScreen->LineDraw(Point2F::Create(rightGoal, 0.0f), Point2F::Create(rightGoal, theScreen->HeightGet()), COLOR_RGBA8_BLUE);
	}
}

// This function handles the collision detection between a passed-in paddle, and the ball.
// This function must be called per-paddle.
void MainGame::CheckCollision(Paddle *p){
	float halfBallSize = ball->GetImage()->SizeGet().x / 2;

	if (((ball->GetPosition().x - halfBallSize <= p->GetCollisionBox().WidthGet() && ball->GetPosition().x + halfBallSize >= p->GetCollisionBox().x) && (ball->GetPosition().y >= p->GetCollisionBox().y && ball->GetPosition().y <= p->GetCollisionBox().HeightGet())))
	{
		if ((p->GetPlayerNumber() == 0 && ball->GetVelocity().x < 0.0f) || (p->GetPlayerNumber() == 1 && ball->GetVelocity().x > 0.0f)){
			ball->SetVelocity(ball->GetVelocity().x * -1.0f, ball->GetVelocity().y);
			if (p->GetYVelocity() > 0.8f || p->GetYVelocity() < -0.8f){
				ball->SetVelocity(ball->GetVelocity() * 1.5f);
			}
			else {
				ball->SetVelocity(ball->GetVelocity() / 1.5f);
			}
		}
	}
}

// Gets the input. Currently the only purpose is to check if the game is over, and if it is, to reset the scores and the positions of the balls/paddles when the player hits R.
void MainGame::GetInput(){
	if (gameState == STATE_END){
		if (theKeyboard->KeyJustPressed(KEY_R)){
			ResetGame();
			gameState = STATE_PLAYING;
		}
	}
}

void MainGame::CheckEndGame(){
	if (playerScore1 >= 10 || playerScore2 >= 10){
		powerUpState = PWR_UP_STATE_DUANE;
		if (playerScore1 >= 10){
			endGameText = theImages->Load("wintext");
		}
		else {
			endGameText = theImages->Load("losetext");
		}
		gameState = STATE_END;
	}
}

// Initializes the scores. Passing in the paddle positions because we want to make certain the paddle positions exist, rather than assuming they've been defined.
// We use the paddle positions to set the score sprites relative to the paddle locations.
void MainGame::InitializeScores(Point2F rightPaddlePos, Point2F leftPaddlePos){
	// Initialize and position the player score sprites
	p1ScoreSprite->Init("Sprites/Sprites", "Numbers");
	p2ScoreSprite->Init("Sprites/Sprites", "Numbers");

	// Setting the sprites position relative to the paddles.
	p1ScoreSprite->PositionSet((int)rightPaddlePos.x - (2 * GOAL_BUFFER), (2 * GOAL_BUFFER));
	p2ScoreSprite->PositionSet((int)leftPaddlePos.x + (2 * GOAL_BUFFER) + 20, (2 * GOAL_BUFFER));

	// Have to do this for some reason otherwise the score wont count up immediately.
	p1ScoreSprite->TimeSet(0);
	p2ScoreSprite->TimeSet(0);
}
//==============================================================================

Ball::Ball()
{
	// Initialize pointers to NULL for safety.
	image = NULL;
}

//------------------------------------------------------------------------------

void Ball::Init()
{
	// Load the image of the ball.
	if (!image){
		image = theImages->Load("Ball");
	}

	// Start the ball in the middle of the screen.
	position = Point2F::Create(theScreen->SizeGet() / 2);

	// Randomize a positive or negative value to the ball, so it doesn't always start going in the same direction.
	float randomx = FrogMath::RandomF() - 0.5f;
	float randomy = FrogMath::RandomF() - 0.5f;
	randomx = randomx / std::abs(randomx);
	randomy = randomy / std::abs(randomy);

	// Randomize an acceleration value to apply to the velocity of the ball.
	Point2F acceleration = Point2F::Create((FrogMath::RandomF() - 0.5f) * BALL_AXIS_SPEED, (FrogMath::RandomF() - 0.5f) * BALL_AXIS_SPEED);

	// Apply the random direction to the acceleration.
	acceleration.x = acceleration.x * randomx;
	acceleration.y = acceleration.y * randomy;

	// Set the ball's initial velocity.
	velocity.Set(BALL_AXIS_SPEED * randomx, BALL_AXIS_SPEED * randomy);

	// Add the acceleration calculated to the ball's velocity.
	velocity += acceleration;
}

//------------------------------------------------------------------------------

void Ball::Deinit()
{
	// Unload the image of the ball.
	if (image)
	{
		theImages->Unload(image);
		image = NULL;
	}
}

//------------------------------------------------------------------------------

void Ball::Update(unsigned int dt)
{
	// Get a floating point number for the duration of the last frame in seconds.
	float dtSeconds = (float)dt / 1000.0f;

	// Make sure the velocity never falls below a certain amount. Otherwise the ball goes too slow.
	if (velocity.x < BALL_MIN_SPEED && velocity.x> 0.0f){
		velocity.x = BALL_MIN_SPEED;
	}
	else if (velocity.x > -BALL_MIN_SPEED && velocity.x < 0.0f){
		velocity.x = -BALL_MIN_SPEED;
	}
	if (velocity.y < BALL_MIN_SPEED && velocity.y > 0.0f){
		velocity.y = BALL_MIN_SPEED;
	}
	else if (velocity.y > -BALL_MIN_SPEED && velocity.y < 0.0f){
		velocity.y = -BALL_MIN_SPEED;
	}

	// Make sure the ball never goes faster than a certain amount
	if (velocity.x > BALL_MAX_SPEED && velocity.x > 0.0f){
		velocity.x = BALL_MAX_SPEED;
	}
	else if (velocity.x < -BALL_MAX_SPEED && velocity.x < 0.0f){
		velocity.x = -BALL_MAX_SPEED;
	}
	if (velocity.y > BALL_MAX_SPEED && velocity.y > 0.0f){
		velocity.y = BALL_MAX_SPEED;
	}
	else if (velocity.y < -BALL_MAX_SPEED && velocity.y < 0.0f){
		velocity.y = -BALL_MAX_SPEED;
	}

   // Update the position of the ball.
   position += velocity * dtSeconds;

   // The position of the ball corresponds to its center.  We want to keep the
   // whole ball on-screen, so figure out the area within which the center must
   // stay.
   Point2F ballSize = Point2F::Create(image->SizeGet());
   Point2F halfBallSize = ballSize / 2.0f;
   Box2F ballArea = {halfBallSize.x, halfBallSize.y,
      (float)theScreen->WidthGet() - ballSize.x,
      (float)theScreen->HeightGet() - ballSize.y};

   // If the ball has gone too far in any direction, make sure its velocity
   // will bring it back.

   // See if it's too far right.
   if ((position.x > ballArea.MaxXGet()) && (velocity.x > 0.0f)){
	   velocity.x *= -1.0f;
   }

   // See if it's too far left.
   if ((position.x < ballArea.x) && (velocity.x < 0.0f)){
	   velocity.x *= -1.0f;
   }

   // See if it's too far down.
   if ((position.y > ballArea.MaxYGet()) && (velocity.y > 0.0f)){
	   velocity.y *= -1.0f;
   }

   // See if it's too far up.
   if ((position.y < ballArea.y) && (velocity.y < 0.0f)){
	   velocity.y *= -1.0f;
   }
   
}

//------------------------------------------------------------------------------

void Ball::Draw()
{
   // The center of the ball is in the center of the image, so use an offset.
   image->Draw(position - (Point2F::Create(image->SizeGet()) / 2.0f));
   if (DEBUG_MODE){
	   theScreen->LineDraw(Point2F::Create(0.0f, 0.0f), position, COLOR_RGBA8_BLUE, 1.0f, 0.0f);
	   theScreen->LineDraw(Point2F::Create(0.0f, 0.0f), position + velocity, COLOR_RGBA8_GREEN, 1.0f, 0.0f);
	   theScreen->LineDraw(position, position + velocity, COLOR_RGBA8_RED, 1.0f, 0.0f);
   }
}

//------------------------------------------------------------------------------

// Returns a Point2F of the ball's position.
Point2F const Ball::GetPosition(){
	return position;
}

// Returns a Point2F of the ball's velocity.
Point2F const Ball::GetVelocity(){
	return velocity;
}

// Sets the ball's velocity to a given x and y float value.
void Ball::SetVelocity(float x, float y){
	velocity.x = x;
	velocity.y = y;
}

// Sets the ball's velocity to a given Point2F vector. 
void Ball::SetVelocity(Point2F v){
	velocity = v;
}

// Returns the ball's image.
Image* const Ball::GetImage(){
	return image;
}

// ========================================================

Duane::Duane(){
	sprite = NULL;
}

void Duane::Init(){
	sprite = frog_new Sprite();

	position = Point2F::Create(theScreen->SizeGet().x * FrogMath::RandomF(),theScreen->SizeGet().y);
	velocity = Point2F::Create(0.0f, -100.0f);

	scale = FrogMath::RandomF() * 2;
	
	velocity.y /= scale;

	sprite->Init("Sprites/Sprites", "Duane");
	sprite->VisibleSet(true);
	sprite->PositionSet(position);
	sprite->ScaleSet(Point2F::Create(scale,scale));
}

void Duane::Deinit(){
	sprite->Deinit();
	frog_delete sprite;
	sprite = NULL;
}

void Duane::Update(unsigned int dt){
	float dtSeconds = (float)dt / 1000.0f;

	sprite->Update(dt);

	if (position.y <= 0){
		float pos = std::abs(FrogMath::RandomF() * theScreen->SizeGet().x);
		scale = FrogMath::RandomF() * 2;
		position = Point2F::Create(pos, theScreen->SizeGet().y);
		sprite->ScaleSet(Point2F::Create(scale, scale));
		velocity.y = -100.0f / scale;
	}

	position += velocity * dtSeconds;

	sprite->PositionSet(position);
}

void Duane::Draw(){
	sprite->Draw();
}

// ========================================================
// This feature may go unused.
// Originally the "Duane Storm" was going to be summoned by the ball hitting a power up on the field. However, I figured it might not be fun, because it only impairs the player and not the AI.
DuanePowerUp::DuanePowerUp(){
	sprite = NULL;
}

void DuanePowerUp::Draw(){
	sprite->Draw();
}

void DuanePowerUp::Update(unsigned int dt, Ball b){
	
}

int DuanePowerUp::CheckCollision(Ball b){
	if ((b.GetPosition().x >= collisionBox.x && b.GetPosition().x <= collisionBox.MaxXGet()) && (b.GetPosition().y >= collisionBox.y && b.GetPosition().y <= collisionBox.MaxYGet())){
		return 1;
	}
	return 0;
}