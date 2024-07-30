#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_ITEMS 10
#define MAX_ENEMIES 5

typedef struct {
    char name[MAX_NAME_LENGTH];
    int hp;
    int attack;
    int defense;
    int level;
    int exp;
    char inventory[MAX_ITEMS][MAX_NAME_LENGTH];
} Character;

typedef enum {
    ITEM_HEALTH_POTION,
    ITEM_STRENGTH_POTION,
    ITEM_SHIELD,
    ITEM_MAX
} ItemType;

typedef struct {
    ItemType type;
    char name[MAX_NAME_LENGTH];
} Item;

Item items[ITEM_MAX] = {
    { ITEM_HEALTH_POTION, "Health Potion" },
    { ITEM_STRENGTH_POTION, "Strength Potion" },
    { ITEM_SHIELD, "Shield" }
};

void initCharacter(Character* character, const char* name) {
    strncpy(character->name, name, MAX_NAME_LENGTH);
    character->hp = 100;
    character->attack = 10 + (rand() % 5);
    character->defense = 5 + (rand() % 5);
    character->level = 1;
    character->exp = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        strcpy(character->inventory[i], "Empty");
    }
}

void displayCharacter(const Character* character) {
    printf("Name: %s\n", character->name);
    printf("HP: %d\n", character->hp);
    printf("Attack: %d\n", character->attack);
    printf("Defense: %d\n", character->defense);
    printf("Level: %d\n", character->level);
    printf("EXP: %d\n", character->exp);
    printf("Inventory:\n");
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("  %d. %s\n", i + 1, character->inventory[i]);
    }
}

void levelUp(Character* character) {
    character->level++;
    character->hp += 20;
    character->attack += 5;
    character->defense += 3;
    printf("%s leveled up to level %d!\n", character->name, character->level);
}

void useItem(Character* character, ItemType itemType) {
    switch (itemType) {
        case ITEM_HEALTH_POTION:
            character->hp += 50;
            printf("%s used a Health Potion and restored 50 HP!\n", character->name);
            break;
        case ITEM_STRENGTH_POTION:
            character->attack += 10;
            printf("%s used a Strength Potion and gained 10 Attack!\n", character->name);
            break;
        case ITEM_SHIELD:
            character->defense += 5;
            printf("%s used a Shield and gained 5 Defense!\n", character->name);
            break;
        default:
            break;
    }
}

void addItemToInventory(Character* character, ItemType itemType) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (strcmp(character->inventory[i], "Empty") == 0) {
            strncpy(character->inventory[i], items[itemType].name, MAX_NAME_LENGTH);
            printf("%s received: %s\n", character->name, items[itemType].name);
            return;
        }
    }
    printf("Inventory full! Could not add %s.\n", items[itemType].name);
}

Character createEnemy(const char* name, int hp, int attack, int defense) {
    Character enemy;
    strncpy(enemy.name, name, MAX_NAME_LENGTH);
    enemy.hp = hp;
    enemy.attack = attack;
    enemy.defense = defense;
    enemy.level = 1;
    enemy.exp = 0;
    for (int i = 0; i < MAX_ITEMS; i++) {
        strcpy(enemy.inventory[i], "Empty");
    }
    return enemy;
}

void battle(Character* player, Character* enemy) {
    printf("A wild %s appears!\n", enemy->name);
    while (player->hp > 0 && enemy->hp > 0) {
        int damageToEnemy = player->attack - enemy->defense;
        if (damageToEnemy < 0) damageToEnemy = 0;
        enemy->hp -= damageToEnemy;
        printf("%s attacks %s for %d damage!\n", player->name, enemy->name, damageToEnemy);

        if (enemy->hp <= 0) {
            printf("%s defeated %s!\n", player->name, enemy->name);
            player->exp += 10;
            if (player->exp >= player->level * 10) {
                levelUp(player);
            }
            addItemToInventory(player, ITEM_HEALTH_POTION);
            return;
        }

        int damageToPlayer = enemy->attack - player->defense;
        if (damageToPlayer < 0) damageToPlayer = 0;
        player->hp -= damageToPlayer;
        printf("%s attacks %s for %d damage!\n", enemy->name, player->name, damageToPlayer);

        if (player->hp <= 0) {
            printf("%s was defeated by %s...\n", player->name, enemy->name);
            return;
        }
    }
}

