#include<iostream>
#include<string>
#include<sstream>
#include<cassert>
#include"tiny_db_DB.h"

using std::cout;
using std::endl;

std::string int_to_string(int a){
	std::stringstream ss;
	ss << a;
	return ss.str();
}

int main(int argc, char **argv){
	tiny_db::DB db;//=new tiny_db::DB();
	for (int i = 1; i < 1000000; i++){
		std::string str = int_to_string(i);
		db.insert(str, str);
		cout << "insert : " << i << endl;
	}
	
	for( int i = 100; i < 1000000; i++){
		std::string str = int_to_string(i);
		std::string temp = db.getValue(str);
		assert(str == temp);
		cout<<"getValue : "<<str<<endl;
	}

	for (int i = 100; i < 5000; i++){
		std::string str = int_to_string(i);
		db.deleteKey(str);
		std::string temp = db.getValue(str);
		assert(temp !=str);
		cout << "deleted succeed " << i << endl;
	}
}
