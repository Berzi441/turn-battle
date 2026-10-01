#pragma once
#include <iostream>
using namespace std;
class Weapon {
public:
	string name;
	int equip;
	int atkup;
	int defup;
	int skillup;
	int durable;
	string passive;
	Weapon(string name = "", int equip = 0, int atkup = 0, int defup = 0, int skillup = 0, int durable = 0, string passive = "") {
		this->equip = equip;
		this->name = name;
		this->atkup = atkup;
		this->defup = defup;
		this->skillup = skillup;
		this->durable = durable;
		this->passive = passive;
	}
};
class Defence {
public:
	string name;
	int equip;
	int hp;
	int def;
	string passive;
	Defence(string name = "", int equip = 0, int hp = 0, int def = 0, string passive = "") {
		this->name = name;
		this->equip = equip;
		this->hp = hp;
		this->def = def;
		this->passive = passive;
	}
}; 