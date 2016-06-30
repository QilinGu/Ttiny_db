#define _CRT_SECURE_NO_DEPRECATE

#include"tiny_db_bpt.h"


namespace tiny_db{

	FType BPTree::getAddress()const{
		_fseeki64(curFile, 0, SEEK_END);
		return _ftelli64(curFile);
	}

	BPTree::BPTree(){
		curFile = fopen("bptree.bin", "wb+");
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
		height = 0;
		writeNodeToDisk(root, b);
	}

	void BPTree::writeNodeToDisk(const FType address, bptNode& b){
		_fseeki64(curFile, address, SEEK_SET);
		fwrite(&b, sizeof(bptNode), 1, curFile);
	}

	void BPTree::readNodeFromDisk(const FType address, bptNode& b){
		_fseeki64(curFile, address, SEEK_SET);
		fread(&b, sizeof(bptNode), 1, curFile);
	}

	//to make sure that keyNum of the node is smaller than maxChild before we insert the record
	void BPTree::splitNode(bptNode& parent, bptNode& childl, int c_index){
		bptNode b;
		for (int i = parent.nums; i > c_index; i--){
			parent.point[i] = parent.point[i - 1];
			parent.child[i] = parent.child[i - 1];
		}
		parent.nums++;
		parent.child[c_index] = childl.child[minChild - 1];
	
		childl.key = childl.child[minChild - 1];
		b.nums = minChild;
		b.type = childl.type;
		for (int i = 0; i < minChild; i++){
			b.child[i] = childl.child[minChild + i];
			b.point[i] = childl.point[minChild + i];
		}
		for (int i = 0; i < minChild; i++){
			childl.point[minChild + i]=0;
		}
		b.key = b.child[b.nums - 1];
		childl.nums = minChild;
		FType address = getAddress();
		parent.point[c_index + 1] = address;
		parent.child[c_index + 1] = b.key;
		
		writeNodeToDisk(address, b);
	}

	void BPTree::searchKeyfromTree(keyType& k,keyType& v){
		bptNode b;
		readNodeFromDisk(root, b);
		while (b.type != LEAF){
			int i = findNodeIndex(b, k);
			if (i >= b.nums){
				i--;
			}
			readNodeFromDisk(b.point[i], b);
		}
		if (memcmp(b.key.k, k.k, 30) == 0 )  v = b.value;
	}

	int BPTree::findNodeIndex(bptNode& b, keyType& k)const{
		int end = b.nums - 1, start = 0,mid;
		while (start<end){
			mid = (start + end)/2;
			if (memcmp(b.child[end].k, k.k, 30) < 0) return end + 1;
			if (memcmp(b.child[mid].k, k.k, 30) == 0) return mid;
			else if (memcmp(b.child[mid].k, k.k, 30) > 0) end = mid;
			else start = mid + 1;
		}
		return start;
	}

