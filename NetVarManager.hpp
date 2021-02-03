// Made by RequestFX#1541

#pragma once

#include <iostream>

#include "Mem.hpp"
#include "Game.hpp"
#include "Vals.hpp"
#include "NetVar.hpp"
#include "PlayerUtil.hpp"

#include "ESP.hpp"

class Manager {
public:
	Mem* mem;
	Game* game;
	Player* player;
	Vals* offsets;

	ESP* esp;

	inline static Manager* instance;

	Manager() {
		instance = this;
	}

	void startClient() {
		mem = new Mem();
		game = new Game(mem);
		player = new Player(game, mem);
		offsets = new Vals();

		esp = new ESP(game, mem, player);
	}

	void stopClient() {
		delete mem;
		delete game;
		delete player;
		delete offsets;

		delete esp;
	}
};