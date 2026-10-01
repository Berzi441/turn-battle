#include "character.h"

// 状态效果统一入口。技能只需指定类型与层数/回合数。
bool character::apply_status(character* target, StatusType type, int amount) {
    if (amount <= 0) return false;
    const auto& rule = status_rule(type);
    if (!judge_debuff_useful(target)) {
        cout << name << "没能对" << target->name << "施加" << rule.name << "!" << endl;
        return false;
    }
    target->statuses.add(type, amount);
    if (type == StatusType::Wound)
        cout << target->name << "被" << name << "重伤" << amount << "回合!" << endl;
    else if (type == StatusType::ExtraStunChance)
        cout << name << "使" << target->name << "下回合眩晕概率增加" << amount << "%" << endl;
    else
        cout << name << "对" << target->name << "施加了" << amount
             << (rule.lifetime == StatusLifetime::HalfTurns ||
                 type == StatusType::Silence || type == StatusType::Freeze ? "回合" : "层")
             << rule.name << "!" << endl;
    return true;
}

void character::gain_divine_shiled(int num) {
    statuses.add(StatusType::DivineShield, num);
    cout << name << "获得了" << num << "层圣盾";
}
void character::gain_prior(int num) {
    statuses.add(StatusType::Priority, num);
    cout << name << "获得了" << num << "层先手光环!" << endl;
    cout << name << "速度提高了" << num << "点!" << endl;
}
void character::gain_shd(int num) {
    SHD += num;
    cout << name << "获得了" << num << "点护盾!" << endl;
}
void character::gain_damage_immunity(int num) {
    damage_immunity += num;
    cout << name << "获得了" << num << "点免伤!" << endl;
}
void character::strong(int num, int rounds) {
    if (rounds > 0) {
        statuses.add_timed(StatusType::Strengthen, num, rounds, 1);
        cout << name << "获得了" << rounds << "回合强化!" << endl;
    } else {
        statuses.add(StatusType::Strengthen, num);
        cout << name << "获得了" << num << "层强化!" << endl;
    }
}

// 保留技能原来的调用方式，计时和属性修正交给状态规则表。
void character::weakness(character* target, int rounds) { apply_status(target, StatusType::Weakness, rounds); }
void character::broken(character* target, int rounds) { apply_status(target, StatusType::ArmorBreak, rounds); }
void character::bring_numbness(character* target, int layers) { apply_status(target, StatusType::Numbness, layers); }
void character::empyrosis(character* target, int layers) { apply_status(target, StatusType::Burn, layers); }
void character::reduced(character* target, int rounds) { apply_status(target, StatusType::Weaken, rounds); }
void character::intertwined(character* target, int rounds) { apply_status(target, StatusType::Entangle, rounds); }
void character::silent(character* target, int rounds) { apply_status(target, StatusType::Silence, rounds); }
void character::ice(character* target, int rounds) { apply_status(target, StatusType::Freeze, rounds); }
void character::colder(character* target, int layers) { apply_status(target, StatusType::Frostbite, layers); }
void character::cursed(character* target, int rounds) { apply_status(target, StatusType::Wound, rounds); }
void character::bring_dizziness_percent(character* target, int percent) { apply_status(target, StatusType::ExtraStunChance, percent); }

// 两个槽共用同一段代码，保留原来的同槽覆盖规则和触发顺序。
void character::delay_attack(character* target, int rounds, int damage, int second) {
    if (!judge_debuff_useful(target)) {
        cout << name << "没能对" << target->name << "施加延迟" << rounds << "回合的延迟伤害!" << endl;
        return;
    }
    target->delayed_damage[second == 0 ? 0 : 1] = {damage, 2 * rounds};
    cout << name << "对" << target->name << "施加了延迟" << rounds << "回合的延迟伤害!" << endl;
}

