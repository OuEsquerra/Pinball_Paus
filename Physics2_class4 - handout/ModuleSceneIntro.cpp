#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball_tex = Right_Flipper_tex = Left_Flipper_tex = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	score = 0;
	prev_score = 0;
	best_score = 0;
	App->renderer->camera.x = App->renderer->camera.y = 0;

	//AUDIO FX--------------------------------------------------------------
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//TEXTURES--------------------------------------------------------------
	ball_tex = App->textures->Load("pinball/ball.png");

	board_tex = App->textures->Load("pinball/clear_board.png");

	cover_board_tex = App->textures->Load("pinball/board_covers.png");
	
	Left_Flipper_tex = App->textures->Load("pinball/Left_Flipper.png");

	Right_Flipper_tex = App->textures->Load("pinball/Right_Flipper.png");

	
	//BOARD COLLIDERS (CHAINS)------------------------------------------------
	cooler_bump = App->physics->CreateCircle(333 , 240 , 4, b2_staticBody);

	Clear_Boards.add(App->physics->CreateChain(0, 0, Clear_Board, 75,b2_staticBody));

	triangles.add(App->physics->CreateChain(0, 0, left_triangles_points, 11, b2_staticBody));

	triangles.add(App->physics->CreateChain(0, 0, right_triangles_points, 11, b2_staticBody));

	left_L = App->physics->CreateChain(0, 0, left_L_point, 11, b2_staticBody);

	right_L = App->physics->CreateChain(0, 0, right_L_point, 11, b2_staticBody);

	cooler = App->physics->CreateChain(0, 0, cooler_point, 29, b2_staticBody);
	
	//FLIPPERS-------------------------------------------------------------------
	left_flipper = App->physics->CreateFlipper(211, 560, Left_Flipper, 17); //17 is the points of the flipper

	right_flipper = App->physics->CreateFlipper(441, 560, Right_Flipper, 17);

	//Body to RevoluteJoint the FLipper (Left First)
	left_flipper_joint = App->physics->CreateCircle( 215 , 582 , 2 , b2_staticBody);

	b2RevoluteJointDef revoluteJointDef_left;
	revoluteJointDef_left.bodyA = left_flipper_joint->body;
	revoluteJointDef_left.bodyB = left_flipper->body;
	revoluteJointDef_left.collideConnected = false;

	revoluteJointDef_left.enableLimit = true;
	revoluteJointDef_left.lowerAngle = -25 * DEGTORAD;
	revoluteJointDef_left.upperAngle = 25 * DEGTORAD;

	revoluteJointDef_left.localAnchorA.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	revoluteJointDef_left.localAnchorB.Set(PIXEL_TO_METERS(10), PIXEL_TO_METERS(15));

	b2RevoluteJoint* left_flipper_joint;
	left_flipper_joint = (b2RevoluteJoint*)App->physics->GetWorld()->CreateJoint(&revoluteJointDef_left);

	//Body to RevoluteJoint the FLipper (Right First)
	right_flipper_joint = App->physics->CreateCircle(440, 582, 2, b2_staticBody);
	
	b2RevoluteJointDef revoluteJointDef_right;
	revoluteJointDef_right.bodyA = right_flipper_joint->body;
	revoluteJointDef_right.bodyB = right_flipper->body;
	revoluteJointDef_right.collideConnected = false;

	revoluteJointDef_right.enableLimit = true;
	revoluteJointDef_right.lowerAngle = -25 * DEGTORAD;
	revoluteJointDef_right.upperAngle = 25 * DEGTORAD;

	revoluteJointDef_right.localAnchorA.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	revoluteJointDef_right.localAnchorB.Set(PIXEL_TO_METERS(-10), PIXEL_TO_METERS(15));

	b2RevoluteJoint* right_flipper_joint;
	right_flipper_joint = (b2RevoluteJoint*)App->physics->GetWorld()->CreateJoint(&revoluteJointDef_right);

	//Starting Ball
	circles.add(App->physics->CreateCircle(620, 600, 12, b2_dynamicBody));

	circles.getLast()->data->listener = this;

	return ret;
}


// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(board_tex, 0, 0, &board_rect);

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 12, b2_dynamicBody));
		circles.getLast()->data->listener = this;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
	{
		left_flipper->body->ApplyTorque(-200, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		left_flipper->body->ApplyTorque(-100, true); 
	}
	else
	{
		left_flipper->body->ApplyTorque(50, true);
	}
	

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		right_flipper->body->ApplyTorque(200, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
	{
		right_flipper->body->ApplyTorque(100, true);
	}
	else
	{
		right_flipper->body->ApplyTorque(-50, true);
	}

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while (c != NULL) {
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(ball_tex, x, y, NULL, 1.0f);

		c = c->next;
	}

	int x, y = 0;
	
	left_flipper->GetPosition(x, y);
	App->renderer->Blit(Left_Flipper_tex, x, y, NULL, 1.0f, left_flipper->GetRotation() , 0 ,0);
	
	right_flipper->GetPosition(x, y);
	App->renderer->Blit(Right_Flipper_tex, x - 95, y, NULL, 1.0f, right_flipper->GetRotation(),95,0);

	App->renderer->Blit(cover_board_tex, 0, 0, &board_rect);
	
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate() {

	p2List_item<PhysBody*>* c = circles.getFirst();


	while (c != nullptr) {
		int x, y;
		c->data->GetPosition(x, y);
		
		if (y > board_rect.h) {
			App->physics->GetWorld()->DestroyBody(c->data->body);
			
			p2List_item<PhysBody*>* tmp = c->next;
			circles.del(c);
			
			c = tmp;
		}
		else c = c->next;
		
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	if (circles.find(bodyA) != -1 && circles.find(bodyB) != -1) {//Ignore collisions between 2 balls
		return;
	}

	

	PhysBody* ball;
	PhysBody* board;
	
	if (circles.find(bodyA) != -1) { //For clarity
		ball = bodyA;
		board = bodyB;
	}
	else if (circles.find(bodyB) != -1) {
		ball = bodyB;
		board = bodyA;
	}
	else {
		return;
	}
	
	LOG("Ball has hit something!");
	
	/*
	if(board->body)

	if (board->audio_fx != NULL) {
		App->audio->PlayFx(board->audio_fx);
	}

	score += board->score_value;
	*/
}