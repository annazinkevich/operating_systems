#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<iostream>
#include<string>
#include<fstream>
#include"struct.h"
using namespace std;

int main(int argc, char* argv[]) {

	FILE* input = fopen(argv[1], "r");
	ofstream out(argv[2]);
	string str = argv[1];
	str = "Otchet with file '' " + str + "''\n";
	out << str;
	employee empl;
	int linesNumber = atoi(argv[3]);
	int hourPayment = atoi(argv[4]);
	for (int i = 0;i < linesNumber;i++) {
		str = "";
		fread(&empl, sizeof(employee), 1, input);
		str += to_string(empl.num);
		str += " ";
		str += empl.name;
		str += " ";
		str += to_string(empl.hours);
		str += " ";
		str += to_string(hourPayment * empl.hours);
		str = str + "\n";
		out << str;
	}

	fclose(input);
	out.close();

	return 0;
}