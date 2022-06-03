#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>
#include"Header.h"

volatile int readerCount = 0;

Order* findOrder(char* filename, int orderNumber) {
	std::ifstream in(filename, std::ios::binary);
	while (in.peek() != EOF)
	{
		Order* order = new Order;
		in.read((char*)order, sizeof(Order));
		if (order->n == orderNumber) {
			in.close();
			return order;
		}
	}
	in.close();
	return nullptr;
}

void modify(Order order) {
	std::fstream f(filename, std::ios::binary | std::ios::in | std::ios::out);
	int pos = 0;
	int orderSize = sizeof(Order);
	while (f.peek() != EOF)
	{
		Order o;
		f.read((char*)&o, sizeof(Order));
		if (order.n == o.n) {
			f.seekp(pos * orderSize, std::ios::beg);
			f.write((char*)&order, sizeof(Order));
			f.close();
			return;
		}
		else {
			pos++;
		}
	}
}

DWORD WINAPI client(LPVOID data)
{
	HANDLE writePipe;
	HANDLE readPipe;
	HANDLE clientReadPipe;
	HANDLE clientWritePipe;

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&readPipe, &clientWritePipe, &sa, 0)) {
		return GetLastError();
	}
	if (!CreatePipe(&clientReadPipe, &writePipe, &sa, 0)) {
		return GetLastError();
	}

	char comLine[333];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	sprintf(comLine, "Client %d %d", (int)clientWritePipe, (int)clientReadPipe);
	char* other_data = new char[333];
	strcpy(other_data, comLine);
	wchar_t wtext[333];
	mbstowcs(wtext, other_data, strlen(other_data) + 1);
	LPWSTR ptr = wtext;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	if (!CreateProcess(NULL, ptr, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		return GetLastError();
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	while (true) {
		DWORD bytesRead;
		DWORD bytesWrite;
		char clientWant;

		if(!ReadFile(readPipe, &clientWant, sizeof(clientWant), &bytesRead, NULL)) {
			std::cout << "Faild to read";
			return GetLastError();
		}

		if (clientWant == READ) {
			EnterCriticalSection(&cs);
			++readerCount;

			if (readerCount == 1) {
				WaitForSingleObject(semaphore, INFINITE);
			}
			LeaveCriticalSection(&cs);

			int orderNumber;
			if(!ReadFile(readPipe, &orderNumber, sizeof(int), &bytesRead, NULL)) {
				std::cout << "Faild to read";
				return GetLastError();
			}

			Order* order = findOrder(filename, orderNumber);
			if (order != nullptr) {
				if(!WriteFile(writePipe, &ORDER_FOUND, sizeof(ORDER_FOUND), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}
				if(!WriteFile(writePipe, order, sizeof(Order), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}
			}
			else {
				if(!WriteFile(writePipe, &ORDER_NOT_FOUND, sizeof(ORDER_NOT_FOUND), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}
			}

			char end;
			if(!ReadFile(readPipe, &end, sizeof(end), &bytesRead, NULL)) {
				std::cout << "Faild to read";
				return GetLastError();
			}

			EnterCriticalSection(&cs);
			--readerCount;

			if (readerCount == 0) {
				ReleaseSemaphore(semaphore, 1, NULL);
			}
			LeaveCriticalSection(&cs);
		}
		else if (clientWant == MODIFY) {
			WaitForSingleObject(semaphore, INFINITE);

			Order order;
			if(!ReadFile(readPipe, &order, sizeof(Order), &bytesRead, NULL)) {
				std::cout << "Faild to read";
				return GetLastError();
			}

			modify(order);
			if(!WriteFile(writePipe, &END_MODIFY, sizeof(END_MODIFY), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}

			char end;
			if(!ReadFile(readPipe, &end, sizeof(end), &bytesRead, NULL)) {
				std::cout << "Faild to read";
				return GetLastError();
			}
			ReleaseSemaphore(semaphore, 1, NULL);
		}
		else {
			break;
		}
	}
	return 0;
}

void createBinaryFile(char* filename) {
	std::ofstream out(filename, std::ios::binary);
	std::cout << "Enter number of orders\n";
	int num;
	std::cin >> num;
	for (int i = 0; i < num; i++)
	{
		system("cls");
		Order order;
		std::cout << "Enter order number: ";
		std::cin >> order.n;
		std::cout << "Enter order name: ";
		std::cin >> order.name;
		std::cout << "Enter product count: ";
		std::cin >> order.amount;
		std::cout << "Enter product price: ";
		std::cin >> order.price;
		out.write((char*)&order, sizeof(struct Order));
	}
	out.close();
}

void checkDataInBinaryFile(char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	system("cls");
	std::cout << "Orders:\n";
	while (in.peek() != EOF)
	{
		Order order;
		in.read((char*)&order, sizeof(Order));
		std::cout << "Order number:\n" << order.n << "\nOrder name:\n" << order.name << "\nProduct count:\n" 
			<< order.amount << "\nProduct price:\n" << order.price << std::endl << std::endl;
	}
	in.close();
}

int main() {
	InitializeCriticalSection(&cs);
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	std::cout << "File name:";
	std::cin >> filename;

	createBinaryFile(filename);
	checkDataInBinaryFile(filename);

	std::cout << "Amount of client processes: ";
	int count;
	std::cin >> count;

	system("cls");

	HANDLE* handles = new HANDLE[count];
	DWORD* ID = new DWORD[count];
	for (int i = 0; i < count; i++)
	{
		handles[i] = CreateThread(NULL, NULL, client, (void*)i, 0, &ID[i]);
	}

	WaitForMultipleObjects(count, handles, TRUE, INFINITE);
	system("cls");
	checkDataInBinaryFile(filename);
	return 0;
}