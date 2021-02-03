// Made by RequestFX#1541

#pragma once

class Module {
private:
	std::string name, description;
	bool toggled;
	int key;
public:
	Player* player;
	Game* game;
	Mem* mem;

	Module(std::string name, std::string description) {
		this->name = name;
		this->description = description;

		this->key = 0;
		this->toggled = false;
	}

	void setGame(Game* game) {
		this->game = game;
	}

	void setMem(Mem* mem) {
		this->mem = mem;
	}

	void setPlayer(Player* player) {
		this->player = player;
	}

	std::string getName() {
		return name;
	}

	std::string getDescription() {
		return description;
	}

	int getKey() {
		return key;
	}

	void setKey(int key) {
		this->key = key;
	}

	bool isToggled() {
		return toggled;
	}

	void setToggled(bool toggled) {
		this->toggled = toggled;
	}

	void toggle() {
		toggled = !toggled;
		std::cout << std::boolalpha;
		std::cout << getName() << ": " << isToggled() << "\n";
	}
};