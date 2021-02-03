// Made by RequestFX#1541

#pragma once

#include "Vals.hpp"

using namespace vals::netvars;
using namespace vals::signatures;

class Game {
private:
	Mem* mem;
public:
	inline static module gameaddr, engineaddr, shaderdx9addr, vstdLibaddr;

	Game(Mem* mem) {
		this->mem = mem;
	}

	void getProcess() {
		do {
			mem->getProcessID("csgo.exe");
		} while (!mem->procID);
	}

	void getClient() {
		do {
			gameaddr = mem->getModule(mem->procID, "client.dll");
		} while (!gameaddr.dwBase);
	}

	void getEngine() {
		do {
			engineaddr = mem->getModule(mem->procID, "engine.dll");
		} while (!engineaddr.dwBase);
	}

	void getShaderDX9() {
		do {
			shaderdx9addr = mem->getModule(mem->procID, "shaderapidx9.dll");
		} while (!shaderdx9addr.dwBase);
	}

	void getVstdLib() {
		do {
			vstdLibaddr = mem->getModule(mem->procID, "vstdlib.dll");
		} while (!vstdLibaddr.dwBase);
	}

	std::string getGameDirectory() {
		char buffer[255];
		mem->readString(engineaddr.dwBase + dwGameDir, buffer, sizeof(buffer));
		return std::string(buffer);
	}

	std::string getMapDirectory() {
		uintptr_t engineptr = mem->readMem<uintptr_t>(engineaddr.dwBase + dwClientState);

		char buffer[255];
		mem->readString(engineptr + dwClientState_MapDirectory, buffer, sizeof(buffer));
		return std::string(buffer);
	}
};