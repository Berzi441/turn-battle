#include <iostream>
#include <ctime>
#include <random>
#include <memory>
#include <string>
#include <initializer_list>
#include"state.h"
#include"battle_effects.h"
#include"weapon.h"
#pragma once
using namespace std;
class character
{
public:
	string name;
	int ATK;
	int DEF;
	int HP;
	int HPMAX;
	int SHD;
	int SPEED;
	int damage_immunity;
	std::unique_ptr<Passive> passive;
	std::unique_ptr<Domain> domain;
	StatusSet statuses;
	ChannelState channel;
	std::array<DelayedDamage, 2> delayed_damage{}; // 保留原来的两段延迟伤害槽。
	static constexpr int SkillSlots = 10;
	static constexpr int SkipAction = -1; // 系统内部的跳过行动标记，不是技能编号
	enum class SkillRequirement { Normal, Domain, Weapon, ChannelSecondStage };
	struct SkillRule {
		SkillRequirement requirement = SkillRequirement::Normal;
		int weapon_cost = 0; // 选择技能时扣除的耐久；技能执行内的消耗仍由技能处理。
	};
	int skill_count = 0; // 主动技能编号为 1～skill_count，普攻为 0。
	SkillRule skill_rules[SkillSlots]{};
	std::array<std::string, SkillSlots> skill_descriptions{};
	int pp[SkillSlots]{};
	int max_pp[SkillSlots]{};
	int spd[SkillSlots]{};
	std::unique_ptr<Weapon> weapon;
	std::unique_ptr<Defence> defence;
	character() {
		name = "";
		ATK = 0;
		DEF = 0;
		HP = 0;
		HPMAX = 0;
		SHD = 0;
		SPEED = 5;
		damage_immunity = 0;
		weapon = std::make_unique<Weapon>();
		defence = std::make_unique<Defence>();
	}
	virtual ~character() = default;
	void set_skill_uses(std::initializer_list<int> uses) {
		int id = 0;
		for (int count : uses) {
			if (id >= SkillSlots) break;
			pp[id] = max_pp[id] = count;
			++id;
		}
	}
	// ATK/DEF/SPEED 保存基础值；临时状态只在读取当前属性时叠加。
	int attack_value() const { return ATK + statuses.attack_modifier(); }
	int defence_value() const { return DEF + statuses.defence_modifier(); }
	int speed_value() const { return SPEED + statuses.speed_modifier(); }
	int action_speed(int skill_id) const {
		return speed_value() + ((skill_id >= 0 && skill_id < SkillSlots) ? spd[skill_id] : 0);
	}
	bool absolute_priority() const { return domain && domain->absolute_priority(*this); }
	// 只有已扣次数的主动技能才能退还；普攻和跳过行动不计次数。
	void refund_skill(int skill_id) {
		if (skill_id > 0 && skill_id < SkillSlots) ++pp[skill_id];
	}
	//状态栏
	void state_list();
	//展示属性状态
	void show_equipment();
	void show_one();
	void show(character* opponent);
	void show_skill(character* opponent);
	virtual std::string describe_skill(int skill_id, const character* opponent) const;
	bool select_skill(int num); // 所有角色共用，不再各自重写。
	virtual void use_skill(int num, character* opponent);
	// 战斗流程通知独立的被动对象。
	void before_opponent_action(character& opponent) {
		if (passive) passive->before_opponent_action(*this, opponent);
	}
	bool settle_opponent_marks(character& opponent) {
		return passive && passive->settle_opponent_marks(*this, opponent);
	}
	void after_action_settlement(character& opponent) {
		if (passive) passive->after_action(*this, opponent);
	}
	void after_round_settlement(character& opponent) {
		if (passive) passive->after_round(*this, opponent);
	}
	virtual void on_channel_complete() {}
	virtual void on_channel_interrupted() {}
	void interrupt_channel() {
		if (channel.remaining_half_turns > 0) on_channel_interrupted();
		channel.cancel();
	}
	//判定是否被控制
	bool judge_choice(character* opponent, int& selected_skill);
	//出手前判定
	void judge_use_skill(int selected_skill, character* opponent);
	//出手时判定 
	void judge_in_attack(character* opponent, int& damage);
	//破甲判定
	void judge_attack_kind(character* opponent, int& defence, int& damage);
	//出手后追加伤害
	void judge_over_attack(character* opponent, int& damage);
	//判定施加debuff是否成功(是否免疫负面效果）
	bool judge_debuff_useful(character* opponent);
	// 所有普通负面状态共用：免疫检查 → 添加状态 → 输出结果。
	bool apply_status(character* target, StatusType type, int amount);
	//攻击通用模板
	int attack(character* opponent, int damage);
	//普通攻击
	int generalattack(character* opponent);
	//真实伤害
	int realattack(character* opponent, int damage);

	//获得圣盾函数
	void gain_divine_shiled(int num);
	//获得先手光环函数
	void gain_prior(int num);
	//获得护盾
	void gain_shd(int num);
	//获得免伤
	void gain_damage_immunity(int num);
	//强化函数
	void strong(int num, int rounds = 0);
	//虚弱函数
	void weakness(character* opponent, int num);
	//破甲函数
	void broken(character* opponent, int num);
	//麻痹函数
	void bring_numbness(character* opponent, int num);
	//烧伤函数
	void empyrosis(character* opponent, int num);
	//弱化函数
	void reduced(character* opponent, int num);
	//缠绕函数
	void intertwined(character* opponent, int num);
	//沉默函数
	void silent(character* opponent, int num);
	//冰冻函数
	void ice(character* opponent, int num);
	//冻伤函数
	void colder(character* opponent, int num);
	//延迟伤害函数
	void delay_attack(character* opponent, int time, int damage, int second = 0);
	//吸血函数
	void suck_blood(character* opponent, int damage_dealt);
	//恢复函数
	void recover(int recover);
	//获得生命函数
	void gain_HP(int gain);
	//重伤函数
	void cursed(character* opponent, int num);
	//驱散正面状态函数
	void dispel_buff(character* opponent);
	//解除负面状态函数
	void relieve_debuff(void);
	//增加眩晕概率函数(一回合)
	void bring_dizziness_percent(character* opponent, int num);
	//冰棱函数(冰天雪女)
	void ice_mark(character* opponent, int num);
	//放逐函数
	void exiled(int num);
	//汲取函数
	void draw_blood(character* opponent, int num);
};
