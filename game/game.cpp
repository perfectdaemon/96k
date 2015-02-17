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

Game::Game() 
{

}

Game::~Game() 
{
	//
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
}

void Game::inputEvent(const InputEvent &e)
{
	
}
