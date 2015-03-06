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

Game::Game() 
{
	
	Default::init();

	atl = TextureAtlas::init(new Stream("data//atlas.tga"), TexExt::extTga, new Stream("data//atlas.atlas"), TextureAtlasExt::extCheetah, true);

	for (int i = 0; i < SPRITE_COUNT; i++) {
		sprites[i] = new Sprite(128.0f, 256.0f, vec2(0.0f, 0.0f));
		sprites[i]->setTextureRegion(atl->getRegion("lander.png"), true);		
		sprites[i]->position = vec3(150 + 50.0f * i, 150 + 50.0f * i, i);		
		
	}			

	//sprites[1]->vertices[0].tc /= 4.0f;
	//sprites[1]->vertices[1].tc /= 4.0f;
	//sprites[1]->vertices[3].tc /= 4.0f;
	//sprites[1]->setSize(5.0f, 5.0f);


	mat = Material::init(Default::spriteShader);
	mat->addTexture(atl, "uDiffuse", 0);	

	camera = new Camera();	
	//camera->setProjParams(0, 0, 20, 20, 45, -1, 100, pmOrtho, pTopLeft);

	font = Font::init(new Stream("data//Times New Roman16.bmp"));
	fbatch = new FontBatch(font);

	text1 = new Text();
	text1->text = "ÏðÎâÅðÊà. Ñîððè çà çàáîð";
	text1->position = vec3(300, 100, 0);
}

Game::~Game() 
{
	for (int i = 0; i < SPRITE_COUNT; i++)
		delete sprites[i];
	delete mat;	
	delete atl;
	delete text1;
	delete font;
	delete fbatch;
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
	camera->update();
	
	mat->bind();	
	batch.begin();
	for (int i = 0; i < SPRITE_COUNT; i++)
		batch.render(sprites[i]);
	batch.end();
	
	fbatch->begin();
		fbatch->render(text1);
	fbatch->end();
	
}

void Game::inputEvent(const InputEvent &e)
{
	
}
