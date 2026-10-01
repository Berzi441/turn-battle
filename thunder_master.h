#pragma once
#include "character.h"
class thunder_master :
	public character
{
public:

	thunder_master() : character() {
		name = "雷霆之主";
		ATK = 3;
		DEF = 1;
		HP = 6;
		HPMAX = 6;
		SHD = 4;

		skill_count = 2;
		passive = std::make_unique<ThunderShieldPassive>();
		skill_descriptions[1] = "雷霆万钧：解除自身负面状态，引导一回合；下次行动自动造成1+2+3+4点伤害并施加1层麻痹";
		skill_descriptions[2] = "雷灵战鼓：攻击属性+1，若敌方处于麻痹状态则敌方下次行动时，眩晕概率增加50%";
		skill_rules[1] = {SkillRequirement::ChannelSecondStage};
		set_skill_uses({1, 2, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0;
	}
	//雷霆万钧：解除自身负面状态，引导一回合 || 造成1+2+3+4点伤害并对敌方施加1层麻痹
	void as_powerful_as_thunderbolt(character* opponent);
	//雷灵战鼓：攻击属性+1，若敌方处于麻痹状态则敌方下次行动时，眩晕概率增加50%
	void thunder_battle_drum(character* opponent);

	std::string describe_skill(int skill_id, const character* opponent) const override;
	void use_skill(int num, character* opponent) override;
	void on_channel_interrupted() override;
};
