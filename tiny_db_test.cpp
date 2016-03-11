#include<iostream>
#include<string>
#include"tiny_db_DB.h"


int main(int argc,char **argv){
	tiny_db::DB db;//=new tiny_db::DB();
	std::string sl = "d";
	db.insert(sl, sl);
	db.insert("assd", "fd");
	db.insert("aasd", "fd");
	std::string s = db.getValue("aasd");
	std::cout << s;
	std::cin >> s;
}