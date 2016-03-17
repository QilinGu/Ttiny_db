#ifndef _TINY_DB_DB_H_
#define _TINY_DB_DB_H_


#include<string>

namespace tiny_db{

	class Memtable;
	class BPTree;

	class DB{
	private:
		Memtable* memtable;
		Memtable* immutable_table;
		BPTree* bptree;
		size_t maxNum=50000;
		bool isCompact;
		//no copying allowed
		DB(DB&);
		DB operator = (DB&);
	public:
		DB();
		~DB(){};
		void dumpTableToDisk(Memtable& immutable_table);
		void insert(std::string, std::string);
		void deleteKey(std::string);
		void update(std::string, std::string);
		std::string getValue(std::string);
		void reset();
	};
} //namespace tiny_db
#endif 
