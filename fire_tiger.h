#pragma once
#include "character.h"
class fire_tiger :
	public character
{
public:
	fire_tiger() : character() {
		name = "赤焰猛虎";
		ATK = 3;
		DEF = 2;
		HP = 6;
		HPMAX = 6;
		skill_count = 2;
		passive = std::make_unique<BurnBonusPassive>();
		skill_descriptions[1] = "火焰喷射：造成3点伤害，对敌方施加2层烧伤";
		skill_descriptions[2] = "火焰牙：造成4点吸血伤害";
		set_skill_uses({1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0;
	}
	//火焰喷射：造成3点伤害，对敌方施加2层烧伤
	void fire_ray(character* opponent);
	//火焰牙：造成4点吸血伤害
	void fire_tooth(character* opponent);

	void use_skill(int num, character* opponent) override;
};
