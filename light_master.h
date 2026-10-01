#pragma once
#include "character.h"
class light_master :
	public character
{
public:
	light_master() : character() {
		name = "神圣之主";
		ATK = 3;
		DEF = 2;
		HP = 6;
		HPMAX = 6;

		skill_count = 4;
		domain = std::make_unique<HolyDomain>();
		skill_descriptions[1] = "神圣之心：开启【神圣领域】，获得1层圣盾";
		skill_descriptions[2] = "神圣之手：汲取敌方2点生命上限(领域技)";
		skill_descriptions[3] = "神圣裁决：对敌方造成等同于我方生命上限一半(向上取整)的伤害(领域技)";
		skill_descriptions[4] = "神圣之佑：恢复等同于我方生命上限一半(向上取整)的生命值";
		skill_rules[2] = {SkillRequirement::Domain};
		skill_rules[3] = {SkillRequirement::Domain};
		set_skill_uses({1, 1, 1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = 0; spd[4] = 0;
	}
	//神圣之心：开启【神圣领域】，获得1层圣盾
	void divine_heart(void);
	//神圣之手：汲取敌方2点生命上限
	void divine_hand(character* opponent);
	//神圣裁决：对敌方造成等同于敌方生命上限一半(向上取整)的伤害
	void divine_attack(character* opponent);
	//神圣之佑：恢复等同于我方生命上限一半(向上取整)的生命值
	void divine_protect(void);

	void use_skill(int num, character* opponent) override;
};
