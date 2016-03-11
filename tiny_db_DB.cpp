#include<utility>
#include<direct.h>
#include<io.h>
#include"tiny_db_DB.h"
#include"tiny_db_memtable.h"
#include"tiny_db_bpt.h"
#include"tiny_db_skiplist.h"

namespace tiny_db{
	
	

	DB::DB() :isCompact(false){
		memtable = new Memtable();
		immutable_table = new Memtable();
		if (_access("\\bptree", 0) == -1){
			_mkdir("\\bptree");
		}
		bptree = new BPTree();
	}

	void DB::reset(){
		immutable_table = memtable;
		memtable = new Memtable();
		dumpTableToDisk(*immutable_table);
	}

	std::string DB::getValue(std::string k){
		std::string s = memtable->getValue(k);
		if (k != "") return s;
		s = immutable_table->getValue(k);
		if (k != "") return s;
		keyType key(k.c_str());
		keyType value;
		bptree->searchKeyfromTree(key, value);
		return s;
	}

	void DB::dumpTableToDisk(Memtable& immutable_table){
		isCompact = true;
		auto node=immutable_table.table->getHead();
		while (node.next_[0]){
			keyType key(node.key.c_str());
			keyType value(node.value.c_str());
			bptree->insertRecordToTree(key,value);
			node = *node.next_[0];
		}
		immutable_table.~Memtable();
		isCompact = false;

	}

	void DB::insert(std::string k, std::string v){
		memtable->addKeyValuePairs(k, v);
		if (memtable->getTally() > maxNum){
			reset();
		}
	}

	void DB::deleteKey(std::string k){
		memtable->deleteKey(k);
		
		if (isCompact){
			immutable_table->deleteKey(k);
		}
		keyType key(k.c_str());
		bptree->deleteRercordFromTree(key);
				
	}

	void DB::update(std::string k, std::string v){
		memtable->updateValue(k, v);
	}
}