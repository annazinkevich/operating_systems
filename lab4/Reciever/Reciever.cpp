#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<iostream>
#include<string>
#include<fstream>
#include"Header.h"

HANDLE hSemaphoreRead;
HANDLE hSemaphoreWrite;
HANDLE hSemaphoreMutex;
int size;
int readPosition = 0;

void createBinaryFile(char* filename, int size) {
	std::ofstream fout(filename, std::ios::binary);
	int position = 0;
	char p[10];
	_itoa(position, p, 10);
	fout.write(p, sizeof(p));
	Message* m = new Message("empty", "empty");
	for (int i = 0; i < size; i++) {
		fout.write((char*)&m, sizeof(Message));
	}
	fout.close();
}

void read(char* file) {
	WaitForSingleObject(hSemaphoreRead, INFINITE);
	WaitForSingleObject(hSemaphoreMutex, INFINITE);
	std::cout << "Message read position: " << readPosition << std::endl;

	std::fstream fin(file, std::ios::binary | std::ios::in | std::ios::out);
	if (!fin.is_open()) {
		std::cout << "error\n";
		return;
	}

	Message* m = new Message();
	char writeIter[10];
	int pos = sizeof(writeIter) + sizeof(Message) * readPosition;

	fin.seekg(pos, std::ios::beg);
	fin.read((char*)m, sizeof(Message));
	std::cout << m->name << " - " << m->text << std::endl;

	fin.seekp(pos, std::ios::beg);
	m = new Message("deleted", "deleted");
	fin.write((char*)m, sizeof(Message));

	readPosition++;
	if (readPosition == size) {
		readPosition = 0;
	}
	fin.close();

	ReleaseSemaphore(hSemaphoreMutex, 1,NULL);
	ReleaseSemaphore(hSemaphoreWrite, 1, NULL);
}

int main() {

	std::cout << "Filename: ";
	char file[30];
	std::cin >> file;

	std::cout << "Amount of messages: ";
	std::cin >> size;

	createBinaryFile(file, size);

	std::cout << "Amount of sender processes: ";
	int n;
	std::cin >> n;

	hSemaphoreRead = CreateSemaphore(NULL, 0, size, (LPCWSTR)"Full");
	if (hSemaphoreRead == NULL) {
		return GetLastError();
	}
	hSemaphoreWrite = CreateSemaphore(NULL, size, size, (LPCWSTR)"Empty");
	if (hSemaphoreWrite == NULL) {
		return GetLastError();
	}
	hSemaphoreMutex = CreateSemaphore(NULL, 1, 1, (LPCWSTR)"Mutex");
	if (hSemaphoreMutex == NULL) {
		return GetLastError();
	}

	char line[50] = "Sender ";
	strcat(line, file);
	strcat(line, " ");
	char num[10];
	_itoa(size, num, 10);
	strcat(line, num);
	wchar_t wtext[100];
	mbstowcs(wtext, line, strlen(line) + 1);
	LPWSTR ptr = wtext;

	STARTUPINFO* siSender=new STARTUPINFO[n];
	PROCESS_INFORMATION* piSender=new PROCESS_INFORMATION[n];

	for (int i = 0; i<n; i++) {
		ZeroMemory(&siSender[i], sizeof(STARTUPINFO));
		siSender[i].cb = sizeof(STARTUPINFO);
		if (!CreateProcess(NULL, ptr, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &siSender[i], &piSender[i])) {
			return GetLastError();
		}
    }

	char message;
	while (true) {
		std::cout << "enter 'r' - read or 'e' - exit\n";
		std::cin >> message;
		if (message == 'r') {
			std::cout << "The message is: \n";
			read(file);
		}
		else {
			break;
		}
	}

	CloseHandle(hSemaphoreRead);
	CloseHandle(hSemaphoreWrite);
	CloseHandle(hSemaphoreMutex);

	return 0;
}