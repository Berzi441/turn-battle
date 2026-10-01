#pragma once
#include <iosfwd>

class character;

// 被动持有自己的使用状态；角色只在对应时机通知它。
class Passive {
public:
    virtual ~Passive() = default;
    virtual void show(std::ostream& out, const character& actor, const character* opponent) const = 0;
    virtual void before_opponent_action(character&, character&) {}
    virtual bool settle_opponent_marks(character&, character&) { return false; }
    virtual void after_action(character&, character&) {}
    virtual void after_round(character&, character&) {}
    virtual void after_basic_attack(character&, character&, int) {}
    virtual int bonus_damage(const character&) const { return 0; }
    virtual void show_state(std::ostream&) const {}
};

class BurnBonusPassive final : public Passive {
public:
    void show(std::ostream& out, const character&, const character* opponent) const override;
    int bonus_damage(const character& target) const override;
};
class FrostMarkPassive final : public Passive {
public:
    void show(std::ostream& out, const character&, const character*) const override;
    void before_opponent_action(character& actor, character& opponent) override;
    bool settle_opponent_marks(character& actor, character& opponent) override;
};
class TurtleShieldPassive final : public Passive {
public:
    void show(std::ostream& out, const character&, const character*) const override;
    void after_round(character& actor, character&) override;
    void show_state(std::ostream& out) const override;
    bool spent() const { return spent_; }
private:
    bool spent_ = false;
};
class ThunderShieldPassive final : public Passive {
public:
    void show(std::ostream& out, const character&, const character* opponent) const override;
    void after_action(character& actor, character& opponent) override;
    void after_basic_attack(character& actor, character& opponent, int damage) override;
private:
    bool spent_ = false;
};

// 领域拥有开关与抵消状态。是否生效由当前护盾/缠绕等条件即时计算，
// 不修改角色基础速度，也不需要另存“已加速”标记。
class Domain {
public:
    Domain(const char* name, const char* description) : name_(name), description_(description) {}
    virtual ~Domain() = default;
    void open() { opened_ = true; }
    bool is_open() const { return opened_; }
    bool is_cancelled() const { return cancelled_; }
    void set_cancelled(bool value) { cancelled_ = value; }
    bool effective(const character& actor) const;
    void show(std::ostream& out, const character& actor) const;
    virtual bool absolute_priority(const character&) const { return false; }
    virtual void before_freeze(character&, character&) const {}
    virtual void after_basic_attack(character&, character&) const {}
private:
    const char* name_;
    const char* description_;
    bool opened_ = false;
    bool cancelled_ = false;
};
class WindDomain final : public Domain {
public:
    WindDomain() : Domain("风神领域", "护盾持续期间获得绝对先手") {}
    bool absolute_priority(const character& actor) const override;
};
class FrostDomain final : public Domain {
public:
    FrostDomain() : Domain("极寒领域", "敌方被冰冻前将追加1点真实伤害") {}
    void before_freeze(character& actor, character& opponent) const override;
};
class HolyDomain final : public Domain {
public:
    HolyDomain() : Domain("神圣领域", "普攻驱散敌方正面状态") {}
    void after_basic_attack(character& actor, character& opponent) const override;
};
