#pragma once
#include "character.h"
class wind_dragon :
	public character
{
public:

	wind_dragon() : character() {
		name = "风龙领主";
		ATK = 3;
		DEF = 1;
		HP = 7;
		HPMAX = 7;

		skill_count = 5;
		domain = std::make_unique<WindDomain>();
		skill_descriptions[1] = "风神护体：开启风神领域，获得4点护盾";
		skill_descriptions[2] = "风旋：造成3*2点伤害，敌方每受到1点伤害获得1点护盾(领域技)";
		skill_descriptions[3] = "风神祝福：强化接下来两次攻击(后手)(领域技)";
		skill_descriptions[4] = "风罡霸体：获得1层风罡，持续时间免疫技能效果,触发时恢复1点生命";
		skill_descriptions[5] = "风龙吐息：造成4点伤害，敌方受到的伤害转化为我方护盾";
		skill_rules[2] = {SkillRequirement::Domain};
		skill_rules[3] = {SkillRequirement::Domain};
		set_skill_uses({1, 1, 1, 1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = -1; spd[4] = 0; spd[5] = 0;
	}
	//风神护体：开启【风神领域】，获得4点护盾
	void wind_protect(void);
	//风旋：造成3*2点伤害，敌方每受到1点伤害获得1点护盾
	void wind_brow(character* opponent);
	//风神祝福：强化接下来两次攻击（后手）
	void wind_bless(void);
	//风罡霸体：获得1层风罡，持续时间免疫技能效果,触发时恢复1点生命
	void wind_immune(void);
	//风龙吐息：造成4点伤害，敌方受到的伤害转化为我方护盾
	void wind_roar(character* opponent);

	void use_skill(int num, character* opponent) override;
};
