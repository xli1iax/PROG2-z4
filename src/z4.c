#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h" // NEMENIT, inak vas kod neprejde testom !!!

// chybove hlasenia
#define ERR_UNIT_COUNT "ERR_UNIT_COUNT"
#define ERR_ITEM_COUNT "ERR_ITEM_COUNT"
#define ERR_WRONG_ITEM "ERR_WRONG_ITEM"
#define ERR_SLOTS "ERR_SLOTS"
#define MAX(a,b) ((a) > (b) ? (a) : (b))

const ITEM* find_item_by_name(const char *name) {
    for (int i = 0; i < NUMBER_OF_ITEMS; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return &items[i];
        }
    }
    return NULL;
}

int load_army(UNIT *army, int numberOfPlayers) {
    for (int i = 0; i < numberOfPlayers; i++) {
        char line[256];
        fgets(line, sizeof(line), stdin);

        line[strcspn(line, "\n")] = '\0'; // видалити символ нового рядка

        char *token = strtok(line, " ");


        strncpy(army[i].name, token, MAX_NAME);
        army[i].hp = 100;
        army[i].item1 = NULL;
        army[i].item2 = NULL;

        int itemCount = 0;
        int totalSlots = 0;

        while ((token = strtok(NULL, " ")) != NULL) {
            const ITEM *found = find_item_by_name(token);
            if (!found) {
                printf("ERR_WRONG_ITEM: %s\n", token);
                return 1;
            }

            if (itemCount == 0) {
                army[i].item1 = found;
            } else if (itemCount == 1) {
                army[i].item2 = found;
            } else {
                printf("ERR_ITEM_COUNT\n");
                return 1;
            }

            totalSlots += found->slots;
            itemCount++;
        }

        if (totalSlots > 2) {
            printf("ERR_SLOTS\n");
            return 1;
        }
    }

    return 0;
}

void print_army(UNIT army[], int count, const char *label)
{
    printf("%s\n", label);
    for (int i = 0; i < count; i++)
    {
        printf("    Unit: %d\n", i);
        printf("    Name: %s\n", army[i].name);
        printf("    HP: %d\n", army[i].hp);
        if (army[i].item1) {
            printf("    Item 1: %s,%d,%d,%d,%d,%d\n", army[i].item1->name, army[i].item1->att,
                army[i].item1->def, army[i].item1->slots, army[i].item1->range, army[i].item1->radius);
        }if (army[i].item2) {
            printf("    Item 2: %s,%d,%d,%d,%d,%d\n", army[i].item2->name, army[i].item2->att,
                army[i].item2->def, army[i].item2->slots, army[i].item2->range, army[i].item2->radius);
        }
        printf("\n");
    }
}

void print_units(UNIT army[], int count, int armyNum)
{
    printf("%d:", armyNum);
    for (int i = 0; i < count; i++)
    {
        printf(" %s,%d", army[i].name, army[i].hp);
    }
    printf("\n");
}

UNIT* army1_attack(UNIT army1[], UNIT army2[], int players1, int players2)
{
    UNIT *army2Damage = malloc(players2 * sizeof(UNIT));
    if (!army2Damage) {
        return NULL;
    }

    for (int i = 0; i < players2; ++i)
    {
        strcpy(army2Damage[i].name, army2[i].name);
        army2Damage[i].hp = 0;
    }

    for (int i = 0; i < players1; i++)
    {
        if (army1[i].item1 && army1[i].item1->range >= i)
        {
            char left[256];
            snprintf(left, sizeof(left), "1,%s,%s:", army1[i].name, army1[i].item1->name);
            printf("%-20s", left);

            for (int j = 0; j <= army1[i].item1->radius; j++)
            {
                if (j >= players2 ) break;
                int defEnemy = army2[j].item1->def + (army2[j].item2 ? army2[j].item2->def : 0);
                int damage = MAX(1, army1[i].item1->att - defEnemy);

                army2Damage[j].hp += damage;
                printf(" [%s,%d]", army2[j].name, damage);
            }
            printf("\n");
        }

        if (army1[i].item2 && army1[i].item2->range >= i)
        {
            char left[256];
            snprintf(left, sizeof(left), "1,%s,%s:", army1[i].name, army1[i].item2->name);
            printf("%-20s", left);
            for (int j = 0; j <= army1[i].item2->radius; ++j)
            {
                if (j >= players2 ) break;
                int defEnemy = army2[j].item1->def + (army2[j].item2 ? army2[j].item2->def : 0);
                int damage = MAX(1, army1[i].item2->att - defEnemy);

                army2Damage[j].hp += damage;
                printf(" [%s,%d]", army2[j].name, damage);
            }

            printf("\n");
        }
    }

    return army2Damage;
}

