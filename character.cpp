#include "character.h"
#include <sstream>
//状态栏
void character::state_list() {
    std::ostringstream details;
    statuses.print(details);
    for (const auto& effect : delayed_damage) {
        if (effect.damage > 0)
            details << "延迟伤害" << effect.damage << "点（"
                    << (effect.remaining_half_turns + 1) / 2 << "） ";
    }
    if (passive) passive->show_state(details);
    std::string summary = details.str();
    while (!summary.empty() && summary.back() == ' ') summary.pop_back();
    cout << "  状态:" << (summary.empty() ? "无" : summary) << endl;
}
//展示武器和护具
void character::show_equipment() {
	if (defence->name != "") {
		cout << "  护具:";
		cout << "#" << defence->name << "#";
		if (defence->hp <= 0) cout << "(已失效)";
		else if (defence->equip > 0) cout << "(已装备)";
		else cout << "(未装备)";
		cout << " 可吸收伤害:" << defence->hp << " 防御:" << defence->def;
		if (!defence->passive.empty()) cout << " " << defence->passive;
		cout << endl;
	}
	if (weapon->name != "") {
		cout << "  武器:";
		cout << "#" << weapon->name << "#";
		if (weapon->durable <= 0 || statuses.has(StatusType::Entangle)) {
			cout << "(已失效)";
		}
		else if (weapon->equip > 0) {
			cout << "(已装备)";
		}
		else {
			cout << "(未装备)";
		}
		cout << " 耐久:" << weapon->durable;
		if (weapon->atkup > 0 && weapon->durable > 0 && !statuses.has(StatusType::Entangle))
			cout << " 普攻伤害提升" << weapon->atkup << "点";
		if (weapon->defup > 0 && weapon->durable > 0 && !statuses.has(StatusType::Entangle))
			cout << " 防御提升" << weapon->defup << "点";
		if (weapon->skillup > 0 && weapon->durable > 0 && !statuses.has(StatusType::Entangle))
			cout << " 技能伤害提升" << weapon->skillup << "点";
		if (weapon->passive != "")
			cout << " " << weapon->passive;
		cout << endl;
	}
}
//展示单个角色的属性；显示双方时复用同一份逻辑。
void character::show_one() {
	cout << name << " 攻击:" << attack_value();
	if (weapon->atkup > 0 && weapon->durable > 0) cout << "+" << weapon->atkup;
	cout << " 防御:" << defence_value();
	if (weapon->defup > 0 && weapon->durable > 0) cout << "+" << weapon->defup;
	cout << " 生命:" << HP << "/" << HPMAX;
	if (absolute_priority()) cout << " 速度：绝对先手";
	else cout << " 速度:" << speed_value();
	if (SHD > 0) cout << " 护盾:" << SHD;
	if (damage_immunity > 0) cout << " 免伤:" << damage_immunity;
	cout << endl;
	state_list();
	if (domain) {
		cout << "  领域:";
		domain->show(cout, *this);
	}
	show_equipment();
}
void character::show(character* opponent) {
	show_one();
	cout << endl;
	opponent->show_one();
	cout << endl;
}

