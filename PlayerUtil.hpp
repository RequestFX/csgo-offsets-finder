// Made by RequestFX#1541

#pragma once

#include "Vals.hpp"

using namespace vals::netvars;
using namespace vals::signatures;

struct GlowStruct {
	float red, green, blue, alpha;

	BYTE buffer[8];
	float healthIndicator;
	BYTE buffer1[4];
	bool renderOccluded;
	bool renderUnoccluded;
	bool fullBloom;

	void setup(float red, float green, float blue, float alpha,
		float healthIndicator, bool renderOccluded, bool renderUnoccluded, bool fullBloom) {
		this->red = red;
		this->green = green;
		this->blue = blue;
		this->alpha = alpha;
		this->healthIndicator = healthIndicator;
		this->renderOccluded = renderOccluded;
		this->renderUnoccluded = renderUnoccluded;
		this->fullBloom = fullBloom;
	}
};

class Player {
private:
	Game* game;
	Mem* mem;
public:
	inline static uintptr_t lPlayer, lTeam, glowManager;
	uintptr_t entity, entityTeam, entityHealth, entityGlowIndex, entityDormant, entityDefusing;

	Player(Game* game, Mem* mem) {
		this->mem = mem;
		this->game = game;
	}

	void readLPlayer() {
		do {
			lPlayer = mem->readMem<uintptr_t>(game->gameaddr.dwBase + dwLocalPlayer);
		} while (!lPlayer);
	}

	void readGlowManager() {
		glowManager = mem->readMem<uintptr_t>(game->gameaddr.dwBase + dwGlowObjectManager);
	}

	void readLTeam() {
		lTeam = mem->readMem<uintptr_t>(lPlayer + m_iTeamNum);
	}

	uintptr_t setEntity(uintptr_t entityaddr) {
		return entity = entityaddr;
	}

	uintptr_t getTeam(uintptr_t entity) {
		return entityTeam = mem->readMem<uintptr_t>(entity + m_iTeamNum);
	}

	uintptr_t getHealth(uintptr_t entity) {
		return entityHealth = mem->readMem<uintptr_t>(entity + m_iHealth);
	}

	uintptr_t getGlowIndex(uintptr_t entity) {
		return entityGlowIndex = mem->readMem<uintptr_t>(entity + m_iGlowIndex);
	}

	uintptr_t getIsDormant(uintptr_t entity) {
		return entityDormant = mem->readMem<uintptr_t>(entity + m_bDormant);
	}

	uintptr_t getIsDefusing(uintptr_t entity) {
		return entityDefusing = mem->readMem<uintptr_t>(entity + m_bIsDefusing);
	}

	bool isEntityEnemy() {
		return lTeam != entityTeam;
	}
};