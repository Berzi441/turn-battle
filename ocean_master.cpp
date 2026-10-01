#include "ocean_master.h"
//巨浪拍打：造成4点吸血伤害
void ocean_master::huge_waves(character* opponent) {
	int damage_dealt = attack(opponent, 4);
	suck_blood(opponent, damage_dealt);
}
//海洋之心：引导一回合，恢复2点生命，下回合获得3点生命
void ocean_master::ocean_heart(void) {
	channel.remaining_half_turns = 2;
	recover(2);
	cout << "海洋领主引导中" << endl;
}
//戟定乾坤：造成4点伤害，驱散敌方正面状态
void ocean_master::trident_attack(character* opponent) {
	attack(opponent, 4);
	dispel_buff(opponent);
}
//雨幕天华：弱化敌方1回合，强化我方1回合
void ocean_master::rain_bless(character* opponent) {
	reduced(opponent, 1);
	strong(1, 1);
}
std::string ocean_master::describe_skill(int skill_id, const character* opponent) const {
    if (skill_id != 0) return character::describe_skill(skill_id, opponent);
    std::string result = "普通攻击：造成" + std::to_string(attack_value());
    if (weapon->durable > 0 && weapon->atkup > 0 && !statuses.has(StatusType::Entangle))
        result += "+" + std::to_string(weapon->atkup);
    result += "点伤害";
    if (weapon->durable > 0 && weapon->atkup > 0 && !statuses.has(StatusType::Entangle))
        result += "(消耗1点耐久)";
    return result;
}

void ocean_master::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "对" << opponent->name << "使用了巨浪拍打" << endl;
		huge_waves(opponent);
		break;
	case 2:
		cout << name << "使用了海洋之心" << endl;
		ocean_heart();
		break;
	case 3:
		cout << name << "对" << opponent->name << "使用了戟定乾坤" << endl;
		trident_attack(opponent);
		break;
	case 4:
		cout << name << "对" << opponent->name << "使用了雨幕天华" << endl;
		rain_bless(opponent);
		break;
	}
}

void ocean_master::on_channel_complete() {
    cout << "海洋之主引导完成" << endl;
    gain_HP(3);
}