	void BPTree::insertRecordToTree(FType ttt,FType fff, keyType& k, keyType& v){
		bptNode b;
		readNodeFromDisk(ttt, b);
		if (ttt == root && b.nums == maxChild){
			if (b.nums == maxChild){
				bptNode newNode;
				b.key = b.child[maxChild - 1];
				for (int i = 0; i < maxChild; i++){
					newNode.point[i] = 0;
				}
				newNode.child[0] = b.key;

				FType tt = root;
				newNode.point[0] = tt;
				newNode.nums = 1;
				newNode.type = INTERNAL;
				splitNode(newNode, b, 0);
				writeNodeToDisk(root, b);
				root = getAddress();
				writeNodeToDisk(root, newNode);
				height++;
				
				if (memcmp(newNode.child[0].k, k.k, 30) > 0){
					return insertRecordToTree(newNode.point[0], root, k, v);
				}
				else{
					return insertRecordToTree(newNode.point[1], root, k, v);
				}
			}
		}
		else{
			readNodeFromDisk(ttt, b);

			if (b.type == INTERNAL){

				if (b.nums == maxChild){
					bptNode parent;
					readNodeFromDisk(fff, parent);
					int kl = findNodeIndex(parent, b.key);
					splitNode(parent, b, kl);
					writeNodeToDisk(fff, parent);
					writeNodeToDisk(ttt, b);
					kl = findNodeIndex(parent, k);
					return insertRecordToTree(parent.point[kl], fff, k, v);
				}
				bptNode c;
				readNodeFromDisk(b.point[0], c);

				if (b.nums == 0 || c.type == LEAF){

					bptNode pre = b;
					int i = findNodeIndex(pre, k);
					
					bptNode temp, temp_1;
					FType address_;

					if (b.nums == 0){
						bptNode temp;
						b.nums++;
						b.key = k;
						b.child[0] = k;
						address_ = getAddress();
						b.point[0] = address_;
						temp.key = k;
						temp.type = LEAF;
						temp.value = v;
						head = address_;
						temp.next = 0;
						temp.pre = 0;
						writeNodeToDisk(address_, temp);
						writeNodeToDisk(ttt, b);
					}
					else{
						if (memcmp(k.k, b.key.k, 30) > 0){
							b.key = k;
							writeNodeToDisk(ttt, b);
						}
						// if we can find the same key in bptree
						// just change the value of the key
						if (b.child[i].k == k.k){
							bptNode d;
							readNodeFromDisk(b.point[i], d);
							d.value = v;
							writeNodeToDisk(b.point[i], d);
						}
						// insert record to leafNode
						else{
							for (int index = pre.nums; index > i; index--){
								pre.child[index] = pre.child[index - 1];
								pre.point[index] = pre.point[index - 1];
							}
							pre.nums++;
							address_ = getAddress();
							pre.child[i] = k;
							pre.point[i] = address_;
							for (int i = 0; i < maxChild; i++){
								temp.point[i] = 0;
							}
							temp.value = v;
							temp.key = k;
							temp.type = LEAF;
							writeNodeToDisk(pre.point[i], temp);
							writeNodeToDisk(ttt, pre);
						}
						// set next pointer and pre pointer
						// case 1:leafNum=0
						// case 2:temp's key is the smallest key
						// case 3:temp's key is the biggest key
						// case 4:other case
						if (leafNum == 0){                           // case:1
							head = address_;
							temp.next = 0;
							temp.pre = 0;
						}
						else if (i == 0){
							readNodeFromDisk(pre.point[1], temp_1);
							if (temp_1.pre == 0){                    // case:2
								temp.next = pre.point[1];
								temp.pre = 0;
								head = address_;
								temp_1.pre = address_;
							}
							else{                                    // case:4
								temp.next = pre.point[1];
								temp.pre = temp_1.pre;
								temp_1.pre = address_;
								bptNode temp_2;
								readNodeFromDisk(temp.pre, temp_2);
								temp_2.next = address_;
								writeNodeToDisk(temp.pre, temp_2);
							}
							writeNodeToDisk(temp.next, temp_1);
						}
						else{
							readNodeFromDisk(pre.point[i - 1], temp_1);
							if (temp_1.next == 0){                    // case:3
								temp_1.next = address_;
								temp.next = 0;
								temp.pre = pre.point[i - 1];

							}
							else{                                     // case:4
								temp.next = pre.point[i + 1];
								temp.pre = pre.point[i - 1];
								temp_1.next = address_;
								bptNode temp_2;
								readNodeFromDisk(temp.next, temp_2);
								temp_2.pre = address_;
								writeNodeToDisk(temp.next, temp_2);
							}
							writeNodeToDisk(temp.pre, temp_1);
						}
						writeNodeToDisk(pre.point[i], temp);
						leafNum++;
					}
				}
				else{
					int i = findNodeIndex(b, k);
					if (i == b.nums){
						if (i > 0){
							i--;
						}
					}
					if (memcmp(k.k, b.key.k, 30) > 0){
						b.key = k;
					}
					writeNodeToDisk(ttt, b);
					fff = ttt;
					return insertRecordToTree(b.point[i], ttt, k, v);
				}
			}
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
			pchild.child[index + 1] = qchild.child[index];
			pchild.point[index + 1] = qchild.point[index];
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
	void BPTree::deleteRercordFromTree(FType fff,FType ttt,keyType& k){
		bptNode b;
		readNodeFromDisk(ttt, b);
		int i = findNodeIndex(b, k);
		if (b.type == INTERNAL){

			if (b.nums >= minChild){
				if (memcmp(b.child[i].k, k.k, 30) != 0){
					//case 1 
					return deleteRercordFromTree(ttt, b.point[i], k);
				}
				else{
					//case 2
					deleteRercordFromTree(ttt, b.point[i], k);
					bptNode c;
					readNodeFromDisk(ttt, c);
					bptNode d;
					readNodeFromDisk(c.point[c.nums - 1], d);
					if (d.nums <= maxChild&&d.nums >=0){
						c.key = d.child[d.nums - 1];
						writeNodeToDisk(ttt, c);
					}

				}
			}
			else{

				bptNode e;
				readNodeFromDisk(fff, e);
				int i = findNodeIndex(e, b.key);
				//case 3
				if (i == 0){
					borrowKeyFromNeighbour(e, b, i);
				}
				//case 4
				else{
					borrowKeyFromNeighbour(e, b, i);
				}
			}
		}
		else{
			//the node type is leaf
			//case 5
			bptNode e;
			readNodeFromDisk(fff, e);
			int i = findNodeIndex(e, b.key);
			//can not find the record
			//do nothing
			if (memcmp(e.child[i].k, k.k, 30) != 0){
				return;
			}
			for (int index = i; index < e.nums - 1; index++){
				e.child[index] = e.child[index + 1];
				e.point[index] = e.point[index + 1];
			}
			e.nums--;
			writeNodeToDisk(fff, e);
			bptNode f, g;
			readNodeFromDisk(b.pre, f);
			readNodeFromDisk(b.next, g);
			if (b.pre != 0 && b.next != 0){
				//case 6
				f.next = b.next;
				g.pre = b.pre;
				writeNodeToDisk(b.pre, f);
				writeNodeToDisk(b.next, g);
			}
			else if (b.pre == 0 && b.next != 0){
				//case 7
				head = b.next;
				g.pre = head;
				writeNodeToDisk(b.next, g);
			}
			else if (b.pre != 0 && b.next == 0){
				//case 8
				f.next = 0;
				writeNodeToDisk(b.pre, f);
			}
		}
		

	}
}//namespace tiny_db