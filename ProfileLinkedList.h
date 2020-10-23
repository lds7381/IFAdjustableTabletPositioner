#include "driverlib/gpio.h"

struct StudentProfile;
struct node;

void printList();
void insertFirst(uint8_t key, struct StudentProfile profile);
struct node* deleteFirst();
bool isEmpty();
int length();
struct node* find(uint8_t key);
struct node* delete(uint8_t key);
