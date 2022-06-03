#pragma once
#include<windows.h>
#include<iostream>
#include<string>

struct Message {
	char name[10];
	char text[20];

	Message() {
		strcpy(name, "");
		strcpy(text, "");
	}

	Message(const char* Name,const char* Text) {
		strcpy(name, Name);
		strcpy(text, Text);
	}
};
