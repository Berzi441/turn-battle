#include "thunder_master.h"
//雷霆万钧：解除自身负面状态，引导一回合 || 造成1+2+3+4点伤害并对敌方施加1层麻痹
void thunder_master::as_powerful_as_thunderbolt(character* opponent) {
	if (pp[1] == 1) {
		channel.remaining_half_turns = 4;
		channel.forced_skill_rounds = 2; // 本回合结束减为1，下回合自动选择第二段。
		relieve_debuff();
	}
	if (pp[1] == 0 && channel.remaining_half_turns > 0) {
		attack(opponent, 1);
		attack(opponent, 2);
		attack(opponent, 3);
		attack(opponent, 4);
		bring_numbness(opponent, 1);
		channel.cancel();
	}
}
void thunder_master::on_channel_interrupted() {
	// 第一段消耗的次数在引导被打断时返还；若第二段已选，其次数由行动结算返还。
	refund_skill(1);
	cout << name << "的雷霆万钧被打断，返还技能次数" << endl;
}
//雷灵战鼓：攻击属性+1，若敌方处于麻痹状态则敌方下次行动时，眩晕概率增加50%
void thunder_master::thunder_battle_drum(character* opponent) {
	ATK++;
	if (opponent->statuses.has(StatusType::Numbness)) bring_dizziness_percent(opponent, 50);
	cout << "雷霆之主攻击提升了1点" << endl;
}
std::string thunder_master::describe_skill(int skill_id, const character* opponent) const {
    std::string result = character::describe_skill(skill_id, opponent);
    if (skill_id == 0 && opponent && !opponent->statuses.has(StatusType::Numbness))
        result += "(并对敌方施加1层麻痹)";
    return result;
}

void thunder_master::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		if (pp[1] == 1) cout << name << "使用了雷霆万钧1段" << endl; else cout << name << "对" << opponent->name << "使用了雷霆万钧2段" << endl;
		as_powerful_as_thunderbolt(opponent);
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了雷灵战鼓" << endl;
		thunder_battle_drum(opponent);
		break;
	}
}
