#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> Clear_Boards;
	p2List<PhysBody*> flippers;

	PhysBody* sensor;
	bool sensed;

	SDL_Texture* ball_tex;
	SDL_Texture* board_tex;
	SDL_Texture* Left_Flipper_tex;
	SDL_Texture* Right_Flipper_tex;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;
	SDL_Rect board_rect = { 0,0,700,650 };
	
private:

	int Clear_Board[66] =
	{
		678, 647,
		507, 48,
		501, 39,
		493, 31,
		482, 24,
		470, 18,
		453, 13,
		438, 10,
		423, 8,
		299, 8,
		282, 7,
		268, 8,
		248, 11,
		232, 16,
		221, 21,
		209, 28,
		199, 37,
		192, 47,
		51, 542,
		237, 651,
		237, 730,
		412, 730,
		412, 650,
		599, 542,
		548, 351,
		513, 351,
		482, 335,
		520, 244,
		527, 244,
		632, 650,
		639, 680,
		688, 680,
		678, 647
	};
	
	int Left_Flipper[20] = {
		7, 0,
		89, 0,
		94, 3,
		94, 10,
		91, 15,
		10, 25,
		1, 20,
		0, 12,
		2, 4,
		7, 0
	};
	int Right_Flipper[20] = {
		-7, 0,
		-89, 0,
		-94, 3,
		-94, 10,
		-91, 15,
		-10, 25,
		-1, 20,
		0, 12,
		-2, 4,
		-7, 0
	};

};
