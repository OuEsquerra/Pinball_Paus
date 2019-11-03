#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

enum GameState {
	GAME_TOSTART,
	GAME_RUNNING,
	GAME_WAITINGBALL,
	GAME_END
};


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void createFlipperJoints();
	void createPistonJoint();
	void crateBall();

public:

	//Bodies
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> Clear_Boards;


	PhysBody* left_flipper;
	PhysBody* right_flipper;
	PhysBody* left_flipper_joint;
	PhysBody* right_flipper_joint;
	PhysBody* piston;
	PhysBody* piston_joint;
	PhysBody* left_L;
	PhysBody* right_L;
	PhysBody* cooler;
	PhysBody* cooler_bump;
	PhysBody* top_left_chain;
	PhysBody* top_center_left;
	PhysBody* top_center_right;
	PhysBody* top_right_chain;
	PhysBody* cooler_sensor;
	

	p2List<PhysBody*> triangles;
	p2List<PhysBody*> red_targets;
	p2List<PhysBody*> red_flags;

	PhysBody* top_jet;
	PhysBody* left_jet;
	PhysBody* right_jet;
	
	PhysBody* ball_block_sensor;
	PhysBody* ball_block;

	PhysBody* powersupply_sensor;
	PhysBody* blueres_sensor;


	//Textures
	SDL_Texture* ball_tex;
	SDL_Texture* board_tex;
	SDL_Texture* cover_board_tex;
	SDL_Texture* Left_Flipper_tex;
	SDL_Texture* Right_Flipper_tex;
	SDL_Texture* s_to_start;
	SDL_Texture* green_light;
	SDL_Texture* balls_text;
	SDL_Texture* ready_text;
	SDL_Texture* jets_text;
	SDL_Texture* high_score_tex;
	SDL_Texture* prev_score_tex;

	//Audio
	uint bonus_fx;
	uint flipper_sound;
	uint jet_sound;
	uint new_ball;
	uint throw_ball;
	uint ball_saved;
	uint red_target_sound;
	uint generic_points;

	p2Point<int> ray;
	bool ray_on;

	SDL_Rect board_rect = { 0,0,700,650 };
	SDL_Rect s_to_start_rect = { 0,0,110,42 };
	SDL_Rect high_score_rect = { 0,0,136,21 };
	SDL_Rect prev_score_rect = { 0,0,136,21 };

	//In Game handling
	uint score;
	uint prev_score;
	uint high_score;
	
	bool save_ball = true;
	int save_ball_timer = 0;

	int ballCount = 1;

	PhysBody* sensor;
	bool sensed;

	GameState current_state;

	uint jet_count = 0;
	uint last_jet_hit;
	bool jet_combo = false;

	uint eject_timer = 0;
	bool eject_timer_running = false;
	
	PhysBody* retained_ball;
	PhysBody* retainer;


private:

	int Clear_Board[76] = {
	630, 650,
	526, 246,
	483, 333,
	513, 350,
	548, 350,
	597, 543,
	408, 648,
	406, 710,
	234, 708,
	230, 648,
	50, 542,
	106, 352,
	146, 348,
	173, 333,
	146, 244,
	136, 240,
	158, 164,
	184, 236,
	160, 153,
	193, 47,
	198, 36,
	205, 29,
	213, 25,
	221, 20,
	230, 15,
	243, 11,
	263, 8,
	422, 8,
	436, 8,
	449, 9,
	461, 13,
	475, 20,
	491, 30,
	506, 45,
	680, 650,
	682, 669,
	639, 673,
	630, 650
	};

	int ball_block_points[10] = {
	524, 246,
	547, 189,
	554, 215,
	533, 246,
	524, 246
	};

	int ball_block_sensor_points[10] = {
	500, 203,
	520, 175,
	540, 174,
	510, 230,
	500, 203
	};

	int top_left_chain_ponits[34] = {
	214, 104,
	223, 61,
	235, 51,
	245, 45,
	255, 41,
	266, 40,
	282, 38,
	294, 38,
	302, 38,
	302, 43,
	268, 52,
	257, 126,
	251, 126,
	249, 103,
	222, 101,
	211, 137,
	214, 104
	};

	int top_right_chain_points[38] = {
	400, 41,
	400, 50,
	416, 50,
	416, 100,
	460, 161,
	450, 73,
	480, 73,
	498, 167,
	498, 176,
	495, 185,
	472, 238,
	501, 178,
	501, 155,
	481, 69,
	471, 59,
	457, 50,
	441, 44,
	425, 41,
	400, 41
	};

	int top_center_left_points[10] = {
	329, 37,
	337, 37,
	337, 48,
	329, 48,
	329, 37
	};

	int top_center_right_points[10] = {
	364, 37,
	370, 37,
	370, 48,
	364, 48,
	364, 37
	};

	int cooler_point[30] = {
	289, 173,
	334, 168,
	383, 174,
	385, 244,
	381, 241,
	372, 187,
	360, 177,
	345, 175,
	334, 174,
	319, 175,
	307, 178,
	296, 187,
	287, 239,
	283, 243,
	289, 173
	};

	int left_L_point[12] = {
	142, 388,
	116, 510,
	208, 561,
	205, 563,
	113, 511,
	142, 388
	};

	int right_L_point[12] = {
	445, 562,
	447, 564,
	542, 510,
	517, 387,
	536, 507,
	445, 562
	};

	int left_triangles_points[12] = {
	432, 498,
	470, 390,
	481, 394,
	493, 477,
	443, 503,
	432, 498
	};

	int right_triangles_points[12] = {
	188, 390,
	175, 397,
	165, 482,
	211, 504,
	220, 495,
	188, 390
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

	int blue_resitence[6] = {
		146, 220,
		167, 206,
		155, 183
	};

	int powersupply[8] = {
		449, 73,
		451, 88,
		473, 88,
		470, 74
	};


};
