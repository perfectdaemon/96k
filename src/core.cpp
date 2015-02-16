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
#include "core.h"
#include "render.h"

//{ Core ----------------------------------------------
float Core::deltaTime;
int Core::lastTime, Core::fpsTime, Core::fps;
getTimePtr Core::getTime;
Input	*Core::input;
Game	*Core::game;

void Core::init(const char *path, getTimePtr getTime) {
	LOG("Core::init\n");

	input = new Input();
	Core::getTime = getTime;
    Stream::init(path);
    Render::init();

	game = new Game();
    fpsTime = getTime();
    lastTime = fpsTime;
    fps = 0;
}

void Core::deinit() {
	LOG("Core::deinit\n");
    delete game;	
	LOG("Core::deinit\n");
    delete input;	
	LOG("Core::deinit\n");
    Render::deinit();
	LOG("Core::deinit\n");
}

void Core::pause() {
	LOG("Core::pause\n");
	game->pause();
}

void Core::resume() {
	LOG("Core::resume\n");
	lastTime = getTime();	
	game->resume();
}

void Core::reset() {
	LOG("Core::reset\n");    
    input->reset();
}

void Core::resize(int width, int height) {
    Render::resize(width, height);
}

void Core::update() {   
	int time = getTime();
	float dt = (time - lastTime) * 0.001f;
	lastTime = time;

	while (dt > _EPS) {
	    deltaTime = _min(0.004f, dt); // 0.004 s - maximum deltaTime per update
        game->update();
        dt -= deltaTime;
	}
}

void Core::render() {
	Render::resetStates();
    game->render();

// fps meter
    fps++;
    int time = getTime();
    if (time >= fpsTime) {
        fpsTime = time + 1000;
        LOG("fps: %d\n", fps);
        fps = 0;
    }
}

void Core::inputEvent(const InputEvent &e) {
	input->inputEvent(e);
    game->inputEvent(e);
}
//}
