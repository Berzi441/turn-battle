#include "character.h"

namespace {
// 两个阶段共用控制处理，避免眩晕/冰冻/沉默分别复制两份。
bool try_stun(character& actor) {
    int chance = actor.statuses.stun_chance();
    if (chance <= 0 || rand() % 100 + 1 > chance) return false;
    actor.statuses.remove(StatusType::Numbness);
    actor.interrupt_channel();
    cout << actor.name << "被晕眩一回合，无法行动" << endl;
    return true;
}

bool settle_freeze(character& actor, bool choosing_skill) {
    if (!actor.statuses.has(StatusType::Freeze)) return false;
    actor.interrupt_channel();
    // 保留原时序：选招时消耗一层，出手前检查会清空本次冰冻。
    if (choosing_skill) {
        actor.statuses.consume(StatusType::Freeze);
    } else {
        actor.statuses.remove(StatusType::Freeze);
    }
    cout << actor.name << "处于冰冻中，无法行动" << endl;
    if (actor.statuses.has(StatusType::Silence)) {
        actor.statuses.consume(StatusType::Silence);
        cout << actor.name << "同时处于冰冻和沉默，冰冻优先；本回合沉默已结算且不生效" << endl;
    }
    if (!actor.statuses.has(StatusType::Freeze))
        cout << actor.name << "解除了冰冻状态" << endl;
    return true;
}

bool settle_silence(character& actor, bool choosing_skill) {
    if (!actor.statuses.has(StatusType::Silence)) return false;
    actor.statuses.consume(StatusType::Silence);
    actor.interrupt_channel();
    cout << actor.name << "处于沉默中，只能使用普通攻击";
    if (choosing_skill) cout << "，跳过技能选择阶段";
    cout << endl;
    return true;
}
}

bool character::judge_choice(character*, int& selected_skill) {
    if (try_stun(*this) || settle_freeze(*this, true)) {
        selected_skill = SkipAction;
        return true;
    }
    if (settle_silence(*this, true)) {
        selected_skill = 0;
        return true;
    }
    if (name == "雷霆之主" && pp[1] == 1 && channel.forced_skill_rounds > 0) {
        selected_skill = 1;
        cout << "雷霆之主雷霆万钧引导中，跳过技能选择阶段" << endl;
        return true;
    }
    return false;
}

void character::judge_use_skill(int selected_skill, character* opponent) {
    if (selected_skill < 0 || selected_skill >= SkillSlots || selected_skill > skill_count) return;
    if (try_stun(*this)) {
        refund_skill(selected_skill);
        return;
    }
    opponent->before_opponent_action(*this);
    if (settle_freeze(*this, false)) {
        refund_skill(selected_skill);
        return;
    }
    if (settle_silence(*this, false)) {
        refund_skill(selected_skill);
        selected_skill = 0;
    }
    use_skill(selected_skill, opponent);
}