UNIT* army2_attack(UNIT army1[], UNIT army2[], int players1, int players2)
{
    UNIT *army1Damage = malloc(players1 * sizeof(UNIT));
    if (!army1Damage) {
        return NULL;
    }

    for (int i = 0; i < players1; ++i)
    {
        strcpy(army1Damage[i].name, army1[i].name);
        army1Damage[i].hp = 0;
    }

    for (int i = 0; i < players2; i++)
    {
        if (army2[i].item1 && army2[i].item1->range >= i)
        {
            char left[256];
            snprintf(left, sizeof(left), "2,%s,%s:", army2[i].name, army2[i].item1->name);
            printf("%-20s", left);
            for (int j = 0; j <= army2[i].item1->radius; j++)
            {
                if (j >= players1) break;
                int defEnemy = army1[j].item1->def + (army1[j].item2 ? army1[j].item2->def : 0);
                int damage = MAX(1, army2[i].item1->att - defEnemy);

                army1Damage[j].hp += damage;
                printf(" [%s,%d]", army1[j].name, damage);
            }
            printf("\n");
        }

        if (army2[i].item2 && army2[i].item2->range >= i)
        {
            char left[256];
            snprintf(left, sizeof(left), "2,%s,%s:", army2[i].name, army2[i].item2->name);
            printf("%-20s", left);

            for (int j = 0; j <= army2[i].item2->radius; ++j)
            {
                if (j >= players1) break;
                int defEnemy = army1[j].item1->def + (army1[j].item2 ? army1[j].item2->def : 0);
                int damage = MAX(1, army2[i].item2->att - defEnemy);

                army1Damage[j].hp += damage;
                printf(" [%s,%d]", army1[j].name, damage);
            }
            printf("\n");
        }
    }

    return army1Damage;
}

void update_army(UNIT* damagedArmy, UNIT* mainArmy, int *players)
{
    for (int i = 0; i < *players; ++i)
    {
        mainArmy[i].hp -= damagedArmy[i].hp;
    }

    int aliveIndex = 0;
    for (int i = 0; i < *players; ++i)
    {
        if (mainArmy[i].hp > 0)
        {
            mainArmy[aliveIndex++] = mainArmy[i]; // оставляем живого
        }
    }

    *players = aliveIndex; // обновляем количество бойцов
}


int main(int argc, char *argv[]){
    int N = 0;      // по умолчанию "бесконечность"
    int useN = 0;         // флаг: использовать ли N

    if (argc > 1) {
        N = atoi(argv[1]); // преобразуем аргумент в число
        useN = 1;          // флаг: теперь N имеет значение
    }

    int numberOfPlayers1 = 0;
    scanf("%d", &numberOfPlayers1);
    getchar(); // убрать '\n' после числа

    if (numberOfPlayers1 < MIN_ARMY || numberOfPlayers1 > MAX_ARMY) {
        printf("ERR_UNIT_COUNT\n");
        return 1;
    }

    UNIT army1[numberOfPlayers1];
    if (load_army(army1, numberOfPlayers1) != 0) return 1;

    int numberOfPlayers2 = 0;

    scanf("%d", &numberOfPlayers2);
    getchar();

    if (numberOfPlayers2 < MIN_ARMY || numberOfPlayers2 > MAX_ARMY) {
        printf("ERR_UNIT_COUNT\n");
        return 1;
    }

    UNIT army2[numberOfPlayers2];

    if (load_army(army2, numberOfPlayers2) != 0) return 1;

    print_army(army1, numberOfPlayers1, "ARMY 1");
    print_army(army2, numberOfPlayers2, "ARMY 2");

    int isGameOver = 0;
    int round = 1;
    while (!isGameOver)
    {
        printf("Round %d\n", round);
        print_units(army1,numberOfPlayers1,1);
        print_units(army2,numberOfPlayers2,2);

        UNIT* damagedArmy2 = army1_attack(army1, army2, numberOfPlayers1, numberOfPlayers2);
        UNIT* damagedArmy1 = army2_attack(army1, army2, numberOfPlayers1, numberOfPlayers2);

        update_army(damagedArmy1, army1, &numberOfPlayers1);
        update_army(damagedArmy2, army2, &numberOfPlayers2);

        print_units(army1,numberOfPlayers1,1);
        print_units(army2,numberOfPlayers2,2);
        printf("\n");
        if (numberOfPlayers1==0 && numberOfPlayers2==0)
        {
            printf("NO WINNER\n");
            isGameOver = 1;
        }

        if (numberOfPlayers1 == 0)
        {
            printf("WINNER: 2\n");
            isGameOver = 1;
        }

        if (numberOfPlayers2 == 0)
        {
            printf("WINNER: 1\n");
            isGameOver = 1;
        }
        if (N == 1 && useN) break;
        if (N > 0 && useN) N--;
        round++;
        free(damagedArmy1);
        free(damagedArmy2);
    }

    return 0;
}
