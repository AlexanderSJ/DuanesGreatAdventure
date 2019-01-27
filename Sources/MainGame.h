#ifndef __MAINGAME_H__
#define __MAINGAME_H__

#include "Frog.h"
#include "Paddle.h"
#include "MenuState.h"

namespace Webfoot {

enum State {STATE_PAUSED=0, STATE_PLAYING, STATE_SCORED, STATE_END };
enum PowerUpState {PWR_UP_STATE_NONE=0, PWR_UP_STATE_DUANE};

class Ball;
class Duane;

//==============================================================================

class MainGame : public MenuState
{
public:
   typedef MenuState Inherited;

   MainGame();
   virtual ~MainGame() {};
   
   virtual void Init();
   virtual void Deinit();

   /// Call this on every frame to update the positions.
   virtual void Update();
   /// Call this on every frame to draw the images.
   virtual void Draw();

   void DebugDrawGoals();

   void InitializeScores(Point2F, Point2F);
   void CheckCollision(Paddle*);
   void CheckGoal(unsigned int);
   void CheckEndGame();
   void ResetRound();
   void ResetGame();
   void GetInput();

   static MainGame instance;
protected:
   /// Returns the name of the GUI layer
   virtual const char* GUILayerNameGet();

   /// The ball that bounces around the screen.
   Ball* ball;
   Paddle* paddle;
   AiPaddle* aiPaddle;
   Duane* duaneSprite[10];
   Duane* theDuane;
   AnimatedBackground* background;
   Image* endGameText;

   float leftGoal;
   float rightGoal;

   int playerScore1;
   int playerScore2;

   Sprite* p1ScoreSprite;
   Sprite* p2ScoreSprite;

   Point2F leftPaddlePosition;
   Point2F rightPaddlePosition;

   State gameState;
   
   Sound* music;

   PowerUpState powerUpState;
};

MainGame* const theMainGame = &MainGame::instance;

//==============================================================================

/// A bouncing ball
class Ball
{
public:
   Ball();
   
   /// Initialize the ball
   void Init();
   /// Clean up the ball
   void Deinit();

   /// Make any changes for the given frame.  'dt' is the amount of time that
   /// has passed since the last frame, in milliseconds.
   void Update(unsigned int dt);
   /// Draw the ball.
   void Draw();

   Point2F const GetPosition();
   Point2F const GetVelocity();

   void SetVelocity(float, float);
   void SetVelocity(Point2F);

   void IncrementSpeed(float);

   Image* const GetImage();

   int const LastPlayerHit();

protected:
   /// Appearance of the ball.
   Image* image;
   /// Current position of the ball.
   Point2F position;
   /// Current velocity of the ball.
   Point2F velocity;
   int playerHit;
};

//==============================================================================

class Duane {
public:
	Duane();
	void Init();
	void Deinit();
	void Update(unsigned int);
	void Draw();
protected:
	float scale;
	Sprite* sprite;
	Point2F position;
	Point2F velocity;
};

class DuanePowerUp{
public:
	DuanePowerUp();
	void Init();
	void Deinit();
	void Update(unsigned int, Ball);
	int CheckCollision(Ball);
	void Draw();
protected:
	Sprite* sprite;
	Point2F position;
	Box2F collisionBox;
};

} //namespace Webfoot {

#endif //#ifndef __MAINGAME_H__
