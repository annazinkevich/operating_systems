#pragma once
char READ = 'r';
char MODIFY = 'm';
char ORDER_NOT_FOUND = 'n';
char ORDER_FOUND = 'f';
char EXIT = 'e';
char END_OPERATION = 'd';
char END_MODIFY = 'n';
char filename[20];

CRITICAL_SECTION cs;
HANDLE semaphore;

struct Order {
	int n;
	char name[10];
	int amount;
	double price;
};