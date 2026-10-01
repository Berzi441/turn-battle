#pragma once
#include "character.h"
class jungle_wizard :
	public character
{
public:
	jungle_wizard() : character() {
		name = "丛林法师";
		ATK = 3;
		DEF = 2;
		HP = 5;
		HPMAX = 5;
		skill_count = 4;
		skill_descriptions[1] = "飞叶快刀：造成2点伤害，然后造成2点穿透伤害";
		skill_descriptions[2] = "弹射叶刃：造成3点伤害，敌方受到伤害则追加3点延迟1回合的伤害";
		skill_descriptions[3] = "青藤缠绕：造成3点伤害，缠绕敌方1回合(武器技)";
		skill_descriptions[4] = "花草护体：获得2点免伤(武器技)";
		skill_rules[3] = {SkillRequirement::Weapon};
		skill_rules[4] = {SkillRequirement::Weapon};
		set_skill_uses({1, 1, 1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = 0; spd[4] = 0;
		*weapon = Weapon("青藤法杖", 1, 0, 0, 1, 2);
	}
	//飞叶快刀：造成2点伤害，然后造成2点穿透伤害
	void fly_leaves(character* opponent);
	//弹射叶刃：造成3点伤害，若敌方受到伤害则追加3点延迟1回合的伤害
	void launch_leaves(character* opponent);
	//青藤缠绕：造成3点伤害，缠绕敌方1回合
	void cirrus_intertwine(character* opponent);
	//花草护体：获得2点免伤
	void plants_protect(void);

	std::string describe_skill(int skill_id, const character* opponent) const override;
	void use_skill(int num, character* opponent) override;
};
