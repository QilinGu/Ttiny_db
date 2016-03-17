

#include<iostream>
#include<utility>
#include"tiny_db_skiplist.h"

namespace tiny_db{



	std::string Skiplist::getValue(const std::string key)const{
		int level = getHeight();
		skipnode* temp = const_cast<skipnode*>(&head);
		level--;
		while (level >= 0){
			temp = findLessThan(key, temp, level);
			level--;
		}
		temp = temp->next_[0];
		assert(temp != nullptr);
		if (temp->key != key){
			return 0;
		}
		return temp->value;
	}

		skipnode* temp = cur;

		while (temp->next_[level] != nullptr){
			
			if (temp->next_[level]->key < key){
					temp = temp->next_[level];
					std::cout << "A";
			}
			else{
				std::cout << "B";
				break;
			}
		}
		assert(temp != nullptr);
		return temp;
	}

	void Skiplist::insertKey(skipnode& s){
		int height_ = randomHeight() - 1;
		int i=getHeight() - 1;
		if (height_ > getHeight() - 1){
			std::cout << "sa";
			for (i = height_; i >= getHeight(); i--){
				head.next_[i] = &s;
				s.next_[i] = nullptr;
			}
			setHeight(height_ + 1);
		}
		skipnode* temp = const_cast<skipnode*>(&head);
		while(i >= 0){
			std::cout << "pa";
			temp = findLessThan(s.key, temp, i);
			s.next_[i] = temp->next_[i];
			temp->next_[i] = &s;
			i--;
		}

	}


	bool Skiplist::isValid(std::string key){
		size_t level = getHeight();
		skipnode* temp = const_cast<skipnode*>(&head);
		while (--level){
			temp = findLessThan(key, temp, level);
		}
		temp = temp->next_[0];
		assert(temp != nullptr);
		return temp->key == key;
	}

	int Skiplist::deleteKey(std::string key){
		int success = 0;
		size_t level = getHeight();
		skipnode* temp = const_cast<skipnode*>(&head), 
				*t = nullptr;
		while (--level){
			temp = findLessThan(key, temp, level);
			if (temp->next_[level]->key == key){
				if (level == 0){
					t = temp->next_[0];
					success = 1;
				}
				temp->next_[level] = temp->next_[level]->next_[level];
			}
		}
		if (t != nullptr){
			delete t;
			t = nullptr;
		}
		return success;
	}

	void Skiplist::updateKey(std::string key, std::string value){
		if (containKey(key)){
			size_t level = getHeight();
			skipnode* temp = const_cast<skipnode*>(&head);
			while (--level){
				temp = findLessThan(key, temp, level);
			}
			temp = temp->next_[0];
			assert(temp != nullptr);
			temp->value = value;
		}
		else{
			skipnode s(key, value);
			insertKey(s);
		}
	}


}//namespace tiny_db;
