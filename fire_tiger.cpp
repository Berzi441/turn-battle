#include "fire_tiger.h"
//火焰喷射：造成3点伤害，对敌方施加2层烧伤
void fire_tiger::fire_ray(character* opponent) {
	attack(opponent, 3);
	empyrosis(opponent, 2);
}
//火焰牙：造成4点吸血伤害
void fire_tiger::fire_tooth(character* opponent) {
	int damage_dealt = attack(opponent, 4);
	suck_blood(opponent, damage_dealt);
}
void fire_tiger::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "对" << opponent->name << "使用了火焰喷射" << endl;
		fire_ray(opponent);
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了火焰牙" << endl;
		fire_tooth(opponent);
		break;
	}
}
