#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "Frog.h"

namespace Webfoot {
	class AiPaddle;

	class Paddle {
	public:
		Paddle();
		void Init(int, bool);
		void Deinit();
		virtual void Update(unsigned int);
		void Draw();
		void DebugDraw();
		virtual void MovePaddle(unsigned int);
		void UpdateCollisionBox();
		bool CanMove(float, float);

		void SetPlayerNumber(int);
		void SetPosition(float, float);
		void SetPosition(Point2F);
		void SetImage(Image*);

		Box2F const GetCollisionBox();
		Point2F const GetPosition();
		float const GetYVelocity();
		int const GetPlayerNumber();
		Image* const GetImage();

		bool debug;
	protected:
		Box2F collisionBox;
		Point2F position;
		Image* image;

		int playerNumber; // 0 = right, 1 = left

		float yVelocity;
	};

	class AiPaddle : public Paddle {
	public:
		typedef Paddle Inherited;

		AiPaddle();
		virtual void MovePaddle(unsigned int, Point2F);
		virtual void Update(unsigned int, Point2F, Point2F);
		void Draw(Point2F, Point2F);
		void Test(Point2F velocity, Point2F position);
	};
} // Namespace
#endif