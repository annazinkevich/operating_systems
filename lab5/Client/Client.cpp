#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<iostream>
#include<fstream>
#include"C:\Users\Андрей\Source\Repos\OC_lab5\OC_lab5\Header.h"

int main(int argc, char* argv[]) {

	HANDLE writePipe = (HANDLE)atoi(argv[1]);
	HANDLE readPipe = (HANDLE)atoi(argv[2]);

	char command;
	while (true) {
		DWORD bytesWrite;
		DWORD bytesRead;
		std::cout << "Enter 'm' - modify, 'r' - read or 'e' - exit:\n";
		std::cin >> command;
		system("cls");
		if (command == 'r') {
			int orderNumber;
			std::cout << "Enter order number:\n";
			std::cin >> orderNumber;

			if (!WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}
			if(!WriteFile(writePipe, &orderNumber, sizeof(orderNumber), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}

			char serverAnswer;
			if(!ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL))
				return GetLastError();

			if (serverAnswer == ORDER_FOUND) {
				Order order;
				if(!ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL))
					return GetLastError();
				std::cout << "\nOrder name:\n" << order.name << "\nProduct count:\n"
					<< order.amount << "\nProduct price:\n" << order.price << std::endl;
			}
			else {
				std::cout << "Order not found.\n";
			}

			char c;
			std::cout << "Press 'f' to finish reading: ";
			std::cin >> c;

			if(!WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}
		}
		else if (command == 'm') {
			int orderNumber;
			std::cout << "Enter order number:\n";
			std::cin >> orderNumber;

			if (!WriteFile(writePipe, &READ, sizeof(READ), &bytesWrite, NULL))
				return GetLastError();
			if (!WriteFile(writePipe, &orderNumber, sizeof(orderNumber), &bytesWrite, NULL))
				return GetLastError();

			char serverAnswer;
			if (!ReadFile(readPipe, &serverAnswer, sizeof(serverAnswer), &bytesRead, NULL))
				return GetLastError();

			if (serverAnswer == ORDER_FOUND) {
				Order order;

				if (!ReadFile(readPipe, &order, sizeof(order), &bytesRead, NULL))
					return GetLastError();
				if (!WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}

				std::cout << "\nOrder name:\n" << order.name << "\nProduct count:\n"
					<< order.amount << "\nProduct price:\n" << order.price << std::endl << std::endl;
				std::cout << "Enter order name:\n";
				std::cin >> order.name;
				std::cout << "Enter product count:\n";
				std::cin >> order.amount;
				std::cout << "Enter product price:\n";
				std::cin >> order.price;

				if (!WriteFile(writePipe, &MODIFY, sizeof(MODIFY), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}
				if (!WriteFile(writePipe, &order, sizeof(order), &bytesWrite, NULL)) {
					std::cout << "Faild to write";
					return GetLastError();
				}
			}
			else {
				std::cout << "Order not found.\n";
			}
			char answer;
			if (!ReadFile(readPipe, &answer, sizeof(answer), &bytesRead, NULL))
				return GetLastError();
			char c;
			std::cout << "Press 'f' to finish modification: ";
			std::cin >> c;
			if (!WriteFile(writePipe, &END_OPERATION, sizeof(END_OPERATION), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}
		}
		else {
			if(!WriteFile(writePipe, &EXIT, sizeof(EXIT), &bytesWrite, NULL)) {
				std::cout << "Faild to write";
				return GetLastError();
			}
			break;
		}
	}
	CloseHandle(writePipe);
	CloseHandle(readPipe);
	return 0;
}