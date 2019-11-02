#include "Globals.h"
#include <string>
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ball_tex = Right_Flipper_tex = Left_Flipper_tex = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->fonts->Load("pinball/Score_Numbers.png", "0123456789", 1);

	score = 0;
	prev_score = 0;
	best_score = 0;

	current_state = GAME_TOSTART;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	//AUDIO FX--------------------------------------------------------------
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	//TEXTURES--------------------------------------------------------------
	ball_tex = App->textures->Load("pinball/ball.png");

	board_tex = App->textures->Load("pinball/clear_board_dark.png");

	cover_board_tex = App->textures->Load("pinball/board_covers.png");
	
	Left_Flipper_tex = App->textures->Load("pinball/Left_Flipper.png");

	Right_Flipper_tex = App->textures->Load("pinball/Right_Flipper.png");

	
	//BOARD COLLIDERS (CHAINS)------------------------------------------------
	cooler_bump = App->physics->CreateCircle(333 , 240 , 4, b2_staticBody);

	top_jet = App->physics->CreateCircle(340, 85, 9, b2_staticBody, 2.0f );

	left_jet = App->physics->CreateCircle(374,110, 9, b2_staticBody, 1 );

	right_jet = App->physics->CreateCircle(306, 110, 9, b2_staticBody, 1 );

	Clear_Boards.add(App->physics->CreateChain(0, 0, Clear_Board, 75,b2_staticBody , 0.0f));

	triangles.add(App->physics->CreateChain(0, 0, left_triangles_points, 11, b2_staticBody , 1.5f  ));

	triangles.add(App->physics->CreateChain(0, 0, right_triangles_points, 11, b2_staticBody , 1.5f  ));

	left_L = App->physics->CreateChain(0, 0, left_L_point, 11, b2_staticBody);

	right_L = App->physics->CreateChain(0, 0, right_L_point, 11, b2_staticBody);

	cooler = App->physics->CreateChain(0, 0, cooler_point, 29, b2_staticBody, 0.0f);
	
	top_left_chain = App->physics->CreateChain(0, 0, top_left_chain_ponits, 33, b2_staticBody);

	top_center_left = App->physics->CreateChain(0, 0, top_center_left_points, 9, b2_staticBody);

	top_center_right = App->physics->CreateChain(0, 0, top_center_right_points, 9, b2_staticBody);

	top_right_chain = App->physics->CreateChain(0, 0, top_right_chain_points, 37, b2_staticBody);

	//FLIPPERS-------------------------------------------------------------------
	left_flipper = App->physics->CreateFlipper(211, 560, Left_Flipper, 17,1.0f , 0.1f); //17 is the points of the flipper

	right_flipper = App->physics->CreateFlipper(441, 560, Right_Flipper, 17, 1.0f, 0.1f);

	piston = App->physics->CreateRectangle(630, 630, 30, 30);

	createFlipperJoints();
	createPistonJoint();

	//Starting Ball
	circles.add(App->physics->CreateCircle(620, 600, 12, b2_dynamicBody, 0.0f , 1.0f));

	circles.getLast()->data->listener = this;
	n_ball = 1;

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

	App->fonts->BlitText(280, 305, 0, std::to_string(score).c_str());

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 12, b2_dynamicBody, 0.0f, 1.0f));
		circles.getLast()->data->listener = this;
	}

	//LEFT FLIPPER
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		left_flipper->body->ApplyTorque(-200, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		left_flipper->body->ApplyTorque(-100, true); 
	}
	else
	{
		left_flipper->body->ApplyTorque(50, true);
	}
	
	//RIGHT FLIPPER
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		right_flipper->body->ApplyTorque(200, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		right_flipper->body->ApplyTorque(100, true);
	}
	else
	{
		right_flipper->body->ApplyTorque(-50, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		b2Vec2 force;
		force.x = 0;
		force.y = -140;

		piston->body->ApplyForce(force, piston->body->GetWorldCenter(), true);
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

			//DESTROY FALLEN BALLS
			App->physics->GetWorld()->DestroyBody(c->data->body);
			
			p2List_item<PhysBody*>* tmp = c->next;
			circles.del(c);
			
			c = tmp;

			//Place another ball at launcher
			circles.add(App->physics->CreateCircle(620, 600, 12, b2_dynamicBody, 0.0f, 1.0f));
			circles.getLast()->data->listener = this;

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
	
	score += 10;
	LOG("Ball has hit something!");
	
	if (board->body->GetFixtureList()->IsSensor()) {
		/*
		if(board->body)

		if (board->audio_fx != NULL) {
			App->audio->PlayFx(board->audio_fx);
		}

		score += board->score_value;
		*/

	}
	

}

void ModuleSceneIntro::createFlipperJoints()
{
	//Body to RevoluteJoint the FLipper (Left First)
	left_flipper_joint = App->physics->CreateCircle(215, 582, 2, b2_staticBody);

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
}

void ModuleSceneIntro::createPistonJoint()
{
	//Body to RevoluteJoint the FLipper (Left First)
	piston_joint = App->physics->CreateCircle(655, 655, 2, b2_staticBody);

	b2PrismaticJointDef piston_jointDef;
	piston_jointDef.bodyA = piston_joint->body;
	piston_jointDef.bodyB = piston->body;
	piston_jointDef.collideConnected = false;

	b2Vec2 vertical;
	vertical.x = 13;
	vertical.y = 44;
	piston_jointDef.localAxisA = vertical;

	piston_jointDef.enableLimit = true;
	piston_jointDef.lowerTranslation = PIXEL_TO_METERS(-50);
	piston_jointDef.upperTranslation = PIXEL_TO_METERS(50);
	
	piston_jointDef.localAnchorA.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	piston_jointDef.localAnchorB.Set(PIXEL_TO_METERS(0), PIXEL_TO_METERS(0));
	piston_jointDef.referenceAngle = -5 * DEGTORAD;

	App->physics->GetWorld()->CreateJoint(&piston_jointDef);
}