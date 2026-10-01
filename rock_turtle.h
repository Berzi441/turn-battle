#pragma once
#include "character.h"
class rock_turtle :
	public character
{
public:

	rock_turtle() : character() {
		name = "岩盾龟";
		ATK = 3;
		DEF = 2;
		HP = 3;
		HPMAX = 3;
		SHD = 5;

		skill_count = 2;
		passive = std::make_unique<TurtleShieldPassive>();
		skill_descriptions[1] = "落岩术：造成4点伤害，敌方护甲小于我方则暴击";
		skill_descriptions[2] = "弱体术：造成3点伤害，虚弱敌方1回合";
		set_skill_uses({1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0;
	}
	//落岩术：造成4点伤害，若敌方护甲小于我方则暴击
	void rock_fall(character* opponent);
	//弱体术：造成3点伤害，虚弱敌方1回合
	void weakness_magic(character* opponent);

	void use_skill(int num, character* opponent) override;
};
