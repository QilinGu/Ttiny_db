#ifndef _TINY_DB_BPT_H_
#define _TINY_DB_BPT_H_

#include <string>
#include <cassert>
#include <stack>

namespace tiny_db{

    using std::stack;

	enum NODE_TYPE{INTERNAL,LEAF};
	typedef int nodeType;
	typedef long long FType;

	//the order of B+Tree
	const int order = 500;
	const int maxChild = 2*order;
	const int minChild = order;

	struct keyType{
		char k[32];
		keyType(){
			memset(k, 0, sizeof(k));
		}
		keyType(const char* s){
			memset(k, 0, sizeof(k));
			strcpy_s(k, s);
		}
	};
	
	// bplustree's node
	struct bptNode{
		keyType child[maxChild];
		FType point[maxChild];       //point to children's address
		NODE_TYPE type;
		FType next;
		FType pre;
		size_t nums;                 // the sum of nodes
		keyType key;
		keyType value;
		
	};

	// b+tree
	class BPTree{
	private:
		FType head;
		FILE*  curFile;
		unsigned long long leafNum;
		// no copying allowed;
		BPTree(BPTree&);
		BPTree operator =(BPTree&);
		int height;
    protected:
        stack<FType> addressStack;
	public:
		FType getAddress();
		BPTree();
		~BPTree();
		//read a specified node from disk
		void readNodeFromDisk(const FType address,bptNode& b);
		//write a node to disk
		void writeNodeToDisk(const FType address, bptNode& b);
		//init a bplustree
		void initBPTree();
		//search a specified key from BPTree 
		//if find it,set v = record.value
		//else do nothing
		void searchKeyfromTree(keyType& k,keyType& v);
		//insert record to BPTree
		void insertRecordToTree(FType ttt, FType fff, keyType& k, keyType& v);
		//delete record from BPTree
		void deleteRercordFromTree(FType fff, FType ttt, keyType& k);
		//borrow key from neigborhood
		void borrowKeyFromNeighbour(bptNode& parent, bptNode& child, size_t c_index);
		//split node
		void splitNode(bptNode& parent, bptNode& child, int c_index);
		//find the key less than specified key
		int findNodeIndex(bptNode& b, keyType& k)const;
		// merge two nodes
		void mergeTwoNodes(bptNode& parent, bptNode& pchild, bptNode& qchild, size_t p_index);
		FType root;
	};
}//namespace tiny_db
#endif