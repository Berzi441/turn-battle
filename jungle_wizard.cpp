#include "jungle_wizard.h"
//飞叶快刀：造成2点伤害，然后造成2点穿透伤害
void jungle_wizard::fly_leaves(character* opponent) {
	int damage = 2;
	if (weapon->durable > 0 && weapon->skillup > 0) {
		damage++;
		weapon->durable--;
	}
	attack(opponent, damage);
	statuses.add(StatusType::Penetration);
	attack(opponent, 2);
}
//弹射叶刃：造成3点伤害，若敌方受到伤害则追加3点延迟1回合的伤害
void jungle_wizard::launch_leaves(character* opponent) {
	int damage = 3;
	if (weapon->durable > 0 && weapon->skillup > 0 && !statuses.has(StatusType::Entangle)) {
		damage++;
		weapon->durable--;
	}
	int damage_dealt = attack(opponent, damage);
	if (damage_dealt > 0) delay_attack(opponent, 1, 3);
}
//青藤缠绕：造成3点伤害，缠绕敌方1回合
void jungle_wizard::cirrus_intertwine(character* opponent) {
	int damage = 3;
	if (weapon->skillup > 0) {
		damage++;
		weapon->durable--;
	}
	attack(opponent, damage);
	intertwined(opponent, 1);
}
//花草护体：获得2点免伤
void jungle_wizard::plants_protect(void) {
	gain_damage_immunity(2);
}
std::string jungle_wizard::describe_skill(int skill_id, const character* opponent) const {
    if (skill_id < 1 || skill_id > 3) return character::describe_skill(skill_id, opponent);
    bool boosted = weapon->durable > 0 && weapon->skillup > 0 &&
                   !statuses.has(StatusType::Entangle);
    std::string extra = boosted ? "+" + std::to_string(weapon->skillup) : "";
    std::string cost = boosted ? "(消耗1点耐久)" : "";
    if (skill_id == 1)
        return "飞叶快刀：造成2" + extra + "点伤害，然后造成2点穿透伤害" + cost;
    if (skill_id == 2)
        return "弹射叶刃：造成3" + extra + "点伤害，敌方受到伤害则追加3点延迟1回合的伤害" + cost;
    return "青藤缠绕：造成3" + extra + "点伤害，缠绕敌方1回合(武器技)" + cost;
}

void jungle_wizard::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "对" << opponent->name << "使用了飞叶快刀" << endl;
		fly_leaves(opponent);
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了弹射叶刃" << endl;
		launch_leaves(opponent);
		break;
	case 3:
		cout << name << "对" << opponent->name << "使用了青藤缠绕" << endl;
		cirrus_intertwine(opponent);
		break;
	case 4:
		cout << name << "使用了花草护体" << endl;
		plants_protect();
		break;
	}
}
