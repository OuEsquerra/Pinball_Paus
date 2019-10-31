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

	App->renderer->camera.x = App->renderer->camera.y = 0;

	ball_tex = App->textures->Load("pinball/ball.png"); 
	
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	board_tex = App->textures->Load("pinball/clear_board.png");
	
	Left_Flipper_tex = App->textures->Load("pinball/Left_Flipper.png");

	Right_Flipper_tex = App->textures->Load("pinball/Right_Flipper.png");

	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);

	cooler_bump = App->physics->CreateCircle(333 , 240 , 4, b2_staticBody);

	Clear_Boards.add(App->physics->CreateChain(0, 0, Clear_Board, 75,b2_staticBody));

	triangles.add(App->physics->CreateChain(0, 0, left_triangles_points, 11, b2_staticBody));

	triangles.add(App->physics->CreateChain(0, 0, right_triangles_points, 11, b2_staticBody));

	left_flipper=App->physics->CreateFlipper(211,560,Left_Flipper, 17 ); //19 is the points of the flipper

	right_flipper=App->physics->CreateFlipper(441, 560, Right_Flipper, 17);

	left_L = App->physics->CreateChain(0, 0, left_L_point, 11, b2_staticBody);

	right_L = App->physics->CreateChain(0, 0, right_L_point, 11, b2_staticBody);

	cooler = App->physics->CreateChain(0, 0, cooler_point, 29, b2_staticBody);

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
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	//int x, y;

	//App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
