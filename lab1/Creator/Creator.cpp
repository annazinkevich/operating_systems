#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<iostream>
#include<string>
#include<fstream>
#include"struct.h"
using namespace std;

int main(int argc, char* argv[]) {

	FILE* file = fopen(argv[1], "w");
	int linesNumber = atoi(argv[2]);
	employee* employees = new employee[linesNumber];
	cout << "Enter employees data: num, name, hours" << endl;
	for (int i = 0;i < linesNumber;i++) {
		cin >> employees[i].num;
		cin >> employees[i].name;
		cin >> employees[i].hours;
		fwrite(&employees[i], sizeof(employee), 1, file);
	}
	fclose(file);

	return 0;
}