//每个角色都有自己的模组
std::string character::describe_skill(int skill_id, const character*) const {
	if (skill_id == 0) return "普通攻击：造成" + std::to_string(attack_value()) + "点伤害";
	return skill_descriptions[skill_id];
}
void character::show_skill(character* opponent) {
	if (passive) passive->show(cout, *this, opponent);
	for (int id = 0; id <= skill_count; ++id) {
		cout << id << "->" << describe_skill(id, opponent);
		if (id == 0) cout << " (无限次)";
		else cout << " (次数:" << pp[id] << "/" << max_pp[id] << ")";
		cout << endl;
	}
	cout << endl;
}
void character::use_skill(int, character*) {}
bool character::select_skill(int num) {
	// 普攻无限次；SkipAction 是系统内部标记，不能作为玩家输入的技能编号。
	if (num == 0) return true;
	if (num < 1 || num >= SkillSlots || num > skill_count) {
		cout << "不存在此技能，请重新输入" << endl;
		return false;
	}
	if (pp[num] <= 0) {
		cout << "技能使用次数不足，请重新输入" << endl;
		return false;
	}

	const SkillRule& rule = skill_rules[num];
	if (rule.requirement == SkillRequirement::Domain &&
		(!domain || !domain->is_open() || statuses.has(StatusType::Entangle))) {
		cout << "领域未开启或处于缠绕状态，无法使用领域技" << endl;
		return false;
	}
	if (rule.requirement == SkillRequirement::Weapon &&
		(!weapon || weapon->durable <= 0 || statuses.has(StatusType::Entangle))) {
		cout << "武器耐久不足或处于缠绕状态，无法使用武器技" << endl;
		return false;
	}
	if (rule.requirement == SkillRequirement::ChannelSecondStage &&
		pp[num] == 1 && channel.remaining_half_turns == 0) {
		cout << name << "引导失败，技能失效" << endl;
		return false;
	}
	if (rule.weapon_cost > 0 && (!weapon || weapon->durable < rule.weapon_cost)) {
		cout << "武器耐久不足，请重新输入" << endl;
		return false;
	}

	// 所有检查通过后再扣资源，失败选择不会消耗次数或耐久。
	--pp[num];
	if (rule.weapon_cost > 0) weapon->durable -= rule.weapon_cost;
	return true;
}
//攻击通用模板
int character::attack(character* opponent, int damage) {
	if (opponent->statuses.has(StatusType::Exile)) {
		cout << opponent->name << "处于放逐状态，无法对其造成伤害" << endl;
		if (statuses.has(StatusType::Penetration)) statuses.remove(StatusType::Penetration);//穿透消失
		return 0;
	}
	judge_in_attack(opponent, damage);
	int shield_absorbed = 0;
	while (opponent->SHD > 0 && damage > 0) {
		opponent->SHD--;
		damage--;
		shield_absorbed++;
	}
	if (shield_absorbed != 0) cout << opponent->name << "的护盾吸收了" << shield_absorbed << "点伤害！" << endl;
	int defence = opponent->defence_value();
	judge_attack_kind(opponent, defence, damage);
	if (damage > defence) {
		int immunity_absorbed = 0;
		damage -= defence;
		judge_over_attack(opponent, damage);
		while (opponent->damage_immunity > 0 && damage > 0) {
			opponent->damage_immunity--;
			damage--;
			immunity_absorbed++;
		}
		if (immunity_absorbed) cout << opponent->name << "免疫了" << immunity_absorbed << "点伤害！" << endl;
		opponent->HP -= damage;
		cout << name << "对" << opponent->name << "造成了" << damage << "点伤害！" << endl;
		return damage;
	}
	cout << name << "对" << opponent->name << "未造成伤害" << endl;
	return 0;
}
//普通攻击
int character::generalattack(character* opponent) {
	int damage = attack_value();
	//武器对于普通攻击的加成
	bool weapon_used = weapon->durable > 0 && weapon->atkup > 0 && !statuses.has(StatusType::Entangle);
	if (weapon_used) {
		damage += weapon->atkup;
		weapon->durable--;
	}
	damage = attack(opponent, damage);
	if (passive) passive->after_basic_attack(*this, *opponent, damage);
	//暗影匕首被动
	if (weapon->name == "暗影匕首" && weapon_used && damage > 0) {
		cursed(opponent, 1);
	}
	//神圣之主领域效果
	if (domain) domain->after_basic_attack(*this, *opponent);
	return damage;
}
//真实伤害
int character::realattack(character* opponent, int damage) {
	if (opponent->statuses.has(StatusType::Exile)) {
		cout << opponent->name << "处于放逐状态，无法对其造成伤害" << endl;
		return 0;
	}
	opponent->HP -= damage;
	cout << name << "对" << opponent->name << "造成了" << damage << "点真实伤害！" << endl;
	return damage;
}

