#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string>
#include "character.h"
#include "judge.h"
#include "battle_order.h"
#include "fire_tiger.h"
#include "ice_lady.h"
#include "jungle_wizard.h"
#include "light_master.h"
#include "ocean_master.h"
#include "rock_turtle.h"
#include "shadow_assassin.h"
#include "wind_dragon.h"
#include "thunder_master.h"

using namespace std;

struct InputEnded {};
template <typename T>
void read_console_value(T& value) {
    while (!(cin >> value)) {
        if (cin.eof() || cin.bad()) throw InputEnded{};
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "输入无效，请重新输入数字：" << endl;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void wait_for_continue(void) {
#ifdef _WIN32
    system("pause");
#else
    cout << "按回车键继续..." << flush;
    string line;
    if (!getline(cin, line)) throw InputEnded{};
#endif
    cout << endl;
}
void show_game_rules(void);

template <typename T>
unique_ptr<character> create_character() {
    return make_unique<T>();
}

struct CharacterOption {
    const char* display_name;
    unique_ptr<character> (*create_character)();
};
const CharacterOption character_options[] = {
    {"风龙领主", create_character<wind_dragon>},
    {"岩盾龟", create_character<rock_turtle>},
    {"炽焰猛虎", create_character<fire_tiger>},
    {"海洋之主", create_character<ocean_master>},
    {"丛林法师", create_character<jungle_wizard>},
    {"雷霆之主", create_character<thunder_master>},
    {"冰天雪女", create_character<ice_lady>},
    {"暗影刺客", create_character<shadow_assassin>},
    {"神圣之主", create_character<light_master>}
};
constexpr int character_option_count = sizeof(character_options) / sizeof(character_options[0]);

void print_character_menu(const int* character_indices, int option_count) {
    for (int menu_index=0;menu_index<option_count;++menu_index)
        cout << menu_index+1 << "->"
             << character_options[character_indices[menu_index]].display_name << endl;
    cout << endl;
}
int read_character_index(const char* prompt, int option_count) {
    int menu_choice = 0;
    cout << prompt << endl;
    read_console_value(menu_choice);
    while (menu_choice < 1 || menu_choice > option_count) {
        cout << "不存在此角色，请重新输入" << endl;
        read_console_value(menu_choice);
    }
    return menu_choice-1;
}
int choose_player_skill(character& acting_character, character& opposing_character) {
    int selected_skill=0;
    acting_character.show(&opposing_character);
    acting_character.show_skill(&opposing_character);
    bool skill_selection_skipped = acting_character.judge_choice(&opposing_character,selected_skill);
    if (!skill_selection_skipped) {
        cout << "请选择技能:" << endl;
        read_console_value(selected_skill);
    } else if (selected_skill == character::SkipAction || selected_skill == 0) {
        // 无法行动或沉默时保留提示暂停；引导续段直接自动释放。
        wait_for_continue();
        if (selected_skill == character::SkipAction) return selected_skill;
    }
    while (!acting_character.select_skill(selected_skill)) read_console_value(selected_skill);
    cout << endl;
    return selected_skill;
}

// 只保留 Boss 各自的选招策略；显示、排序、执行和状态结算由战斗循环处理。
enum class BossKind { RockTurtle = 1, FireTiger = 2, ShadowAssassin = 3 };

struct BossController {
    BossKind boss_kind;
    int current_round = 1;
    int current_choice;
    explicit BossController(BossKind selected_boss_kind)
        : boss_kind(selected_boss_kind),
          current_choice(rand() % (boss_kind == BossKind::ShadowAssassin ? 4 : 2)) {}
    int choose_skill(character& boss, character& player) {
        if (!boss.judge_choice(&player, current_choice)) {
            if (boss_kind == BossKind::RockTurtle) {
                if (current_round == 1 && current_choice == 1) current_choice = 2;
                else if (current_round >= 2) {
                    if (boss.pp[2] > 0) current_choice = 2;
                    else if (boss.pp[1] > 0) current_choice = 1;
                    else current_choice = 0;
                }
            } else if (boss_kind == BossKind::FireTiger) {
                if (current_round == 2) {
                    if (boss.pp[1] > 0) current_choice = 1;
                    else if (boss.pp[2] > 0) current_choice = 2;
                    if (current_choice == 2 && boss.HP == boss.HPMAX) current_choice = 0;
                } else if (current_round >= 3) {
                    if (boss.pp[1] > 0) current_choice = 1;
                    else if (boss.pp[2] > 0) current_choice = 2;
                    else current_choice = 0;
                }
            } else {
                if (current_round == 2) {
                    current_choice = rand() % 4;
                    if (boss.pp[2] == 0 && current_choice == 2) current_choice = 1;
                    else if (boss.pp[1] == 0 && current_choice == 1) current_choice = 2;
                } else if (current_round >= 3) {
                    if (boss.pp[3] == 2) current_choice = 3;
                    else if (boss.pp[2] > 0) current_choice = 2;
                    else if (boss.pp[1] > 0) current_choice = 1;
                    else if (boss.pp[3] == 1) current_choice = 3;
                    else current_choice = 0;
                }
            }
            ++current_round;
        } else {
            wait_for_continue();
            if (current_choice == character::SkipAction) return current_choice;
        }
        if (!boss.select_skill(current_choice)) {
            // 若策略给出不可用技能，用普攻兜底，避免执行未扣次数的技能。
            current_choice = 0;
            boss.select_skill(current_choice);
        }
        return current_choice;
    }
};

// player、opponent 的顺序始终固定；default_first 决定同速时谁先手。
bool execute_battle_round(character& player, character& opponent,
                int player_skill, int opponent_skill, bool default_first) {
    bool player_first = acts_before(player, player_skill, opponent, opponent_skill, default_first);
    character* first_actor = player_first ? &player : &opponent;
    character* second_actor = player_first ? &opponent : &player;
    int first_skill = player_first ? player_skill : opponent_skill;
    int second_skill = player_first ? opponent_skill : player_skill;
    cout << first_actor->name << "获得先手" << endl << endl;
    first_actor->judge_use_skill(first_skill, second_actor);
    cout << endl;
    if (!judge_between_battle(&player, &opponent)) return false;
    cout << endl;
    second_actor->judge_use_skill(second_skill, first_actor);
    cout << endl;
    if (!judge_in_round_over(&player, &opponent)) return false;
    cout << endl;
    return true;
}
void run_battle(character& player, character& opponent, BossController* boss_controller=nullptr,
           bool default_first=true) {
    cout << endl << endl << "战斗开始" << endl;
    character* left_character = default_first ? &player : &opponent;
    character* right_character = default_first ? &opponent : &player;
    cout << "     " << left_character->name << " vs " << right_character->name << "     " << endl << endl;
    while (player.HP > 0 && opponent.HP > 0) {
        if (boss_controller) {
            cout << opponent.name << "装备与技能列表" << endl;
            opponent.show_equipment();
            cout << endl << endl;
            opponent.show_skill(&player);
            cout << endl;
        }
        int player_skill = choose_player_skill(player, opponent);
        int opponent_skill = boss_controller ? boss_controller->choose_skill(opponent,player)
                                 : choose_player_skill(opponent,player);
        if (!execute_battle_round(player,opponent,player_skill,opponent_skill,default_first)) break;
        wait_for_continue();
    }
}

void run_custom_battle(void) {
    cout << "欢迎使用自定义对战系统" << endl << endl;
    cout << " 角色1(先手) vs 角色2(后手)" << endl << endl;
    int available_character_indices[character_option_count];
    for(int character_index=0;character_index<character_option_count;++character_index)
        available_character_indices[character_index]=character_index;
    print_character_menu(available_character_indices,character_option_count);
    cout << "现阶段推荐测试冰天雪女，体验她的护具和领域效果" << endl << endl;
    int first_character_index,second_character_index;
    do {
        first_character_index = read_character_index("请选择角色1(先手)",character_option_count);
        second_character_index = read_character_index("请选择角色2(后手）",character_option_count);
        if(first_character_index==second_character_index) cout << "不能进行镜像对局,请重新输入" << endl;
    } while(first_character_index==second_character_index);
    auto player=character_options[first_character_index].create_character();
    auto opponent=character_options[second_character_index].create_character();
    run_battle(*player,*opponent);
}
void run_boss_battle(void) {
    cout << "人机对战" << endl << endl;
    cout << "请选择一名boss进行讨伐" << endl << endl;
    const int boss_character_indices[]={1,2,7};
    print_character_menu(boss_character_indices,3);
    int boss_menu_choice=0;
    read_console_value(boss_menu_choice);
    while(boss_menu_choice<1 || boss_menu_choice>3) {
        cout << "不存在此boss，请重新输入" << endl;
        read_console_value(boss_menu_choice);
    }
    BossKind boss_kind=static_cast<BossKind>(boss_menu_choice);
    int boss_character_index=boss_character_indices[boss_menu_choice-1];
    auto boss=character_options[boss_character_index].create_character();
    int player_character_indices[character_option_count-1];
    int player_option_count=0;
    for(int character_index=0;character_index<character_option_count;++character_index)
        if(character_index!=boss_character_index)
            player_character_indices[player_option_count++]=character_index;
    cout << "请选择你的角色" << endl << endl;
    print_character_menu(player_character_indices,player_option_count);
    int player_menu_index=read_character_index("请选择角色编号",player_option_count);
    auto player=character_options[player_character_indices[player_menu_index]].create_character();
    bool default_first=rand()%2==0;
    cout << (default_first ? "在本次战斗中，你默认先手！" : "在本次战斗中，你默认后手！") << endl << endl;
    BossController boss_controller(boss_kind);
    run_battle(*player,*boss,&boss_controller,default_first);
}
int main() try {
    srand((unsigned)time(NULL));
    int game_mode=0;
    while (true) {
        cout << "请选择游戏模式" << endl;
        cout << "1->人机对战(推荐)" << endl;
        cout << "2->自定义对战" << endl;
        cout << "3->查看游戏设定（新手必看）" << endl << endl;
        read_console_value(game_mode);
        cout << endl << endl;
        if (game_mode == 1) { run_boss_battle(); break; }
        if (game_mode == 2) { run_custom_battle(); break; }
        if (game_mode == 3) { show_game_rules(); wait_for_continue(); continue; }
        cout << "请重新输入正确数字" << endl;
    }
} catch (const InputEnded&) {
    cout << "输入结束，退出游戏。" << endl;
    return 0;
}

void show_game_rules(void) {
	cout << "角色基础属性：" << endl << endl;
	cout << "全属性：指攻击值与防御值" << endl;
	cout << "攻击：普通攻击可以造成的伤害值，每个角色普通攻击都可以不限次数使用" << endl;
	cout << "防御：可以抵御一些攻击类型的攻击，使敌方伤害降低" << endl;
	cout << "生命：降到0点游戏结束" << endl;
	cout << "护盾：可以抵御除了真实伤害的一切伤害，可以提升自保能力" << endl;
	cout << "免伤：可以抵御除了真实伤害的一切伤害，可以提升自保能力，但免伤触发条件是在即将扣除生命值时" << endl;
	cout << endl;
	cout << "关于领域武器和护具：" << endl << endl;
	cout << "领域：开启领域后获得奇特的领域效果，有些领域还拥有领域特殊印记" << endl;
	cout << "领域技：开启领域后便可使用领域技，缠绕状态下领域技无法使用" << endl;
	cout << "领域印记：有些领域拥有领域印记，叠加印记以获得不同的增益效果，缠绕状态下失效" << endl;
	cout << "领域抵消：场上存在两种不同的领域时，领域效果便会抵消，此时领域效果便会失效，但是可以使用领域技和印记效果" << endl;
	cout << "武器：已装备且有耐久时会提供各种增益(普攻提升，技能伤害提升，防御提升等)，有些武器还拥有被动效果" << endl;
	cout << "武器技：已装备且有耐久的武器能消耗若干耐久来使用武器技" << endl;
	cout << "护具：已装备的护具拥有自己的防御并且可以吸收伤害，还具有被动效果" << endl;
	cout << "增益效果：" << endl << endl;
	cout << "强化：下次造成的伤害提升1点" << endl;
	cout << "穿透：穿透伤害无视防御，但是可以被护盾和免伤削减" << endl;
	cout << "暴击：暴击伤害可以造成先前的1.5倍的伤害" << endl;
	cout << "汲取：获得敌方生命上限，恢复同等生命" << endl;
	cout << "恢复：回复生命值，但无法超过上限" << endl;
	cout << "吸血：恢复等同于敌方受到伤害的生命值，同样无法超过上限" << endl;
	cout << "获得生命：回复生命值，但是可以超过并增加生命上限" << endl;
	cout << "霸体：处于霸体状态下将免疫敌方技能效果" << endl;
	cout << "引导：引导后可以触发更强大的技能效果，但引导状态下将可以被眩晕、冰冻等控制所打断" << endl;
	cout << "放逐：处于放逐状态下将无法受到伤害和被施加负面效果，对其使用的技能仍会消耗使用次数" << endl;
	cout << "先手光环：每拥有1层先手光环，速度增加1点，上限3点" << endl;
	cout << "圣盾：每次受到伤害都将激活圣盾，圣盾可以是伤害变为原先的一半(向下取整)" << endl;
	cout << endl;
	cout << "减益效果：" << endl << endl;
	cout << "弱化：下次造成的伤害降低1点" << endl;
	cout << "虚弱：处于虚弱状态下全属性降低1点" << endl;
	cout << "重伤：处于重伤状态下，吸血、恢复、获得生命和汲取效果变为原先的一半(向下取整)" << endl;
	cout << "烧伤：处于烧伤状态下在回合结束时将会受到1点真实伤害" << endl;
	cout << "冻伤：处于冻伤状态下下次受到的伤害将提升1点" << endl;
	cout << "延迟伤害：处于延迟伤害状态下将在自身回合结束后受到伤害" << endl;
	cout << "破甲：处于破甲状态下防御将变为原先的一半(向上取整)" << endl;
	cout << "缠绕：处于缠绕状态下武器护具和领域将会短暂失效" << endl;
	cout << "眩晕：处于眩晕状态下将无法行动" << endl;
	cout << "冰冻：处于冰冻状态下将无法行动并免疫除了穿透伤害和真实伤害的一切伤害，受到穿透伤害将会解除冰冻状态(碎冰)" << endl;
	cout << "麻痹：处于麻痹状态下时，每层麻痹将降低1点速度并增加20%的眩晕概率，每回合减少1层，若成功触发眩晕后将清空麻痹层数" << endl;
	cout << "沉默：处于沉默状态下时，只能使用普通攻击" << endl;
	cout << endl;
	cout << "一些符号标记:" << endl << endl;
	cout << "持续2回合的技能标志||技能将拥有两段可以释放，且2段技能将跳过选择技能阶段" << endl;
	cout << endl;
	cout << "感谢您游玩本人的游戏" << endl;
	cout << endl;
}
