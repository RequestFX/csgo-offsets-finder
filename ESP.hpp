// Made by RequestFX#1541

#pragma once

#include "Module.hpp"

class ESP : public Module {
private:
	GlowStruct glowStruct;
public:
	ESP(Game* game, Mem* mem, Player* player) : Module("ESP", "Draws an outline around entities") {
		// https://keycode.info/
		setKey(117);

		setGame(game);
		setMem(mem);
		setPlayer(player);
	}

	void myTeam() {
		GlowStruct* glowStruct = new GlowStruct();
		glowStruct->setup(0.2, 0.5, 0.8, 0.5, 1, true, false, false);

		mem->writeMem<GlowStruct>(Player::glowManager + ((player->entityGlowIndex * 0x38) + 0x4), *glowStruct);
		delete glowStruct;
	}

	void enemyTeam() {
		GlowStruct* glowStruct = new GlowStruct();
		if (player->getIsDefusing(player->entity))
			glowStruct->setup(1, 1, 1, 0.8, 1, true, false, false);
		else {
			glowStruct->setup(player->entityHealth * -0.01 + 1, player->entityHealth * 0.01, 0, 0.7, 1, true, false, false);
		}

		mem->writeMem<GlowStruct>(Player::glowManager + ((player->entityGlowIndex * 0x38) + 0x4), *glowStruct);
		delete glowStruct;
	}

	void eventRender2D() {

		for (int i = 1; i < 64; i++) {

			player->setEntity(mem->readMem<uintptr_t>(game->gameaddr.dwBase + dwEntityList + i * 0x10));
			if (!player->entity) continue;

			player->getIsDormant(player->entity);
			if (player->entityDormant) continue;

			player->getHealth(player->entity);
			if (player->entityHealth <= 0) continue;

			player->getTeam(player->entity);
			player->getGlowIndex(player->entity);

			if (!player->isEntityEnemy())
				myTeam();
			else
				enemyTeam();
		}
	}
};