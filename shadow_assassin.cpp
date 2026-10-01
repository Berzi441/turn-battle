#include "shadow_assassin.h"
//影魔杀：造成1点真实伤害，使敌方沉默并破甲一回合
void shadow_assassin::shadow_attack(character* opponent) {
	realattack(opponent, 1);
	silent(opponent, 1);
	broken(opponent, 1);
}
//影遁：放逐自身1回合
void shadow_assassin::shadow_hide(void) {
	exiled(1);
}
//如影随行：获得1层先手光环和1点免伤
void shadow_assassin::walk_like_shadow(void) {
	gain_prior(1);
	gain_damage_immunity(1);
}
std::string shadow_assassin::describe_skill(int skill_id, const character* opponent) const {
    if (skill_id == 3)
        return std::string("如影随行") + (pp[3] == 2 ? "x2" : "") +
               "：获得1层先手光环和1点免伤(武器技)";
    if (skill_id != 0) return character::describe_skill(skill_id, opponent);
    std::string result = "普通攻击：造成" + std::to_string(attack_value());
    bool knife_ready = weapon->durable > 0 && weapon->atkup > 0 &&
                       !statuses.has(StatusType::Entangle);
    if (knife_ready) result += "+" + std::to_string(weapon->atkup);
    result += "点伤害";
    if (knife_ready) result += "(普攻使敌方受到伤害则重伤敌方一回合，消耗1点耐久)";
    return result;
}

void shadow_assassin::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "对" << opponent->name << "使用了影魔杀" << endl;
		shadow_attack(opponent);
		break;
	case 2:
		cout << name << "使用了影遁" << endl;
		shadow_hide();
		break;
	case 3:
		cout << name << "使用了如影随形" << endl;
		walk_like_shadow();
		break;
	}
}
