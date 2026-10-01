#pragma once
#include"character.h"
class ocean_master :
	public character
{
public:
    void on_channel_complete() override;
	ocean_master() : character() {
		name = "海洋之主";
		ATK = 3;
		DEF = 1;
		HP = 7;
		HPMAX = 7;
		skill_count = 4;
		skill_descriptions[1] = "巨浪拍打：造成4点吸血伤害";
		skill_descriptions[2] = "海洋之心：引导一回合，恢复2点生命，下回合获得3点生命";
		skill_descriptions[3] = "戟定乾坤：造成4点伤害，驱散敌方正面状态(武器技，消耗1点耐久)";
		skill_descriptions[4] = "雨幕天华：弱化敌方1回合，强化我方1回合(武器技，消耗0点耐久)";
		skill_rules[3] = {SkillRequirement::Weapon, 1};
		skill_rules[4] = {SkillRequirement::Weapon};
		set_skill_uses({1, 1, 1, 1, 1});
		spd[0] = 0; spd[1] = 0; spd[2] = 0; spd[3] = 0; spd[4] = 0;
		*weapon = Weapon("海石三叉戟", 1, 1, 0, 0, 3);
	}
	//巨浪拍打：造成4点吸血伤害
	void huge_waves(character* opponent);
	//海洋之心：引导一回合，恢复2点生命，下回合获得3点生命
	void ocean_heart(void);
	//戟定乾坤：造成4点伤害，驱散敌方正面状态
	void trident_attack(character* opponent);
	//雨幕天华：弱化敌方1回合，强化我方1回合
	void rain_bless(character* opponent);

	std::string describe_skill(int skill_id, const character* opponent) const override;
	void use_skill(int num, character* opponent) override;
};
