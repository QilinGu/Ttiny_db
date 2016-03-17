#ifndef _TINY_DB_SKIPLIST_H_
#define _TINY_DB_SKIPLIST_H_

#include<string>
#include<iostream>
#include<cassert>
#include<ctime>

namespace tiny_db{

	//the node of skiplist
	struct skipnode{
		std::string key;
		std::string value;
		skipnode *next_[12];

		skipnode(){};

		skipnode(std::string k, std::string v) :key(k), value(v){
			next_[0] = nullptr;
		};

	};


	//the storage structure in memory
	class Skiplist{
	private:
		int maxHeight;
		int curHeight;
		skipnode head;
		int randomLevel;
	public:
		skipnode getHead()const{
			return head;
		};
		Skiplist();
		//get random height 
		inline int getHeight()const{
			return curHeight;
		}
		inline void setHeight(int h){
			assert(h >= 0);
			curHeight = h;
		}
		//get random height 
		int randomHeight();
		//return the latest skipnode which less than key in a special level
		skipnode* findLessThan(const std::string key, skipnode* cur, int level)const;
		//return the value of key
		std::string getValue(const std::string key)const;
		//insert the key-value pair
		void insertKey(skipnode& s);
		//return true iff skiplist contains the key
		bool isValid(std::string key);
		//delete a skipnode from skiplist
		int deleteKey(std::string key);
		// return true iff contains the key
		bool containKey(std::string key);
		void updateKey(std::string key, std::string value);
	};

	
	inline int Skiplist::randomHeight(){
		randomLevel = (16807L * randomLevel) % ((1<<16) - 1);
		randomLevel = randomLevel % maxHeight + 1;
		assert(randomLevel <= maxHeight);
		assert(randomLevel >= 1);
		return randomLevel;
	}
	

	
	inline bool Skiplist::containKey(std::string key){
		return isValid(key);
	}

	
}
#endif
