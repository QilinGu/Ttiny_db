#include<stack>
#include"tiny_db_bpt.h"
using std::stack;

namespace tiny_db{


	FType BPTree::getAddress()const{
		fseek(curFile, 0, SEEK_END);
		return ftell(curFile);
	}

	BPTree::BPTree(){
		curFile = fopen("\\bptree", "rb+");
		initBPTree();
	}

	BPTree::~BPTree(){
		fclose(curFile);
	}

	void BPTree::initBPTree(){
		root = getAddress();
		bptNode b;
		b.point[maxChild - 1] = 0;
		b.next = 0;
		b.nums = 0;
		b.type = INTERNAL;
		head = 0;
		leafNum = 0;
		writeNodeToDisk(root, b);
	}

	void BPTree::writeNodeToDisk(const FType address, bptNode& b){
		fseek(curFile, address, SEEK_SET);
		fwrite(&b, sizeof(bptNode), 1, curFile);
	}

	void BPTree::readNodeFromDisk(const FType address, bptNode& b){
		fseek(curFile, address, SEEK_SET);
		fread(&b, sizeof(bptNode), 1, curFile);
	}

	//to make sure that keyNum of the node is smaller than maxChild before we insert the record
	void BPTree::splitNode(bptNode& parent, bptNode& child, size_t c_index){
		bptNode b;
		for (int i = parent.nums; i > c_index; i--){
			parent.point[i] = parent.point[i - 1];
			parent.child[i] = parent.child[i - 1];
		}
		parent.nums++;
		parent.child[c_index] = child.child[maxChild / 2 - 1];
		child.nums = minChild;
		FType address = getAddress();
		parent.point[c_index+1] = address;
		parent.child[c_index + 1] = child.child[maxChild - 1];
		b.nums = minChild;
		b.type = child.type;
		for (int i = 0; i < minChild; i++){
			b.child[i] = child.child[minChild + i];
			b.point[i] = child.point[minChild + i];
		}
		writeNodeToDisk(address, b);
	}

	void BPTree::searchKeyfromTree(keyType& k,keyType& v){
		bptNode b;
		readNodeFromDisk(root, b);
		while (b.type != LEAF){
			int i = findNodeIndex(b, k);
			readNodeFromDisk(b.point[i], b);
		}
		if (memcmp(b.key.k, k.k, 30) == 0 )  v = b.value;
	}

	int BPTree::findNodeIndex(bptNode& b, keyType& k)const{
		int end = b.nums - 1, start = 0,mid;
		while (start<end){
			mid = (start + end) >> 1;
			if (memcmp(b.child[mid].k, k.k, 30) < 0) return end + 1;
			if (memcmp(b.child[mid].k, k.k, 30) == 0) return mid;
			else if (memcmp(b.child[mid].k, k.k, 30) > 0) end = mid;
			else start = mid + 1;
		}
		return start;
	}

	void BPTree::insertRecordToTree(keyType& k, keyType& v){
		bptNode b;
		int i=0;
		FType j;
		bptNode pre;
		readNodeFromDisk(root, b);
		if (b.nums==maxChild){
			bptNode newNode;
			b.key = b.child[maxChild - 1];
			newNode.child[0] = b.key;
			newNode.point[0] = root;
			splitNode(newNode, b, 0);
			writeNodeToDisk(root, b);
			root = getAddress();
			writeNodeToDisk(root, newNode);
		}
		readNodeFromDisk(root, b);

		while (b.type != LEAF){
			if (b.nums == maxChild){
				splitNode(pre, b, i);
				writeNodeToDisk(pre.point[i], b);
				writeNodeToDisk(j, pre);
			}
			if (i) {
				j = b.point[i];
			}

			i = findNodeIndex(b, k);

			// in this case , the key is biggest than any key in bptree
			//so we need to change b.key 
			if (i == b.nums){
				i--;
				b.key = k;
			}
			pre = b;
			readNodeFromDisk(b.point[i], b);
		}
		// if we can find the same key in bptree
		// just change the value of the key
		if (b.key.k == k.k){
			b.value = v;
		}
		// insert record to leafNode
		else{
			for (int index = pre.nums; index > i; index--){
				pre.child[index] = pre.child[index - 1];
				pre.point[index] = pre.point[index - 1];
			}
			pre.nums++;
			FType f = getAddress();
			pre.child[i] = k;
			pre.point[i] = f;
			writeNodeToDisk(j, pre);

			bptNode temp,temp_1;
			temp.value = v;
			temp.key = k;
			temp.type = LEAF;
			// set next pointer and pre pointer
			// case 1:leafNum=0
			// case 2:temp's key is the smallest key
			// case 3:temp's key is the biggest key
			// case 4:other case
			if (leafNum == 0){                           // case:1
				head = f;
				temp.next = 0;
				temp.pre = 0;
			}
			else if(i==0){
				readNodeFromDisk(pre.point[1], temp_1);
				if (temp_1.pre == 0){                    // case:2
					temp.next = pre.point[1];
					temp.pre = 0;
					head = f;
					temp_1.pre = f;
				}
				else{                                    // case:4
					temp.next = pre.point[1];
					temp.pre = temp_1.pre;
					temp_1.pre = f;
					bptNode temp_2;
					readNodeFromDisk(temp.pre, temp_2);
					temp_2.next = f;
					writeNodeToDisk(temp.pre, temp_2);
				}
				writeNodeToDisk(temp.next, temp_1);
			}
			else{
				readNodeFromDisk(pre.point[i - 1], temp_1);
				if (temp_1.next == 0){                    // case:3
					temp_1.next = f;
					temp.next = 0;
					temp.pre = pre.point[i - 1];
					
				}
				else{                                     // case:4
					temp.next = pre.point[i+1];
					temp.pre = pre.point[i-1];
					temp_1.next = f;
					bptNode temp_2;
					readNodeFromDisk(temp.next, temp_2);
					temp_2.pre = f;
					writeNodeToDisk(temp.next, temp_2);
				}
				writeNodeToDisk(temp.pre, temp_1);
			}
			writeNodeToDisk(pre.point[i], temp);
			leafNum++;
		}
	}

