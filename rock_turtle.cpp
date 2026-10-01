#include "rock_turtle.h"
//落岩术：造成4点伤害，若敌方护甲小于我方则暴击
void rock_turtle::rock_fall(character* opponent) {
	if (defence_value() > opponent->defence_value())
		statuses.add(StatusType::Critical);
	attack(opponent, 4);
	if (statuses.has(StatusType::Critical)) statuses.consume(StatusType::Critical);//攻击具有暴击而不是暴击一回合时
}
//弱体术：造成3点伤害，虚弱敌方1回合
void rock_turtle::weakness_magic(character* opponent) {
	attack(opponent, 3);
	weakness(opponent, 1);
}
void rock_turtle::use_skill(int num, character* opponent) {
	switch (num) {
	case 0:
		cout << name << "对" << opponent->name << "使用了普通攻击" << endl;
		generalattack(opponent);
		break;
	case 1:
		cout << name << "对" << opponent->name << "使用了落岩术" << endl;
		rock_fall(opponent);
		break;
	case 2:
		cout << name << "对" << opponent->name << "使用了弱体术" << endl;
		weakness_magic(opponent);
		break;
	}
}
