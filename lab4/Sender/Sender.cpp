#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<iostream>
#include<string>
#include<fstream>
#include"C:\Users\Андрей\Source\Repos\ОС_лаб4\ОС_лаб4\Header.h"

HANDLE hSemaphoreRead;
HANDLE hSemaphoreWrite;
HANDLE hSemaphoreMutex;
int size;

void write(char* filename, char* message, char* name) {
	WaitForSingleObject(hSemaphoreWrite, INFINITE);
	WaitForSingleObject(hSemaphoreMutex, INFINITE);

	std::fstream fout(filename, std::ios::binary | std::ios::in | std::ios::out);

	int writePosition;
	char p[10];
	fout.read(p, sizeof(p));
	writePosition = atoi(p);
	std::cout << "Message write position: " << writePosition << std::endl;

	Message* m = new Message(name, message);
	int pos = sizeof(p) + sizeof(Message) * writePosition;
	fout.seekp(pos, std::ios::beg);
	fout.write((char*)m, sizeof(Message));

	writePosition++;
	if (writePosition == size) {
		writePosition = 0;
	}
	_itoa(writePosition, p, 10);
	fout.seekp(0, std::ios::beg);
	fout.write(p, sizeof(p));

	fout.close();

	ReleaseSemaphore(hSemaphoreMutex, 1, NULL);
	ReleaseSemaphore(hSemaphoreRead, 1, NULL);
}

int main(int argc, char*argv[]) {

	hSemaphoreRead = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"Full");
	hSemaphoreWrite = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"Empty");
	hSemaphoreMutex = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, (LPCWSTR)"Mutex");

	char* file = argv[1];
	std::cout << file << "\n";
	size = atoi(argv[2]);
	std::cout << size << "\n";

	char name[10];
	std::cout << "Process name: ";
	std::cin >> name;

	char command;
	while (true) {
		std::cout << "Enter 'w' - write or 'e' - exit:\n";
		std::cin >> command;
		if (command == 'w') {
			std::cout << "Enter a message: ";
			char message[20];
			std::cin >> message;
			write(file, message, name);
		}
		else {
			break;
		}
	}
	return 0;
}