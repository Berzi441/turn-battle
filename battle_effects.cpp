#include "battle_effects.h"
#include "character.h"

void BurnBonusPassive::show(std::ostream& out, const character&, const character* opponent) const {
    out << "被动：对烧伤单位造成伤害+1"
        << (opponent && opponent->statuses.has(StatusType::Burn) ? "(已激活)" : "(未激活)") << endl;
}
int BurnBonusPassive::bonus_damage(const character& target) const {
    return target.statuses.has(StatusType::Burn) ? 1 : 0;
}
void FrostMarkPassive::show(std::ostream& out, const character&, const character*) const {
    out << "【被动·寒霜侵袭】：敌方每次行动前获得1层冰棱；冰棱达到5层时清空，并冰冻敌方1回合" << endl;
}
void FrostMarkPassive::before_opponent_action(character& actor, character& opponent) {
    if (opponent.statuses.has(StatusType::Freeze)) return;
    cout << "(寒霜侵袭)";
    actor.ice_mark(&opponent, 1);
    settle_opponent_marks(actor, opponent);
}
bool FrostMarkPassive::settle_opponent_marks(character& actor, character& opponent) {
    if (opponent.statuses.count(StatusType::IceMark) < 5) return false;
    if (actor.domain) actor.domain->before_freeze(actor, opponent);
    opponent.statuses.remove(StatusType::IceMark);
    opponent.statuses.add(StatusType::Freeze);
    cout << "冰棱达到5层，" << opponent.name << "被冰冻一回合" << endl;
    return true;
}
void TurtleShieldPassive::show(std::ostream& out, const character&, const character*) const {
    out << "被动：护盾破碎后，回合开始时全属性+1"
        << (spent_ ? "(未激活)" : "(已激活)") << endl;
}
void TurtleShieldPassive::after_round(character& actor, character&) {
    if (spent_ || actor.SHD != 0) return;
    cout << "岩盾龟被动已激活，全属性+1" << endl;
    ++actor.ATK;
    ++actor.DEF;
    spent_ = true;
}
void TurtleShieldPassive::show_state(std::ostream& out) const {
    if (spent_) out << "岩盾龟被动已激活 ";
}
void ThunderShieldPassive::show(std::ostream& out, const character&, const character* opponent) const {
    out << "被动：护盾破碎后，对敌方施加1层麻痹"
        << (spent_ ? "(未激活)" : "(已激活)") << endl;
    bool can_numb = opponent && !opponent->statuses.has(StatusType::Numbness);
    out << "被动：若敌方未处于麻痹状态，普攻使敌方受到伤害时可对敌方施加1层麻痹"
        << (can_numb ? "(已激活)" : "(未激活)") << endl;
}
void ThunderShieldPassive::after_action(character& actor, character& opponent) {
    if (spent_ || actor.SHD != 0) return;
    cout << "雷霆之主被动已激活" << endl;
    actor.bring_numbness(&opponent, 1);
    spent_ = true;
}
void ThunderShieldPassive::after_basic_attack(character& actor, character& opponent, int damage) {
    if (damage > 0 && !opponent.statuses.has(StatusType::Numbness))
        actor.bring_numbness(&opponent, 1);
}

bool Domain::effective(const character& actor) const {
    return opened_ && !cancelled_ && !actor.statuses.has(StatusType::Entangle);
}
void Domain::show(std::ostream& out, const character& actor) const {
    out << "【" << name_ << "】：" << description_;
    if (!opened_) out << "(未开启)";
    else out << (effective(actor) ? "(已开启)" : "(已失效)");
    out << endl;
}
bool WindDomain::absolute_priority(const character& actor) const {
    return effective(actor) && actor.SHD > 0;
}
void FrostDomain::before_freeze(character& actor, character& opponent) const {
    if (!effective(actor)) return;
    --opponent.HP;
    cout << opponent.name << "受到了1点真实伤害" << endl;
}
void HolyDomain::after_basic_attack(character& actor, character& opponent) const {
    if (effective(actor)) actor.dispel_buff(&opponent);
}
