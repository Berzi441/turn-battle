#pragma once
#include "character.h"

// 绝对先手优先于数值速度；双方都有或都没有时仍比较技能速度和默认先手。
inline bool acts_before(const character& first, int first_skill,
                        const character& second, int second_skill, bool default_first) {
    if (first.absolute_priority() != second.absolute_priority())
        return first.absolute_priority();
    int first_speed = first.action_speed(first_skill);
    int second_speed = second.action_speed(second_skill);
    return first_speed > second_speed || (default_first && first_speed == second_speed);
}