//出手时判定 
void character::judge_in_attack(character* opponent, int& damage) {
	//对强化的判定
	if (statuses.has(StatusType::Strengthen)) {
		damage++;
		statuses.consume(StatusType::Strengthen);
	}
	//对弱化的判定
	if (statuses.has(StatusType::Weaken)) {
		damage--;
		statuses.consume(StatusType::Weaken);
	}
	if (passive) damage += passive->bonus_damage(*opponent);
}
//破甲判定
void character::judge_attack_kind(character* opponent, int& defence, int& damage) {
	//对护具的判定
	if (opponent->defence->equip > 0 && opponent->defence->hp > 0 && !opponent->statuses.has(StatusType::Entangle)) {
		if (statuses.has(StatusType::Penetration)) {
			opponent->defence->hp -= damage;
			cout << "此次伤害附带穿透，" << opponent->name << "的" << opponent->defence->name << "受到穿透伤害" << damage << "点" << endl;
		}
		else if (damage > opponent->defence->def) {
			damage -= opponent->defence->def;
		int armour_absorbed = 0;
			while (damage != 0 && opponent->defence->hp != 0) {
				damage--;
				opponent->defence->hp--;
				armour_absorbed++;
			}
			cout << opponent->name << "的" << opponent->defence->name << "吸收了" << armour_absorbed << "点伤害" << endl;
		}
		if (opponent->defence->hp <= 0) {
			cout << opponent->name << "的" << opponent->defence->name << "已失效" << endl;
			if (opponent->defence->name == "冰灵帽") opponent->ice_mark(this, 1);
		}
	}
	//对破甲的判定
	if (opponent->statuses.has(StatusType::ArmorBreak)) {
		int defence_reduction = defence / 2;
		defence -= defence_reduction;
		cout << "此次攻击附带破甲，在此次攻击中，" << opponent->name << "的防御将减少" << defence_reduction << "点, 相当于" << defence << "点!" << endl;
	}
	//对冰冻的判定
	if (opponent->statuses.has(StatusType::Freeze) && !statuses.has(StatusType::Penetration)) {
		damage = 0;
		return;
	}
	//对穿透的判定
	if (statuses.has(StatusType::Penetration)) {
		defence = 0;
		statuses.consume(StatusType::Penetration);
		cout << "此次攻击附带穿透，在此次攻击中，" << name << "将无视敌方防御!" << endl;
		if (opponent->statuses.has(StatusType::Freeze)) {
			opponent->statuses.consume(StatusType::Freeze);
			cout << "穿透伤害对冰冻状态敌人造成伤害，冰冻状态解除" << endl;
		}
	}
}
//出手后追加伤害
void character::judge_over_attack(character* opponent, int& damage) {
	//对暴击的判定
	int i = rand() % 50 + 1;
	if (i == 1 && !statuses.has(StatusType::Critical)) {
		statuses.add(StatusType::Critical);
		cout << name << "触发了角色自带的2%的暴击率！" << endl;
	}
	if (statuses.has(StatusType::Critical)) {
		damage = damage * 3 / 2;
		statuses.consume(StatusType::Critical);
		cout << "此次伤害暴击，敌方将受到1.5倍伤害！" << endl;
	}
	//对冻伤的判定
	if (opponent->statuses.has(StatusType::Frostbite)) {
		int increase = 0;
		while (opponent->statuses.has(StatusType::Frostbite) && damage > 0) {
			damage++;
			opponent->statuses.consume(StatusType::Frostbite);
			increase++;
		}
		cout << opponent->name << "处于冻伤状态，此次伤害将增加" << increase << "点" << endl;
	}
	//对圣盾的判定
	if (opponent->statuses.has(StatusType::DivineShield)) {
		damage = damage / 2;
		opponent->statuses.consume(StatusType::DivineShield);
		cout << opponent->name << "拥有圣盾，此次伤害变为原先的一半(向下取整)" << endl;
	}
}
//判定施加debuff是否成功(是否免疫负面效果）
bool character::judge_debuff_useful(character* opponent) {
	if (opponent->statuses.has(StatusType::NegativeImmunity)) {
		opponent->statuses.consume(StatusType::NegativeImmunity);
		if (opponent->name == "风龙领主") {
			cout << "风龙领主的风罡被触发" << endl;
			opponent->recover(1);
		}
		return 0;
	}
	if (opponent->statuses.has(StatusType::Exile)) {
		cout << opponent->name << "处于放逐状态，无法对其施加负面状态" << endl;
		return 0;
	}
	return 1;
}
