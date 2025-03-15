#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_POKEMON 100
#define MAX_NAME_LENGTH 30
#define MAX_TYPE_LENGTH 10
#define MAX_USER_POKEMON 6
#define EXP_THRESHOLD 100  // 레벨업 경험치 기준

typedef struct {
    char name[MAX_NAME_LENGTH];
    char type[MAX_TYPE_LENGTH];
    int min_hp;
    int min_atk;
} PokemonInfo;

typedef struct {
    char name[MAX_NAME_LENGTH];
    char type[MAX_TYPE_LENGTH];
    int attack;
    int max_hp;
    int now_hp;
    int level;
    int exp;
} Pokemon;

PokemonInfo pokemon_list[MAX_POKEMON];
Pokemon user[MAX_USER_POKEMON];

int user_money = 10000;
int user_bag[2] = {3, 1}; // 몬스터볼, 회복물약

int load_pokemon_list();
void select_initial_pokemon();
void battle(Pokemon *player, Pokemon *enemy);
void level_up(Pokemon *p);
void shopping();
void save_game();
int load_game();
void display_pokemon_status(Pokemon *p);
int type_synergy(char *t1, char *t2);
void cleanup();
void random_pokemon(Pokemon *enemy);
int random_range(int min, int max);
int percent_chance(int probability);

int main() {
    srand(time(NULL));
    int pokemon_count = load_pokemon_list();
    
    if (pokemon_count == -1) return 0;

    printf("포켓몬스터\n");
    printf("press enter key to start\n");
    getchar();

    if (load_game() == -1) {
        select_initial_pokemon();
    }

    while (1) {
        int choice;
        printf("\n여행을 떠나시겠습니까?\n");
        printf("1. 네  2. 저장하고 종료  3. 상점가기\n>> ");
        scanf("%d", &choice);

        if (choice == 1) {
            Pokemon enemy;
            random_pokemon(&enemy);
            printf("\n야생의 %s이(가) 나타났다!\n", enemy.name);
            battle(&user[0], &enemy);
        } 
        else if (choice == 2) {
            save_game();
            printf("게임이 저장되었습니다. 종료합니다.\n");
            cleanup();
            return 0;
        } 
        else if (choice == 3) {
            shopping();
        }
        else {
            printf("올바른 값을 입력해주세요.\n");
        }
    }

    return 0;
}

int load_pokemon_list() {
    FILE *file = fopen("pokemon_list.txt", "r");
    if (!file) {
        printf("pokemon_list.txt 파일을 열 수 없습니다.\n");
        return -1;
    }

    int count;
    fscanf(file, "%d", &count);

    for (int i = 0; i < count; i++) {
        fscanf(file, " %[^|] | %[^|] | %d | %d", 
               pokemon_list[i].name, 
               pokemon_list[i].type, 
               &pokemon_list[i].min_hp, 
               &pokemon_list[i].min_atk);
    }

    fclose(file);
    return count;
}

void select_initial_pokemon() {
    int choice;
    printf("\n어느 포켓몬을 고르시겠습니까?\n");
    printf("1. 파이리 2. 이상해씨 3. 꼬부기\n>> ");
    
    scanf("%d", &choice);
    choice--;

    strcpy(user[0].name, pokemon_list[choice].name);
    strcpy(user[0].type, pokemon_list[choice].type);
    user[0].attack = random_range(100, 150);
    user[0].max_hp = user[0].now_hp = random_range(500, 1000);
    user[0].level = 1;
    user[0].exp = 0;
}

void battle(Pokemon *player, Pokemon *enemy) {
    while (player->now_hp > 0 && enemy->now_hp > 0) {
        int choice;
        printf("\n1. 공격 2. 도망치기\n>> ");
        scanf("%d", &choice);

        if (choice == 1) {
            int synergy = type_synergy(player->type, enemy->type);
            int damage = (synergy == 1) ? player->attack * 1.5 : (synergy == -1) ? player->attack * 0.5 : player->attack;
            enemy->now_hp -= damage;

            printf("%s가(이) %s에게 %d의 데미지를 입혔다!\n", player->name, enemy->name, damage);
            if (enemy->now_hp <= 0) {
                printf("%s를 쓰러트렸다!\n", enemy->name);
                player->exp += 50;
                if (player->exp >= EXP_THRESHOLD) {
                    level_up(player);
                }
                return;
            }

            int enemy_damage = enemy->attack * random_range(1, 2);
            player->now_hp -= enemy_damage;
            printf("적 %s가(이) %s에게 %d의 피해를 입혔다!\n", enemy->name, player->name, enemy_damage);
        }
        else if (choice == 2) {
            if (percent_chance(50)) {
                printf("도망쳤습니다!\n");
                return;
            } else {
                printf("도망치기에 실패했습니다!\n");
            }
        } else {
            printf("잘못된 입력입니다.\n");
        }
    }

    if (player->now_hp <= 0) {
        printf("%s가 쓰러졌습니다!\n", player->name);
        player->now_hp = player->max_hp;
    }
}

void level_up(Pokemon *p) {
    p->level++;
    p->attack += 10;
    p->max_hp += 50;
    p->now_hp = p->max_hp;
    p->exp = 0;
    printf("%s가 레벨업 했습니다! 현재 레벨: %d\n", p->name, p->level);
}

void shopping() {
    int choice;
    printf("\n상점에 오신 것을 환영합니다!\n");
    printf("현재 소지금: %d\n", user_money);
    printf("1. 몬스터볼 (1000원) 2. 회복물약 (2500원) 3. 나가기\n>> ");
    
    scanf("%d", &choice);
    if (choice == 1 && user_money >= 1000) {
        user_money -= 1000;
        user_bag[0]++;
        printf("몬스터볼을 구매하였습니다.\n");
    }
    else if (choice == 2 && user_money >= 2500) {
        user_money -= 2500;
        user_bag[1]++;
        printf("회복물약을 구매하였습니다.\n");
    }
    else {
        printf("돈이 부족하거나 잘못된 입력입니다.\n");
    }
}

void save_game() {
    FILE *file = fopen("save.txt", "w");
    fprintf(file, "%s %s %d %d %d %d %d\n", 
            user[0].name, user[0].type, user[0].attack, 
            user[0].max_hp, user[0].now_hp, user[0].level, user[0].exp);
    fprintf(file, "%d %d\n", user_money, user_bag[0]);
    fclose(file);
}

int random_range(int min, int max) { return rand() % (max - min + 1) + min; }
int percent_chance(int probability) { return rand() % 100 < probability; }
