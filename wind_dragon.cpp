#include "wind_dragon.h"
//风神护体：开启【风神领域】，获得4点护盾
void wind_dragon::wind_protect(void) {
	gain_shd(4);
	domain->open();
	cout << name << "开启了【风神领域】" << endl;
}
//风旋：造成3*2点伤害，敌方每受到1点伤害获得1点护盾
void wind_dragon::wind_brow(character* opponent) {
	int first_hit_damage = attack(opponent, 3);
	int second_hit_damage = attack(opponent, 3);
	int total_damage = first_hit_damage + second_hit_damage;
	gain_shd(total_damage);
}
//风神祝福：强化接下来两次攻击（后手）
void wind_dragon::wind_bless(void) {
	strong(2);
}
//风罡霸体：获得1层风罡，持续时间免疫技能效果,触发时恢复1点生命
void wind_dragon::wind_immune(void) {
	statuses.add(StatusType::NegativeImmunity);
	cout << name << "获得了1层风罡!" << endl;
}
//风龙吐息：造成4点伤害，敌方受到的伤害转化为我方护盾
void wind_dragon::wind_roar(character* opponent) {
	int damage_dealt = attack(opponent, 4);
	gain_shd(damage_dealt);
}
void wind_dragon::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "使用了风神护体" << endl;
		wind_protect();
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了风旋" << endl;
		wind_brow(opponent);
		break;
	case 3:
		cout << name << "使用了风神祝福" << endl;
		wind_bless();
		break;
	case 4:
		cout << name << "使用了风罡霸体" << endl;
		wind_immune();
		break;
	case 5:
		cout << name << "对" << opponent->name << "使用了风龙吐息" << endl;
		wind_roar(opponent);
		break;
	}
}
