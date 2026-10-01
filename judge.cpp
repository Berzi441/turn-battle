#include "judge.h"

namespace {
bool battle_continues(const character& player, const character& opponent) {
    if (player.HP > 0 && opponent.HP > 0) return true;
    if (player.HP <= 0 && opponent.HP <= 0) cout << "平局！";
    else cout << (player.HP > 0 ? player.name : opponent.name) << "获得胜利！" << endl;
    return false;
}
void advance_channel(character& actor) {
    if (actor.channel.remaining_half_turns <= 0) return;
    if (actor.channel.remaining_half_turns == 1) actor.on_channel_complete();
    --actor.channel.remaining_half_turns;
}
void advance_delayed_damage(character& target, character& actor, std::size_t slot) {
    auto& effect = target.delayed_damage[slot];
    if (effect.remaining_half_turns <= 0) return;
    if (--effect.remaining_half_turns == 0) {
        cout << actor.name << "触发了延迟伤害" << endl;
        actor.attack(&target, effect.damage);
        effect = {};
    }
}
void advance_statuses(character& player, character& opponent, StatusPhase phase) {
    player.statuses.advance(phase, player);
    opponent.statuses.advance(phase, opponent);
}
}

void judge_flag_useful(character* player, character* opponent) {
    bool both_open = player->domain && opponent->domain &&
        player->domain->is_open() && opponent->domain->is_open();
    bool was_cancelled = (player->domain && player->domain->is_cancelled()) ||
        (opponent->domain && opponent->domain->is_cancelled());
    if (both_open && !was_cancelled) {
        player->domain->set_cancelled(true);
        opponent->domain->set_cancelled(true);
        cout << "场上具有两种领域，领域效果互相抵消" << endl;
    } else if (!both_open && was_cancelled) {
        if (player->domain) player->domain->set_cancelled(false);
        if (opponent->domain) opponent->domain->set_cancelled(false);
        cout << "场上领域恢复至一种，领域效果恢复正常" << endl;
    }
}

// 每次行动后结算一次。player/opponent 始终保持玩家/对手顺序，与实际先后手无关。
bool judge_between_battle(character* player, character* opponent) {
    if (!battle_continues(*player, *opponent)) return false;
    judge_flag_useful(player, opponent);

    // 先让双方的普通计时状态递减，再触发被动；新施加的麻痹不会立即递减。
    advance_statuses(*player, *opponent, StatusPhase::AfterAction);
    player->after_action_settlement(*opponent);
    opponent->after_action_settlement(*player);
    advance_channel(*player);
    advance_channel(*opponent);

    // 保留原顺序：第1段双方 → 第2段双方。
    for (std::size_t slot = 0; slot < player->delayed_damage.size(); ++slot) {
        advance_delayed_damage(*player, *opponent, slot);
        advance_delayed_damage(*opponent, *player, slot);
    }
    // 缠绕在延迟伤害之后过期，不能与上面的普通状态合并结算。
    advance_statuses(*player, *opponent, StatusPhase::AfterDelayedDamage);
    if (opponent->settle_opponent_marks(*player)) player->interrupt_channel();
    if (player->settle_opponent_marks(*opponent)) opponent->interrupt_channel();
    return battle_continues(*player, *opponent);
}

bool judge_in_round_over(character* player, character* opponent) {
    advance_statuses(*player, *opponent, StatusPhase::RoundEnd);
    for (character* actor : {player, opponent}) {
        if (actor->channel.forced_skill_rounds > 0) --actor->channel.forced_skill_rounds;
    }
    if (!judge_between_battle(player, opponent)) return false;
    player->after_round_settlement(*opponent);
    opponent->after_round_settlement(*player);
    return true;
}
