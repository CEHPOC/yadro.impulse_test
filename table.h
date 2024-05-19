#pragma once
#include "client.h"

class table{
	int price;
	int income;
	int worktime;
	int startsessiontime;

	public:
	client* name;

	table(int,int);
	void newclient(int, client*);
	void leaveclient(int);
	int get_income();
	int get_worktime();
};
