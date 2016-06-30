#include "tiny_db_memtable.h"
#include "tiny_db_skiplist.h"



namespace tiny_db{

	const int SUCCESS = 1;
	const int FAIL = 0;

	//the implement of class Memtable


	void Memtable::addKeyValuePairs(std::string key, std::string value){
		skipnode* s = new skipnode(key,value);
		table->insertKey(*s);
		increase();
	}

	int Memtable::deleteKey(std::string key){
		if (table->deleteKey(key)){
			
			return SUCCESS;
		}
		else{
			return FAIL;
		}
	}

	int Memtable::deleteKeyValuePairs(std::string key, std::string value){
		if (getValue(key) == value){
			deleteKey(key);
			
			return SUCCESS;
		}
		else{
			return FAIL;
		}
	}

	std::string Memtable::getValue(std::string key){
		return table->getValue(key);
	}

	void Memtable::updateValue(std::string key, std::string value){
		table->updateKey(key, value);
	}

	bool Memtable::isEqual(std::string key, std::string value){
		std::string s;
		if (table->containKey(key)){
			return getValue(key) == value;
		}
		else{
			return false;
		}
	}
	
	void Memtable::increase(){ 
		++tally;
	}

	int Memtable::getTally()const{
		return tally;
	}

	Memtable::Memtable(){
		tally = 0;
		table = new Skiplist();
	};

	Memtable::~Memtable(){};
}//namespace tiny_db
