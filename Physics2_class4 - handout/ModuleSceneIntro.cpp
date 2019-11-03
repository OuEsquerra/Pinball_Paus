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

	flipper_sound = App->audio->LoadFx("pinball/audio/flipper_sound.wav");

	jet_sound = App->audio->LoadFx("pinball/audio/jets.wav");

	new_ball= App->audio->LoadFx("pinball/audio/new_ball.wav");

	throw_ball = App->audio->LoadFx("pinball/audio/throw_ball.wav");

	//TEXTURES--------------------------------------------------------------
	ball_tex = App->textures->Load("pinball/ball.png");

	s_to_start = App->textures->Load("pinball/s_to_start.png");


	balls_text = App->textures->Load("pinball/ball_text.png");

	green_light = App->textures->Load("pinball/green_light.png");

	ready_text = App->textures->Load("pinball/ready_text.png");


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

	triangles.add(App->physics->CreateChain(0, 0, left_triangles_points, 11, b2_staticBody , 1.25f  ));

	triangles.add(App->physics->CreateChain(0, 0, right_triangles_points, 11, b2_staticBody , 1.25f  ));

	left_L = App->physics->CreateChain(0, 0, left_L_point, 11, b2_staticBody);

	right_L = App->physics->CreateChain(0, 0, right_L_point, 11, b2_staticBody);

	cooler = App->physics->CreateChain(0, 0, cooler_point, 29, b2_staticBody, 0.0f);
	
	top_left_chain = App->physics->CreateChain(0, 0, top_left_chain_ponits, 33, b2_staticBody);

	top_center_left = App->physics->CreateChain(0, 0, top_center_left_points, 9, b2_staticBody);

	top_center_right = App->physics->CreateChain(0, 0, top_center_right_points, 9, b2_staticBody);

	top_right_chain = App->physics->CreateChain(0, 0, top_right_chain_points, 37, b2_staticBody);


	//MOVING PARTS-------------------------------------------------------------------
	left_flipper = App->physics->CreateFlipper(211, 560, Left_Flipper, 17,1.0f , 0.2f); 

	right_flipper = App->physics->CreateFlipper(441, 560, Right_Flipper, 17, 1.0f, 0.2f);

	piston = App->physics->CreateRectangle(630, 630, 30, 30);

	createFlipperJoints();
	createPistonJoint();

	//SENSORS------------------------------------------------------------------------------------
	//Ball Block
	ball_block_sensor = App->physics->CreateChainSensor(0, 0, ball_block_sensor_points, 9);


	ball_block = App->physics->CreateChain(0, 0, ball_block_points, 9, b2_staticBody);
	
	
	powersupply_sensor = App->physics->CreateChainSensor(0, 0, powersupply, 8);
	powersupply_sensor->retain_ball = true;
	
	b2Vec2 throw_dir;
	throw_dir.x = 0;
	throw_dir.y = 200;

	powersupply_sensor->throw_direction = throw_dir;
	
	


	//circles.getLast()->data->listener = this;
	//n_ball = 1;

	App->audio->PlayMusic("pinball/audio/music.ogg");

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

	if (eject_timer_running == true) {
		eject_timer++;

		retained_ball->body->SetLinearVelocity(b2Vec2(0, 0));
		
		if (eject_timer >= 60) {
			eject_timer = 0;
			eject_timer_running = false;


			retained_ball->body->ApplyForceToCenter( retainer->throw_direction, true);
		}
	}


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
		App->audio->PlayFx(flipper_sound, 0);
		left_flipper->body->ApplyTorque(-300, true);
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
		App->audio->PlayFx(flipper_sound, 0);
		right_flipper->body->ApplyTorque(300, true);
	}
	else if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		right_flipper->body->ApplyTorque(100, true);
	}
	else
	{
		right_flipper->body->ApplyTorque(-50, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		App->audio->PlayFx(throw_ball, 0);
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
	
	if (ballCount > 4)
	{
		current_state = GAME_END;
	}

	switch (current_state)
	{
		case GAME_TOSTART:

			score = 0;
			crateBall(); //Starting Ball
			current_state = GAME_WAITINGBALL;
			ballCount = 1;

			ball_block_sensor->body->SetActive(true);

			ball_block->body->SetActive(false);

			break;

		case GAME_WAITINGBALL:
			
			ball_block_sensor->body->SetActive(true);

			ball_block->body->SetActive(false);

			break;

		case GAME_RUNNING:

			ball_block_sensor->body->SetActive(false);

			ball_block->body->SetActive(true);

			App->renderer->Blit(green_light, 30, 27);

			break;

		case GAME_END:

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
			{
				current_state = GAME_TOSTART;
			}

			App->renderer->Blit(s_to_start,275,331,&s_to_start_rect);

			ballCount = 1;

			break;
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate() {

	p2List_item<PhysBody*>* c = circles.getFirst();


	while (c != nullptr) {
		int x, y;
		c->data->GetPosition(x, y);
		

		if (y > board_rect.h) { 

			//Destroy Ball & Mouse Joint (If it exists)
			if (App->physics->foundBody != nullptr)
			{
				App->physics->GetWorld()->DestroyJoint(App->physics->mouse_joint);
				App->physics->mouse_joint = nullptr;
				App->physics->foundBody = nullptr;
			};
			App->physics->GetWorld()->DestroyBody(c->data->body);
			
			p2List_item<PhysBody*>* tmp = c->next;

			circles.del(c);

			ballCount++; //Sum ball to count

			if (ballCount <= 3 )
			{
				crateBall();
			}

			current_state = GAME_WAITINGBALL;

			c = tmp;

			//Place another ball at launcher
			/*circles.add(App->physics->CreateCircle(620, 600, 12, b2_dynamicBody, 0.0f, 1.0f));
			circles.getLast()->data->listener = this;*/

		}
		else c = c->next;
		
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{

	//Ignore collisions that involve no balls. It shouldn't happen, but it's just in case.
	if (circles.find(bodyA) != -1 && circles.find(bodyB) != -1) return;



	if (bodyB == ball_block_sensor) 
	{ 
		current_state = GAME_RUNNING;
	}


	if (circles.find(bodyA) != -1 && triangles.find(bodyB) != -1)
	{
		App->audio->PlayFx(jet_sound, 0);
	}

	if ((bodyB == top_jet || bodyB == left_jet || bodyB == right_jet) )
	{
		score += 10;
		App->audio->PlayFx(jet_sound, 0);
	}

	if (bodyB->retain_ball == true) {
		bodyA->body->SetLinearVelocity(b2Vec2(0, 0));


		eject_timer_running = true;

		retained_ball = bodyA;
		retainer = bodyB;
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
	//Create a prismatic joint
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


	b2RevoluteJoint* left_flipper_joint;
	left_flipper_joint = (b2RevoluteJoint*)App->physics->GetWorld()->CreateJoint(&piston_jointDef);

	App->physics->GetWorld()->CreateJoint(&piston_jointDef);
}

void ModuleSceneIntro::crateBall()
{
	App->audio->PlayFx(new_ball, 0);

	circles.add(App->physics->CreateCircle(620, 600, 12, b2_dynamicBody, 0.0f, 1.0f));

	circles.getLast()->data->listener = this;
}