#pragma once
#include "character.h"
class shadow_assassin :
	public character
{
public:
	shadow_assassin() : character() {
		name = "暗影刺客";
		ATK = 3;
		DEF = 2;
		HP = 6;
		HPMAX = 6;
		skill_count = 3;
		skill_descriptions[1] = "影魔杀：造成1点真实伤害，使敌方沉默并破甲一回合";
		skill_descriptions[2] = "影遁：放逐自身1回合，期间无法受到伤害和负面效果，敌方技能仍消耗使用次数";
		skill_descriptions[3] = "如影随行：获得1层先手光环和1点免伤(武器技)";
		skill_rules[3] = {SkillRequirement::Weapon};
		set_skill_uses({1, 1, 1, 2});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = 0;
		*weapon = Weapon("暗影匕首", 1, 1, 0, 0, 3, "普攻使敌方受到伤害则重伤敌方一回合");
	}
	//影魔杀：造成1点真实伤害，使敌方沉默并破甲一回合
	void shadow_attack(character* opponent);
	//影遁：放逐自身1回合，敌方对放逐目标使用的技能仍消耗使用次数
	void shadow_hide(void);
	//如影随行：获得1层先手光环和1点免伤
	void walk_like_shadow(void);

	std::string describe_skill(int skill_id, const character* opponent) const override;
	void use_skill(int num, character* opponent) override;
};
