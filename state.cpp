#include "character.h"
#include <algorithm>

namespace {
using Type = StatusType;
using Life = StatusLifetime;
using Phase = StatusPhase;
// 顺序对应 StatusType。名称、计时、净化/驱散、属性修正都在这里配置。
constexpr StatusRule rules[] = {
    StatusRule{"强化", Life::TimedCharges, Phase::AfterAction}.allow_dispel(),
    StatusRule{"圣盾", Life::Charges, Phase::Manual}.allow_dispel(),
    StatusRule{"弱化", Life::TimedCharges, Phase::AfterAction}
        .allow_cleanse().with_timer(StatusTimer::RoundsPerAmount),
    StatusRule{"虚弱", Life::HalfTurns, Phase::AfterAction}
        .allow_cleanse().with_grace(1).with_attributes(-1, -1),
    StatusRule{"破甲", Life::HalfTurns, Phase::AfterAction}.with_grace(1),
    StatusRule{"重伤", Life::HalfTurns, Phase::AfterAction}.allow_cleanse(),
    StatusRule{"麻痹", Life::DecayingLayers, Phase::AfterAction}
        .allow_cleanse().with_attributes(0, 0, -1),
    StatusRule{"烧伤", Life::RoundLayers, Phase::RoundEnd}
        .allow_cleanse().with_periodic_damage(1),
    StatusRule{"冻伤", Life::Charges, Phase::Manual}.allow_cleanse(),
    {"暴击", Life::Charges, Phase::Manual},
    StatusRule{"缠绕", Life::HalfTurns, Phase::AfterDelayedDamage}
        .allow_cleanse().with_grace(1),
    {"沉默", Life::Charges, Phase::Manual},
    {"冰冻", Life::Charges, Phase::Manual},
    {"冰棱", Life::Charges, Phase::Manual},
    StatusRule{"风罡", Life::Charges, Phase::Manual}.allow_dispel(),
    StatusRule{"额外眩晕概率", Life::TimedCharges, Phase::AfterDelayedDamage}
        .hide().with_timer(StatusTimer::OneRound),
    StatusRule{"先手光环", Life::Charges, Phase::Manual}
        .allow_dispel().with_attributes(0, 0, 1),
    StatusRule{"穿透", Life::Charges, Phase::Manual}.hide(),
    {"放逐", Life::HalfTurns, Phase::AfterAction}
};
static_assert(sizeof(rules) / sizeof(rules[0]) == static_cast<std::size_t>(Type::Count),
              "每种状态都需要一条规则");
}
const StatusRule& status_rule(StatusType type) { return rules[static_cast<std::size_t>(type)]; }
StatusSet::Entry& StatusSet::entry(StatusType type) { return entries[static_cast<std::size_t>(type)]; }
const StatusSet::Entry& StatusSet::entry(StatusType type) const { return entries[static_cast<std::size_t>(type)]; }
bool StatusSet::has(StatusType type) const { return count(type) > 0; }
int StatusSet::count(StatusType type) const { return entry(type).amount; }
int StatusSet::ticks_left(StatusType type) const { return entry(type).remaining_ticks; }
int StatusSet::displayed_count(StatusType type) const {
    if (!has(type)) return 0;
    const auto& rule = status_rule(type);
    if (rule.lifetime == Life::HalfTurns)
        return std::max(1, (ticks_left(type) + 1 - rule.first_tick_grace) / 2);
    return count(type);
}
void StatusSet::add(StatusType type, int amount) {
    if (amount <= 0) return;
    auto& value = entry(type);
    const auto& rule = status_rule(type);
    if (rule.lifetime == Life::HalfTurns) {
        value.remaining_ticks += 2 * amount + (has(type) ? 0 : rule.first_tick_grace);
        value.amount = 1;
    } else {
        value.amount += amount;
        if (rule.lifetime == Life::DecayingLayers) value.remaining_ticks += 2 * amount;
        if (rule.timer == StatusTimer::RoundsPerAmount) value.remaining_ticks = 2 * amount;
        if (rule.timer == StatusTimer::OneRound) value.remaining_ticks = 2;
    }
}
void StatusSet::add_timed(StatusType type, int amount, int rounds, int first_tick_grace) {
    if (amount <= 0 || rounds <= 0) return;
    add(type, amount);
    entry(type).remaining_ticks = 2 * rounds + first_tick_grace;
}
void StatusSet::consume(StatusType type, int amount) {
    if (amount > 0) entry(type).amount = std::max(0, count(type) - amount);
}
void StatusSet::remove(StatusType type) { entry(type) = {}; }
void StatusSet::set(StatusType type, int amount, int remaining_ticks) {
    entry(type) = {std::max(0, amount), std::max(0, remaining_ticks)};
}
void StatusSet::advance(StatusPhase phase, character& actor) {
    for (std::size_t i = 0; i < size; ++i) {
        const auto type = static_cast<Type>(i);
        const auto& rule = rules[i];
        if (rule.phase != phase || phase == Phase::Manual) continue;
        auto& value = entries[i];
        bool was_active = has(type);
        if (was_active && rule.periodic_damage > 0) {
            actor.HP -= rule.periodic_damage;
            cout << actor.name << "处于" << rule.name << "状态，受到"
                 << rule.periodic_damage << "点真实伤害!" << endl;
        }
        if (rule.lifetime == Life::RoundLayers) {
            consume(type);
        } else if (value.remaining_ticks > 0) {
            if (rule.lifetime == Life::DecayingLayers && value.remaining_ticks % 2 == 1)
                consume(type);
            --value.remaining_ticks;
            if (value.remaining_ticks == 0) value.amount = 0;
        }
        if (was_active && !has(type)) cout << actor.name << "解除了" << rule.name << "状态" << endl;
    }
}
void StatusSet::clear_cleansable(character& actor) {
    for (std::size_t i = 0; i < size; ++i) {
        const auto type = static_cast<Type>(i);
        if (!rules[i].cleansable) continue;
        if (has(type)) cout << actor.name << "成功解除了" << rules[i].name << "状态" << endl;
        remove(type); // 同时清除层数和计时，不留下一回合再次触发的标记。
    }
}
void StatusSet::clear_dispellable(character& target, const character& actor) {
    for (std::size_t i = 0; i < size; ++i) {
        const auto type = static_cast<Type>(i);
        if (!rules[i].dispellable) continue;
        if (has(type)) cout << actor.name << "成功驱散了" << target.name << "的" << rules[i].name << "状态！" << endl;
        remove(type);
    }
}
void StatusSet::print(std::ostream& out) const {
    for (std::size_t i = 0; i < size; ++i) {
        const auto type = static_cast<Type>(i);
        if (rules[i].visible && has(type)) out << rules[i].name << displayed_count(type) << "层 ";
    }
    if (stun_chance() > 0) out << "眩晕概率" << stun_chance() << "% ";
}
int StatusSet::modifier(int StatusRule::*field) const {
    int total = 0;
    for (std::size_t i = 0; i < size; ++i) total += entries[i].amount * (rules[i].*field);
    return total;
}
int StatusSet::attack_modifier() const { return modifier(&StatusRule::attack_modifier); }
int StatusSet::defence_modifier() const { return modifier(&StatusRule::defence_modifier); }
int StatusSet::speed_modifier() const { return modifier(&StatusRule::speed_modifier); }
int StatusSet::stun_chance() const { return 20 * count(Type::Numbness) + count(Type::ExtraStunChance); }
