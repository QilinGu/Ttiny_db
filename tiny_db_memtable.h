#ifndef _TINY_DB_MEMTABLE_H_
#define _TINY_DB_MEMTABLE_H_ 

#include<string>

namespace tiny_db{
	
	class Skiplist;

	class Memtable{
	private:
		//the upper boundary 
		int tally;
		
		
	public:
		Skiplist *table;
		~Memtable();
		// ++tally iff client add a new Key-Value pair
		void increase();
		int getTally()const;
		// default constructor
		Memtable();
		// add a new new Key-Value pair to table
		void addKeyValuePairs(std::string key, std::string value);
		int deleteKey(std::string key);
		int deleteKeyValuePairs(std::string key, std::string value);
		void updateValue(std::string key, std::string value);
		//get the value of special key 
		std::string getValue(std::string key);
		//return true iff the value of "key" equal to "value" 
		bool isEqual(std::string key, std::string value);
		
	};
}// namespace tiny_db

#endif