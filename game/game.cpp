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
#include "core.h"
#include "render.h"
#include "utils.h"
#include "resources.h"

Game::Game() 
{
	Default::init();

	for (int i = 0; i < SPRITE_COUNT; i++) {
		sprites[i] = new Sprite(20.0f, 20.0f, vec2(0.5f, 0.5f));
		sprites[i]->position = vec3(15 + 20.0f * i, 15 + 20.0f * i, i);
	}	

	mat = Material::init(Default::spriteShader);
	mat->addTexture(Default::blankTexture, "uDiffuse", 0);	

	camera = new Camera();	
	//camera->setProjParams(0, 0, 20, 20, 45, -1, 100, pmOrtho, pTopLeft);
}

Game::~Game() 
{
	for (int i = 0; i < SPRITE_COUNT; i++)
		delete sprites[i];
	delete mat;
	delete camera;
	Default::deinit();
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

}

void Game::render() {
	Render::clear(CLEAR_ALL, 0.0f, 0.5f, 0.8f, 1.0f);	
	mat->bind();
	camera->update();
	batch.begin();
	for (int i = 0; i < SPRITE_COUNT; i++)
		batch.render(sprites[i]);
	batch.end();	
}

void Game::inputEvent(const InputEvent &e)
{
	
}
