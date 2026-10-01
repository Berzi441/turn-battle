#pragma once
#include"character.h"
//判定领域是否同时失效
void judge_flag_useful(character* player, character* opponent);
//半个回合判定
bool judge_between_battle(character* player, character* opponent);
//回合结束判定
bool judge_in_round_over(character * player, character * opponent); 
