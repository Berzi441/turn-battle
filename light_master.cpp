#include "light_master.h"
//神圣之心：开启【神圣领域】，获得1层圣盾
void light_master::divine_heart(void) {
	domain->open();
	cout << name << "开启了【神圣领域】" << endl;
	gain_divine_shiled(1);
}
//神圣之手：汲取敌方2点生命上限
void light_master::divine_hand(character* opponent) {
	draw_blood(opponent, 2);
}
//神圣裁决：对敌方造成等同于我方生命上限一半(向上取整)的伤害
void light_master::divine_attack(character* opponent) {
	int damage = (HPMAX + 1) / 2;
	attack(opponent, damage);
}
//神圣之佑：恢复等同于我方生命上限一半(向上取整)的生命值
void light_master::divine_protect(void) {
	int cure = (HPMAX + 1) / 2;
	recover(cure);
}
void light_master::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "使用了神圣之心" << endl;
		divine_heart();
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了神圣之手" << endl;
		divine_hand(opponent);
		break;
	case 3:
		cout << name << "对" << opponent->name << "使用了神圣裁决" << endl;
		divine_attack(opponent);
		break;
	case 4:
		cout << name << "使用了神圣之佑" << endl;
		divine_protect();
		break;
	}
}
