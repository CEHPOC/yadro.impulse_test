#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include "table.h"
#include <queue>
#include <unordered_map>
#include "client.h"

using namespace std;

string check_format(ifstream &fin){
	int n=0;
	string line;
	smatch matches;
	regex reg_count("([1-9][0-9]*)");
	regex reg_time("([01][0-9]|2[0-3]):[0-5][0-9] ([01][0-9]|2[0-3]):[0-5][0-9]");
	const regex r("(([01][0-9]|2[0-3]):[0-5][0-9]) ([1-4]) [a-z0-9_-]+ ?([1-9]?[0-9]*)");

	getline(fin,line);
	if(!regex_match(line, matches , reg_count))
		return line;
	n=stoi(matches[1].str());

	getline(fin,line);
	if(!regex_match(line, reg_time))
		return line;

	getline(fin,line);
	if(!regex_match(line, reg_count))
		return line;

	string prev_time = "";
	while(!fin.eof()){
		getline(fin,line);
		if(!regex_match(line, matches, r))
			return line;
		if(stoi(matches[3].str())!=2 && matches[4].str()!="")
			return line;
		if(stoi(matches[3].str())==2 && (matches[4].str()=="" || stoi(matches[4].str())>n))
			return line;
		if(prev_time>matches[1].str())
			return line;
		prev_time = matches[1].str();
	}
	return "";
}

int get_time(string line){
	return ((line[0]-'0')*10+line[1]-'0')*60+(line[3]-'0')*10+line[4]-'0';
}

string get_name(string line){
	if(line[6]=='2')
		return line.substr(8,line.rfind(" ")-8);
	return line.substr(8);
}

string transform_time(int time){
	string result = "";
	result += to_string(time/600);
	time = time%600;
	result += to_string(time/60);
	time = time%60;
	result += ":";
	result += to_string(time/10);
	time = time%10;
	result += to_string(time);
	return result;
}

int main(int argc, char *argv[]){
	if(argc<2){
		cerr << "No file name" << endl;
		return 1;
	}

	string filename(argv[1]);
	ifstream fin(filename);
	int n;
	int start_time;
	int end_time;
	int price;
	bool f_end_of_work = false;

	string check = check_format(fin);
	if(check!=""){
		cerr << check << endl;
		return 1;
	}

	fin.close();
	fin.open(filename);

	string line;
	getline(fin,line);
	n = stoi(line);
	getline(fin,line);
	cout << line.substr(0,5) << endl;
	start_time = ((line[0]-'0')*10+(line[1]-'0'))*60+(line[3]-'0')*10+line[4]-'0';
	end_time = ((line[6]-'0')*10+(line[7]-'0'))*60+(line[9]-'0')*10+line[10]-'0';
	getline(fin,line);
	price = stoi(line);

	vector<table*> tables(n);
	for(int i=0;i<n;i++)
		tables[i]=new table(i+1,price);
	queue<client*> que;
	map<string, client*> clients;
	int current_time;
	string name;


	while(!fin.eof()){
		getline(fin,line);
		if(line=="")
			continue;
		current_time = get_time(line);

		if(current_time>end_time && !f_end_of_work){
			f_end_of_work = true;
			for(auto cl : clients){
				tables[cl.second->table]->leaveclient(end_time);
				cout << transform_time(end_time) << " 11 " << cl.first << endl;
				delete clients[cl.first];
			}
			clients.clear();
		}

		name = get_name(line);
		cout << line << endl;
		string time = line.substr(0,6);
		if(line[6]=='1'){
			if(clients.count(name)!=0){
				cout << time << "13 YouShallNotPass" << endl;
				continue;
			}
			if(current_time<start_time||current_time>end_time){
				cout << time << "13 NotOpenYet" << endl;
				continue;
			}
			clients.insert({name, new client(name)});
			continue;
		}
		if(line[6]=='2'){
			int table_number = stoi(line.substr(line.rfind(" ")+1))-1;
			if(tables[table_number]->name!=0){
				cout << time << "13 PlaceIsBusy" << endl;
				continue;
			}
			if(clients.count(name)==0){
				cout << time << "13 ClientUnknown" << endl;
				continue;
			}
			tables[table_number]->newclient(current_time, clients[name]);
			clients[name]->table = table_number;
			continue;
		}
		if(line[6]=='3'){
			bool f_free_tables = false;
			for(int i=0;i<n;i++){
				if(tables[i]->name==0)
					f_free_tables = true;
			}
			if(f_free_tables){
				cout << time << "13 ICanWaitNoLonger!" << endl;
				continue;
			}
			if(que.size()>=n){
				cout << time << "11 " << name << endl;
				delete clients[name];
				clients.erase(name);
				continue;
			}
			que.push(clients[name]);
		}
		if(line[6]=='4'){
			if(clients.count(name)==0){
				cout << time << "13 ClientUnknown" << endl;
			}
			int m = clients[name]->table;
			tables[m]->leaveclient(current_time);
			delete clients[name];
			clients.erase(name);
			if(!que.empty()){
				client *p = que.front();
				que.pop();
				tables[m]->newclient(current_time, p);
				p->table = m;
				cout << time << "12 " << p->name << " " << (m+1) << endl;
			}
			continue;
		}
	}

	if(!f_end_of_work){
		for(auto cl : clients){
			tables[cl.second->table]->leaveclient(end_time);
			cout << transform_time(end_time) << " 11 " << cl.first << endl;
			delete clients[cl.first];
		}
		clients.clear();
	}

	cout << transform_time(end_time) << endl;
	for(int i=0;i<n;i++){
		cout << (i+1) << " " << tables[i]->get_income() << " " << transform_time(tables[i]->get_worktime()) << endl;
	}

	return 0;
}