void battleMenu(Character* player, Character* enemy) {
    int choice;
    while (player->hp > 0 && enemy->hp > 0) {
        printf("\n1. Attack\n2. Use Item\n3. Run\nChoose an action: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                battle(player, enemy);
                return;
            case 2:
                printf("Choose an item to use:\n");
                for (int i = 0; i < MAX_ITEMS; i++) {
                    printf("%d. %s\n", i + 1, player->inventory[i]);
                }
                int itemChoice;
                scanf("%d", &itemChoice);
                if (itemChoice > 0 && itemChoice <= MAX_ITEMS && strcmp(player->inventory[itemChoice - 1], "Empty") != 0) {
                    for (int i = 0; i < ITEM_MAX; i++) {
                        if (strcmp(player->inventory[itemChoice - 1], items[i].name) == 0) {
                            useItem(player, items[i].type);
                            strcpy(player->inventory[itemChoice - 1], "Empty");
                            break;
                        }
                    }
                } else {
                    printf("Invalid item choice.\n");
                }
                break;
            case 3:
                printf("You fled the battle.\n");
                return;
            default:
                printf("Invalid choice.\n");
                break;
        }
    }
}

void saveGame(const Character* player) {
    FILE* file = fopen("savegame.txt", "w");
    if (file == NULL) {
        printf("Error saving game.\n");
        return;
    }
    fprintf(file, "%s %d %d %d %d %d\n", player->name, player->hp, player->attack, player->defense, player->level, player->exp);
    for (int i = 0; i < MAX_ITEMS; i++) {
        fprintf(file, "%s\n", player->inventory[i]);
    }
    fclose(file);
    printf("Game saved successfully.\n");
}

void loadGame(Character* player) {
    FILE* file = fopen("savegame.txt", "r");
    if (file == NULL) {
        printf("No save game found.\n");
        return;
    }
    fscanf(file, "%s %d %d %d %d %d\n", player->name, &player->hp, &player->attack, &player->defense, &player->level, &player->exp);
    for (int i = 0; i < MAX_ITEMS; i++) {
        fscanf(file, "%s\n", player->inventory[i]);
    }
    fclose(file);
    printf("Game loaded successfully.\n");
}

int main() {
    srand(time(NULL));

    Character player;
    initCharacter(&player, "Hero");

    Character enemies[MAX_ENEMIES];
    enemies[0] = createEnemy("Goblin", 50, 8, 3);
    enemies[1] = createEnemy("Orc", 80, 12, 6);
    enemies[2] = createEnemy("Troll", 120, 15, 10);
    enemies[3] = createEnemy("Dragon", 200, 25, 15);
    enemies[4] = createEnemy("Zombie", 60, 7, 5);

    int choice;
    printf("1. New Game\n2. Load Game\nChoose an option: ");
    scanf("%d", &choice);
    if (choice == 2) {
        loadGame(&player);
    } else {
        initCharacter(&player, "Hero");
        addItemToInventory(&player, ITEM_HEALTH_POTION);
        addItemToInventory(&player, ITEM_STRENGTH_POTION);
        addItemToInventory(&player, ITEM_SHIELD);
    }

    printf("Welcome to the RPG Game!\n");
    displayCharacter(&player);

    int enemyIndex = rand() % MAX_ENEMIES;
    battleMenu(&player, &enemies[enemyIndex]);

    saveGame(&player);

    printf("Game Over!\n");
    displayCharacter(&player);

    return 0;
}