	// we need to merge two nodes in some situation
	// which causes the "child" node missing 
	// so we write the "child" node to disk in this fuction if it still exist
	void BPTree::borrowKeyFromNeighbour(bptNode& parent, bptNode& child, size_t c_index){
		if (c_index == parent.nums - 1){
			bptNode b;
			readNodeFromDisk(parent.point[c_index - 1], b);
			if (b.nums == minChild){
				mergeTwoNodes(parent, b, child, c_index - 1);
			}
			else{
				b.nums--;
				for (int index = child.nums; index > 0; index++){
					child.child[index] = child.child[index - 1];
					child.point[index] = child.point[index - 1];
				}
				child.child[0] = b.child[b.nums];
				child.point[0] = b.point[b.nums];
				b.key = b.child[b.nums - 1];
				writeNodeToDisk(parent.point[c_index], child);
			}
			writeNodeToDisk(parent.point[c_index - 1], b);
		}
		else{
			bptNode b;
			readNodeFromDisk(parent.point[c_index + 1], b);
			if (b.nums == minChild){
				mergeTwoNodes(parent, child, b, c_index + 1);
			}
			else{
				b.nums--;
				child.child[child.nums] = b.child[0];
				child.point[child.nums] = b.point[0];
				for (int index = 0; index < b.nums; index++){
					b.child[index] = b.child[index + 1];
					b.point[index] = b.point[index + 1];
				}
				writeNodeToDisk(parent.point[c_index + 1], b);
			}
			writeNodeToDisk(parent.point[c_index], child);
		}
	}

	void BPTree::mergeTwoNodes(bptNode& parent, bptNode& pchild, bptNode& qchild, size_t p_index){
		for (int index = 0; index < qchild.nums; index++){
			pchild.child[pchild.nums] = qchild.child[index];
			pchild.point[pchild.nums] = qchild.point[index];
			pchild.nums++;
		}
		pchild.key = pchild.child[pchild.nums - 1];
		for (int index = p_index + 1; index < parent.nums-1; index++){
			parent.child[index] = parent.child[index + 1];
			parent.point[index] = parent.point[index + 1];
		}
		parent.nums--;
	}

	// in this fuction, we do nothing if the key is not exist
	void BPTree::deleteRercordFromTree(keyType& k){
		// if k is equal to internalKey's key
		// we push the address of those internalKey
		// and change them after we have deleted the leafNode
		stack<FType> f_sta;
		stack<int> i_sta;
		bptNode cur,parent;
		readNodeFromDisk(root, cur);
		FType s_address = root;
		int i;
		while (cur.type != LEAF){
			i = findNodeIndex(cur, k);
			if (cur.child[i].k==k.k){
				f_sta.push(s_address);
				i_sta.push(i);
			}
			s_address = cur.point[i];
			parent = cur;
			readNodeFromDisk(s_address, cur);
		}

		keyType temp;
		//delete leafNode
		bptNode preNode, nextNode;
		if (cur.pre != 0){
			readNodeFromDisk(cur.pre, preNode);
			preNode.next = cur.next;
			temp = preNode.key;
			writeNodeToDisk(cur.pre, preNode);
		}
		if (cur.next != 0){
			readNodeFromDisk(cur.next, nextNode);
			nextNode.pre = cur.pre;
			writeNodeToDisk(cur.next, nextNode);
		}
		parent.nums--;
		for (int index = i; index < parent.nums; index++){
			parent.child[index] = parent.child[index + 1];
			parent.point[index] = parent.point[index + 1];
		}

		FType child, father;
		int c_i;
		bptNode cNode, pNode;

		while (!f_sta.empty()){
			child = f_sta.top();
			f_sta.pop();
			if (!f_sta.empty()){
				father = f_sta.top();
				c_i = i_sta.top();
				readNodeFromDisk(child, cNode);
				readNodeFromDisk(father, pNode);
				cNode.child[c_i] = temp;
				if (cNode.key.k == k.k){
					cNode.key = temp;
				}

				i_sta.pop();
				int p_i = i_sta.top();
				if (cNode.nums < minChild){
					borrowKeyFromNeighbour(pNode, cNode, p_i);
					temp = cNode.child[cNode.nums - 1];
				}
				writeNodeToDisk(child, cNode);
			}
			else{
				c_i = i_sta.top();
				i_sta.pop();
				readNodeFromDisk(child, cNode);
				cNode.child[c_i] = temp;
				writeNodeToDisk(child, cNode);
			}
		}
	}
}//namespace tiny_db