//吸血函数
void character::suck_blood(character* opponent, int damage_dealt) {
	if (judge_debuff_useful(opponent)) {
		if (statuses.has(StatusType::Wound)) {
			damage_dealt /= 2;
			cout << name << "处于重伤状态，效果变为原先的一半(向下取整)" << endl;
		}
		if (HP + damage_dealt > HPMAX) {
			int cure = HPMAX - HP;
			int over = damage_dealt - cure;
			HP = HPMAX;
			cout << "本次伤害为吸血伤害，恢复了" << cure << "点生命，溢出" << over << "点！" << endl;
		}
		else {
			HP += damage_dealt;
			cout << "本次伤害为吸血伤害，恢复了" << damage_dealt << "点生命！" << endl;
		}
	}
	else cout << name << "的伤害发动吸血效果失败！" << endl;
}
//恢复函数
void character::recover(int recover) {
	if (statuses.has(StatusType::Wound)) {
		recover /= 2;
		cout << name << "处于重伤状态，效果变为原先的一半(向下取整)" << endl;
	}
	if (HP + recover > HPMAX) {
		int cure = HPMAX - HP;
		int over = recover - cure;
		HP = HPMAX;
		cout << name << "恢复了" << cure << "点生命，溢出" << over << "点！" << endl;
	}
	else {
		HP += recover;
		cout << name << "恢复了" << recover << "点生命！" << endl;
	}
}
//获得生命函数
void character::gain_HP(int gain) {
	if (statuses.has(StatusType::Wound)) {
		gain /= 2;
		cout << name << "处于重伤状态，效果变为原先的一半(向下取整)" << endl;
	}
	if (HP + gain > HPMAX) {
		int over = HP + gain - HPMAX;
		HPMAX += over;
		int cure = HPMAX - HP;
		HP = HPMAX;
		cout << name << "提升了" << over << "点生命上限并恢复了" << cure << "点，现有" << HPMAX << "点生命和生命上限！" << endl;
	}
	else {
		HP += gain;
		cout << name << "恢复了" << gain << "点生命！" << endl;
	}
}
// 净化/驱散名单来自 state.cpp，不再逐个检查和复原属性。
void character::dispel_buff(character* target) {
    target->statuses.clear_dispellable(*target, *this);
}
void character::relieve_debuff() {
    statuses.clear_cleansable(*this);
    bool had_delayed_damage = false;
    for (auto& effect : delayed_damage) {
        had_delayed_damage |= effect.damage > 0 || effect.remaining_half_turns > 0;
        effect = {};
    }
    if (had_delayed_damage) cout << name << "成功解除了延迟伤害" << endl;
}
// 冰棱只判断目标是否能接受，不检查施加者的领域和缠绕。
void character::ice_mark(character* target, int layers) {
    if (judge_debuff_useful(target) && !target->statuses.has(StatusType::Freeze)) {
        target->statuses.add(StatusType::IceMark, layers);
        cout << name << "对" << target->name << "施加了" << layers << "层冰棱!" << endl;
    } else if (!target->statuses.has(StatusType::Freeze)) {
        cout << name << "没能对" << target->name << "施加" << layers << "层冰棱!" << endl;
    }
}
void character::exiled(int rounds) {
    statuses.add(StatusType::Exile, rounds);
    cout << name << "成功放逐了自己" << rounds << "回合" << endl;
}
//汲取函数
void character::draw_blood(character* opponent, int blood) {
	if (judge_debuff_useful(opponent)) {
		if (statuses.has(StatusType::Wound)) {
			blood /= 2;
			cout << name << "处于重伤状态，效果变为原先的一半(向下取整)" << endl;
		}
		opponent->HPMAX -= blood;
		HPMAX += blood;
		HP += blood;
		if (opponent->HP > opponent->HPMAX) {
			int damage = opponent->HP - opponent->HPMAX;
			opponent->HP = opponent->HPMAX;
			cout << opponent->name << "的生命值减少" << damage << "点！" << endl;
		}
		cout << name << "汲取了" << opponent->name << blood << "点生命上限，生命上限变为" << HPMAX << "点!" << endl;
		cout << opponent->name << "生命上限变为" << opponent->HPMAX << "点！" << endl;
	}
	else {
		cout << name << "没能汲取" << opponent->name << "的生命上限!" << endl;
	}
}
