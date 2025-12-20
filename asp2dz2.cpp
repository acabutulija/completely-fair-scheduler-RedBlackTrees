#include <iostream>
#include <string>
#include <fstream>
#include <queue>
using namespace std;

//Class declaraction

typedef unsigned int Time;

const size_t MaxStringSize = 256;

class Process {
public:

	Process(string name, Time timeToComplete, Time maxWaitingTime);

	static inline void setTimeSlice(Time timeSlice) {
		Process::timeSlice = timeSlice;
	}

	inline void setWaitingTime(Time waitingTime) {
		this->waitingTime = waitingTime;
	}

	inline Time getWaitingTime() {
		return this->waitingTime;
	}

	inline void setExecTime(Time executionTime) {
		this->executionTime = executionTime;
	}

	inline Time getExecTime() {
		return this->executionTime;
	}

	//Initilize both values to 0
	inline void initWaitExecTime() {
		this->waitingTime = 0;
		this->executionTime = 0;
	}

	//Returns TRUE if waiting time had to be adajusted by maxWaitingTime.
	bool increaseWaitingTime(Time amount);


private:
	string name;
	Time timeToComplete;
	Time maxWaitingTime;
	Time waitingTime;
	Time executionTime;


	static Time timeSlice;
};

class RBNode {
public:
	RBNode();

	~RBNode();

	Process* nodes[3];
	RBNode* children[4];
	unsigned short count;
};


class RBTree {
public:

	static RBTree* GetInstance() {
		if (instance == nullptr) {
			instance = new RBTree();
		}

		return instance;
	}

	
	
protected:
	//Forms an empty tree (SINGLETON)
	RBTree();

	//Destructor
	~RBTree();

private:
	RBNode* root;

	static RBTree* instance;
};

//Class declaration end
// 
//Function definition

Time Process::timeSlice = 5;

bool Process::increaseWaitingTime(Time amount) {
	this->waitingTime += amount;
	if (this->waitingTime >= this->maxWaitingTime) {
		this->waitingTime -= this->maxWaitingTime;
		return true;
	}

	return false;
}

Process::Process(string name, Time timeToComplete, Time maxWaitingTime) 
	: timeToComplete(timeToComplete), maxWaitingTime(maxWaitingTime), waitingTime(0), executionTime(0) {
	if (name.length() <= MaxStringSize) {
		this->name = name;
	}
	else
	{
		this->name = name.substr(0, MaxStringSize);
	}
}

RBNode::RBNode() {
	for (int i = 0; i < 3; i++) {
		this->nodes[i] = nullptr;
	}

	for (int i = 0; i < 4; i++) {
		this->children[i] = nullptr;
	}

	this->count = 0;
}

RBNode::~RBNode() {
	for (int i = 0; i < 3; i++) {
		if (nodes[i]) delete nodes[i];
	}
}

RBTree* RBTree::instance = nullptr;

RBTree::RBTree() {
	root = nullptr;
}

RBTree::~RBTree() {
	RBNode* curr = root;

	while (curr != nullptr) {

	}
}

int main() {
	
	return 0;
}