#pragma once
#include <array>
#include <cstddef>
#include <iosfwd>

class character;

// 新增普通状态：在这里加类型，并在 state.cpp 的规则表中加一行。
enum class StatusType {
    Strengthen, DivineShield, Weaken, Weakness, ArmorBreak, Wound,
    Numbness, Burn, Frostbite, Critical, Entangle, Silence, Freeze,
    IceMark, NegativeImmunity, ExtraStunChance, Priority, Penetration,
    Exile, Count
};
enum class StatusPhase { Manual, AfterAction, AfterDelayedDamage, RoundEnd };
enum class StatusLifetime {
    Charges,         // 按攻击/行动次数消耗，由对应行为调用 consume。
    HalfTurns,       // 每次行动结算减一次；两个计时单位等于一回合。
    TimedCharges,    // 可以被消耗，也会到期（例如弱化、限时强化）。
    DecayingLayers,  // 每两个计时单位减少一层（麻痹）。
    RoundLayers     // 每回合结束减少一层（烧伤）。
};
// 限时次数状态：施加时刷新为指定时长，强化也可以由 add_timed 单独指定。
enum class StatusTimer { None, OneRound, RoundsPerAmount };
struct StatusRule {
    const char* name;
    StatusLifetime lifetime;
    StatusPhase phase;
    bool cleansable = false;   // 保留原净化技能能移除的状态范围。
    bool dispellable = false;
    int first_tick_grace = 0;  // 抵消新状态施加后紧接着的一次行动结算。
    int attack_modifier = 0;
    int defence_modifier = 0;
    int speed_modifier = 0;
    int periodic_damage = 0;
    bool visible = true;
    StatusTimer timer = StatusTimer::None;

    // 这些函数仅用于填写规则表。具名配置比连续的 true/false/-1 更易读。
    constexpr StatusRule& allow_cleanse() { cleansable = true; return *this; }
    constexpr StatusRule& allow_dispel() { dispellable = true; return *this; }
    constexpr StatusRule& with_grace(int ticks) { first_tick_grace = ticks; return *this; }
    constexpr StatusRule& with_attributes(int attack, int defence, int speed = 0) {
        attack_modifier = attack; defence_modifier = defence; speed_modifier = speed;
        return *this;
    }
    constexpr StatusRule& with_periodic_damage(int damage) { periodic_damage = damage; return *this; }
    constexpr StatusRule& hide() { visible = false; return *this; }
    constexpr StatusRule& with_timer(StatusTimer value) { timer = value; return *this; }
};
const StatusRule& status_rule(StatusType type);

class StatusSet {
public:
    bool has(StatusType type) const;
    int count(StatusType type) const;
    int ticks_left(StatusType type) const;
    int displayed_count(StatusType type) const;
    void add(StatusType type, int amount = 1);
    void add_timed(StatusType type, int amount, int rounds, int first_tick_grace = 0);
    void consume(StatusType type, int amount = 1);
    void remove(StatusType type);
    // 明确设置状态快照（测试/读档）；普通技能使用 add。
    void set(StatusType type, int amount, int remaining_ticks = 0);
    void advance(StatusPhase phase, character& actor);
    void clear_cleansable(character& actor);
    void clear_dispellable(character& target, const character& actor);
    void print(std::ostream& out) const;
    int attack_modifier() const;
    int defence_modifier() const;
    int speed_modifier() const;
    int stun_chance() const;
private:
    struct Entry { int amount = 0; int remaining_ticks = 0; };
    static constexpr std::size_t size = static_cast<std::size_t>(StatusType::Count);
    std::array<Entry, size> entries{};
    Entry& entry(StatusType type);
    const Entry& entry(StatusType type) const;
    int modifier(int StatusRule::*field) const;
};
struct ChannelState {
    int remaining_half_turns = 0;
    int forced_skill_rounds = 0;
    void cancel() { remaining_half_turns = 0; forced_skill_rounds = 0; }
};
struct DelayedDamage {
    int damage = 0;
    int remaining_half_turns = 0;
};
