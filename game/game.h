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
#ifndef GAME_H
#define GAME_H

#include "input.h"
#include "render2d.h"
#include "scene.h"

#define SPRITE_COUNT 5

struct Game {	
	
	SpriteBatch batch;
	Sprite *sprites[SPRITE_COUNT];

	Camera *camera;

	Material *mat;
    Game();
    ~Game();
    void pause();
	void resume();
    void update();
    void render();
    void inputEvent(const InputEvent &e);
};

#endif // GAME_H
