#include "table.h"
#include "client.h"

using namespace std;

table::table(int id, int price){
	this->price = price;
	name = 0;
	income = 0;
	worktime = 0;
	startsessiontime = 0;
}

void table::newclient(int time, client* name){
	this->name = name;
	startsessiontime = time;
}

void table::leaveclient(int time){
	worktime += time-startsessiontime;
	income += (time-startsessiontime-1)/60*price+price;
	name = 0;
}

int table::get_income(){
	return income;
}

int table::get_worktime(){
	return worktime;
}
