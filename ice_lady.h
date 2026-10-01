#pragma once
#include "character.h"
class ice_lady :
	public character
{
public:

	ice_lady() : character() {
		name = "冰天雪女";
		ATK = 3;
		DEF = 1;
		HP = 7;
		HPMAX = 7;

		*defence = Defence("冰灵帽", 0, 5, 1, "被摧毁后对敌方施加1层冰棱");
		skill_count = 4;
		passive = std::make_unique<FrostMarkPassive>();
		domain = std::make_unique<FrostDomain>();
		skill_descriptions[1] = "极度冰点：开启【极寒领域】，装备#冰灵帽#";
		skill_descriptions[2] = "玄冰之咒：对敌方施加1层冻伤和2层冰棱(领域技)";
		skill_descriptions[3] = "寒霜冲击：造成3点伤害并沉默敌方一回合，对敌方施加1层冰棱(领域技)";
		skill_descriptions[4] = "极寒风暴：施加1层冻伤，造成3点延迟1回合的伤害，再造成3点延迟2回合的伤害";
		skill_rules[2] = {SkillRequirement::Domain};
		skill_rules[3] = {SkillRequirement::Domain};
		set_skill_uses({1, 1, 1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = 0; spd[4] = 0;
	}
	//极度冰点：开启【极寒领域】，装备【冰灵帽】
	void extremely_cold(void);
	//玄冰之咒：对敌方施加1层冻伤和2层冰棱
	void ice_spell(character* opponent);
	//寒霜冲击：造成3点伤害并沉默敌方一回合，对敌方施加1层冰棱
	void snow_attack(character* opponent);
	//极寒风暴：造成3点延迟1回合的伤害，再造成3点延迟2回合且附带1层冻伤的伤害
	void snow_storm(character* opponent);

	void use_skill(int num, character* opponent) override;
};
