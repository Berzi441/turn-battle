#include "ice_lady.h"
//极度冰点：开启【极寒之域】，装备【冰灵帽】
void ice_lady::extremely_cold(void) {
	domain->open();
	defence->equip = 1;
	cout << name << "开启了【极寒领域】" << endl;
	cout << name << "装备了#冰灵冠#" << endl;
}
//玄冰之咒：对敌方施加1层冻伤和2层冰棱
void ice_lady::ice_spell(character* opponent) {
	colder(opponent, 1);
	ice_mark(opponent, 2);
}
//寒霜冲击：造成3点伤害并沉默敌方一回合，对敌方施加1层冰棱
void ice_lady::snow_attack(character* opponent) {
	attack(opponent, 3);
	silent(opponent, 1);
	ice_mark(opponent, 1);
}
//极寒风暴：施加1层冻伤，造成3点延迟1回合的伤害，再造成3点延迟2回合的伤害
void ice_lady::snow_storm(character* opponent) {
	colder(opponent, 1);
	delay_attack(opponent, 1, 3);
	delay_attack(opponent, 2, 3, 1);
}
void ice_lady::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "使用了极度冰点" << endl;
		extremely_cold();
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了玄冰之咒" << endl;
		ice_spell(opponent);
		break;
	case 3:
		cout << name << "对" << opponent->name << "使用了寒霜冲击" << endl;
		snow_attack(opponent);
		break;
	case 4:
		cout << name << "对" << opponent->name << "使用了极寒风暴" << endl;
		snow_storm(opponent);
		break;
	}
}
