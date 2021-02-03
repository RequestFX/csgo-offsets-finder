// Made by RequestFX#1541

#include "NetVarManager.hpp"

#include <future>
#include <chrono>
#include <thread>

void loading(std::future<void> futureObj) {
	std::cout << "Please wait, client is initialising ";

	while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
		std::cout << ". ";
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void processCheck(Game* game, Mem* mem) {
	std::cout << "Waiting for csgo to start...\n";

	game->getProcess();
	std::cout << "Found Process! ProcID: " << mem->procID << "\n";

	game->getClient();
	std::cout << "Found client.dll! : " << game->gameaddr.dwBase << "\n";

	game->getEngine();
	std::cout << "Found engine.dll! : " << game->engineaddr.dwBase << "\n";

	game->getShaderDX9();
	std::cout << "Found shaderapidx9.dll! : " << game->shaderdx9addr.dwBase << "\n";

	game->getVstdLib();
	std::cout << "Found vstdlib.dll! : " << game->vstdLibaddr.dwBase << "\n";
}

void main() {
	Manager manager;
	manager.startClient();

	auto* game = Manager::instance->game;
	auto* mem = Manager::instance->mem;
	auto* player = Manager::instance->player;
	auto* offsets = Manager::instance->offsets;
	auto* esp = Manager::instance->esp;

	processCheck(game, mem);

	std::promise<void> exitSignal;
	std::future<void> futureObj = exitSignal.get_future();

	std::thread load(&loading, std::move(futureObj));

	auto t1 = std::chrono::system_clock::now();
	offsets->initVals(game, mem);
	auto t2 = std::chrono::system_clock::now();

	exitSignal.set_value();
	load.join();

	std::chrono::duration<float> delta = t2 - t1;
	std::cout << "\nFinished initialising, took: " << delta.count() << "s\n";

	while (!GetAsyncKeyState(VK_END)) {

		player->readLPlayer();
		player->readLTeam();
		player->readGlowManager();

		if (GetAsyncKeyState(esp->getKey()) & 1) {
			esp->toggle();
		}

		if (esp->isToggled())
			esp->eventRender2D();

		Sleep(1);
	}

	manager.stopClient();
}