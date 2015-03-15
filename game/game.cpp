  /*=================================================================}
 /  <<<<<<<<<<<<<<<<<<<<<--- Harsh Engine --->>>>>>>>>>>>>>>>>>>>>>  }
/  e-mail  : xproger@list.ru                                         }
{  github  : https://github.com/xproger/harsh                        }
{====================================================================}
{ LICENSE:                                                           }
{ Copyright (c) 2013, Timur "XProger" Gagiev                         }
{ All rights reserved.                                               }
{                                                                    }
{ Redistribution and use in source and binary forms, with or without /
{ modification, are permitted under the terms of the BSD License.   /
{=================================================================*/
#include "game.h"
#include "render.h"
#include "core.h"
#include "utils.h"
#include "resources.h"

Player::Player() {
	parent = new Sprite();

	body = new Sprite();
	body->setParent(parent);
	body->position.z = 1;
	//body->setVerticesColor(vec4(0.3f, 0.3f, 0.3f, 1.0f));	

	head = new Sprite();
	head->setPivot(vec2(0.5f, 0.7f));
	head->setParent(body);	
	head->position.z = 2;
	//head->setVerticesColor(vec4(0.6f, 0.3f, 0.3f, 1.0f));

	shoulders = new Sprite();	
	shoulders->setParent(body);
	shoulders->position.z = 2;
	//shoulders->setVerticesColor(vec4(0.3f, 0.3f, 0.6f, 1.0f));
}

void Player::update() {
	vec2 dir = (Core::input->mouse.pos - parent->position.xy());
	//dir.normalize();
	head->setRotation(dir.getRotationAngle() + 90);
}

Game::Game() 
{	
	
	atl = TextureAtlas::init(new Stream("data/stealth.tga"), TexExt::extTga, new Stream("data/stealth.atlas"), TextureAtlasExt::extCheetah, true);
	
	mat = Material::init(Default::spriteShader);
	mat->addTexture(atl, "uDiffuse", 0);	

	camera = new Camera();	
	camera->setScale(0.5f);
	
	font = Font::init(new Stream("data/Times New Roman16.bmp"));
	fbatch = new FontBatch(font);
	
	text1 = new Text();
	text1->text = "ÏðÎâÅðÊà. Ñîððè çà çàáîð";
	text1->position = vec3(300, 100, 0);

	player = new Player();
	player->parent->position = vec3(100, 200, 5);
	player->body->setTextureRegion(atl->getRegion("player_body.png"));
	player->head->setTextureRegion(atl->getRegion("player_head.png"));
	player->shoulders->setTextureRegion(atl->getRegion("player_shoulders.png"));
}

Game::~Game() 
{	
	delete player;
	
	delete mat;	
	delete atl;
	delete text1;
	delete font;
	delete fbatch;
	delete camera;	
}

void Game::pause() 
{
	//
}

void Game::resume()
{

}

void Game::update() 
{
	player->update();
}

void Game::render() {
	Render::clear(CLEAR_ALL, 0.0f, 0.5f, 0.8f, 1.0f);		
	camera->update();
	
	mat->bind();	
	batch.begin();	
		batch.render(player->body);		
		batch.render(player->head);
		batch.render(player->shoulders);
	batch.end();
	
	fbatch->begin();
		fbatch->render(text1);
	fbatch->end();
	
}

void Game::inputEvent(const InputEvent &e)
{
	
}
