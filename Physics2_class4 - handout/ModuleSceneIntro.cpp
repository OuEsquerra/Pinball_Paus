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

	Clear_Boards.add(App->physics->CreateChain(0, 0, Clear_Board, 64));

	flippers.add(App->physics->CreateChain(211,560, Left_Flipper, 19)); //19 is the points of the flipper

	flippers.add(App->physics->CreateChain(441, 560, Right_Flipper, 19));

	circles.add(App->physics->CreateCircle(620, 600, 15));

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
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 15));
		circles.getLast()->data->listener = this;
	}

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	if (c != NULL) {
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(ball_tex, x, y, NULL, 1.0f);
	}

	c = flippers.getFirst();

	int x, y = 0;

	if (c != NULL)
	{
		c->data->GetPosition(x, y);
		App->renderer->Blit(Left_Flipper_tex, x, y, NULL, 1.0f, c->data->GetRotation());
	}
	c = c->next;
	if (c != NULL)
	{
		c->data->GetPosition(x, y);
		App->renderer->Blit(Right_Flipper_tex, x - 95, y, NULL, 1.0f, c->data->GetRotation());
	}

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
