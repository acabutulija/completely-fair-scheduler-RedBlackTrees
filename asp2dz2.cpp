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

	string getName();

	void setName(string name) {
		if (name.length() <= MaxStringSize) {
			this->name = name;
		}
		else
		{
			this->name = name.substr(0, MaxStringSize);
		}
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

	bool isLeaf();

	bool isFull();

	bool isEmpty();

	//Returns the position of the inserted process
	//Insert process code relies on count being accurate, in future utilizations count should possibly be made as a private field.
	int insertProcess(Process* process);

	Process* extractProcess(int position);

	Process* nodes[3];
	RBNode* children[4];
	RBNode* parent;
	unsigned short count;
	//Position of the black node
	int blackPosition;
};


class RBTree {
public:

	static RBTree* GetInstance() {
		if (instance == nullptr) {
			instance = new RBTree();
		}

		return instance;
	}

	static void DestroyInstance() {
		if (!instance) return;
		
		delete instance;
		instance = nullptr;
	}

	Process* searchProcess(string name);

	Process* searchProcess(Time waitingTime, Time execTime);

	void insertProcess(Process* process);

	void printTree(ostream& output);

	Process* deleteProcess(Process* process);
	
protected:
	//Forms an empty tree (SINGLETON)
	RBTree();

	//Destructor
	~RBTree();

private:
	//Function is private to prevent splitting with a node that is not in the tree
	void split(RBNode* node, Process* process);

	Process* deleteProcessLeaf(RBNode* node, int position);


	//Help functions for deletion process. ChildIndex included to increase time.
	RBNode* findRealBrother(RBNode* node, int childIndex);

	void borrowOne(RBNode* to, RBNode* from, int childIndex);

	void borrowTwo(RBNode* to, RBNode* from, int childIndex);

	//Merges parent's id1 and id2 nodes
	void merge(RBNode* parent, int id1, int id2);

	int indexOfChild(RBNode* parent, RBNode* child);

	RBNode* findPredecessor(RBNode* node, int pid);

	RBNode* findSuccessor(RBNode* node, int pid);

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

string Process::getName() {
	return this->name;
}

RBNode::RBNode() {
	for (int i = 0; i < 3; i++) {
		this->nodes[i] = nullptr;
	}

	for (int i = 0; i < 4; i++) {
		this->children[i] = nullptr;
	}

	this->count = 0;
	this->blackPosition = 0;
	this->parent = nullptr;
}

RBNode::~RBNode() {
	for (int i = 0; i < 3; i++) {
		if (nodes[i]) delete nodes[i];
	}
}

int RBNode::insertProcess(Process* process) {
	if (count >= 3) return -1;

	int pos = 2;
	for (int i = 0; i < 2; i++) {
		if (!nodes[i] || nodes[i]->getWaitingTime() > process->getWaitingTime()) {
			pos = i;
			break;
		}
	}

	if (nodes[pos]) {
		children[count + 1] = children[count];
		for (int i = count; i > pos; i--) {
			nodes[i] = nodes[i - 1];
			children[i] = children[i - 1];
		}
	}

	nodes[pos] = process;
	count++;

	if (pos <= blackPosition && count == 2) blackPosition++;

	return pos;
}

bool RBNode::isLeaf() {
	for (int i = 0; i < 4; i++) {
		if (this->children[i] != nullptr) return false;
	}
	return true;
}

int RBTree::indexOfChild(RBNode* parent, RBNode* child) {
	for (int i = 0; i < 4; i++) {
		if (parent->children[i] == child) return i;
	}
	return -1;
}

//Other function has to update the black position if black node is removed
Process* RBNode::extractProcess(int position) {
	if (position < 0 && position > count - 1) return nullptr;
	
	Process* p = nodes[position];
	nodes[position] = nullptr;
	
	for (int i = position; i < count - 1; i++) {
		nodes[i] = nodes[i + 1];
	}
	
	count--;
	
	if (position < blackPosition)
		blackPosition--;

	return p;
}

bool RBNode::isFull() {
	/*for (int i = 0; i < 3; i++) {
		if (this->nodes[i] == nullptr) return false;
	}
	return true;*/
	return count == 3;
}

bool RBNode::isEmpty() {
	return count == 0;
}

RBTree* RBTree::instance = nullptr;

RBTree::RBTree() {
	root = nullptr;
}

RBTree::~RBTree() {
	RBNode* curr = root;
	queue<RBNode*> NodesQueue;
	NodesQueue.push(curr);
	while (!NodesQueue.empty()) {
		curr = NodesQueue.front();
		NodesQueue.pop();
		for (int i = 0; i < 4; i++) {
			if (curr->children[i] != nullptr) NodesQueue.push(curr->children[i]);
		}
		delete curr;
	}
}

Process* RBTree::searchProcess(Time waitingTime, Time execTime) {
	RBNode* curr = this->root;

	while (curr) {
		for (int i = 0; i < curr->count; i++) {

			if (curr->nodes[i]->getWaitingTime() == waitingTime && curr->nodes[i]->getExecTime() == execTime) return curr->nodes[i];
			
		}

		int j = 0;
		for (j; j < 3; j++) {
			if (!curr->nodes[j]) continue;

			if (curr->nodes[j]->getWaitingTime() >= waitingTime) break;
		}

		if (curr->nodes[2] && curr->nodes[2]->getWaitingTime() < waitingTime) j++;

		curr = curr->children[j];
	}

	return nullptr;
}

Process* RBTree::searchProcess(string name) {
	RBNode* curr = this->root;
	queue<RBNode*> Q;

	if (curr) Q.push(curr);
	
	while (!Q.empty()) {
		curr = Q.front();
		Q.pop();

		for (int i = 0; i < 3; i++) {
			//if (!curr->nodes[i]) continue;

			if (curr->nodes[i]->getName() == name) return curr->nodes[i];
		}

		for (int j = 0; j < 4; j++) {
			if (curr->children[j]) Q.push(curr->children[j]);
		}

	}

	return nullptr;
}

void RBTree::insertProcess(Process* process) {
	if (!root) {
		RBNode* node = new RBNode;
		node->nodes[0] = process;
		node->count++;
		root = node;
		return;
	}

	RBNode* curr = root;

	while (!curr->isLeaf()) {
		int j;
		for (j = 0; j < 3; j++) {
			if (curr->nodes[j]->getWaitingTime() >= process->getWaitingTime()) break;
		}

		if (curr->nodes[2] && curr->nodes[2]->getWaitingTime() < process->getWaitingTime()) j++;

		curr = curr->children[j];
	}

	if (!curr->isFull()) {
		curr->insertProcess(process);
		return;
	}

	//SPLIT
	RBTree::split(curr, process);

}

void RBTree::split(RBNode* node, Process* process) {
	RBNode* toBeSplit = node;
	RBNode* rightSibling = nullptr;
	RBNode* leftChild = nullptr, *rightChild = nullptr;
	Process* toInsert = process;

	while (toBeSplit->isFull()) {
		Process* middle = toBeSplit->nodes[1];
		rightSibling = new RBNode;
		
		rightSibling->insertProcess(toBeSplit->nodes[2]);
		rightSibling->children[0] = toBeSplit->children[2];
		rightSibling->children[1] = toBeSplit->children[3];
		

		toBeSplit->nodes[1] = nullptr;
		toBeSplit->nodes[2] = nullptr;
		toBeSplit->children[2] = nullptr;
		toBeSplit->children[3] = nullptr;

		toBeSplit->count = 1;
		toBeSplit->blackPosition = 0;

		if (toInsert->getWaitingTime() <= middle->getWaitingTime()) {
			int insertPos = toBeSplit->insertProcess(toInsert);
			toBeSplit->children[insertPos] = leftChild;
			toBeSplit->children[insertPos + 1] = rightChild;
		}
		else
		{
			int insertPos = rightSibling->insertProcess(toInsert);
			rightSibling->children[insertPos] = leftChild;
			rightSibling->children[insertPos + 1] = rightChild;
		}

		RBNode* parent = toBeSplit->parent;

		if (!parent) {
			parent = new RBNode;
			parent->nodes[0] = middle;

			parent->children[0] = toBeSplit;
			parent->children[1] = rightSibling;
			parent->count++;

			toBeSplit->parent = parent;
			rightSibling->parent = parent;
			
			this->root = parent;

			break;
		}

		if (!parent->isFull()) {
			int insertPos = parent->insertProcess(middle);
			parent->children[insertPos] = toBeSplit;
			parent->children[insertPos + 1] = rightSibling;
			toBeSplit->parent = parent;
			rightSibling->parent = parent;
		}
		else
		{
			toInsert = middle;
			leftChild = toBeSplit;
			rightChild = rightSibling;
		}

		toBeSplit = parent;
	}
}



void RBTree::printTree(ostream& output) {
	if (!root) return;

	queue<RBNode*> tLevel;
	
	tLevel.push(root);

	while (!tLevel.empty()) {
		RBNode* node = tLevel.front();
		queue<RBNode*> nLevel;
		tLevel.pop();

		output << "| ";
		for (int i = 0; i < node->count; i++) {
			if (node->nodes[i]) output << node->nodes[i]->getName();
			if (i == node->blackPosition) output << "*";
			output << " ";
		}
		output << " |";

		for (int i = 0; i < 4; i++) {
			if (node->children[i]) nLevel.push(node->children[i]);
		}

		if (tLevel.empty()) { 
			tLevel = nLevel;
			output << "\n";
		}
	}
}

void RBTree::borrowOne(RBNode* to, RBNode* from, int childIndex) {
	RBNode* parent = to->parent;
	
	bool left;
	int parentIndex;
	
	if (childIndex != 3 && parent->children[childIndex + 1] == from)
		left = true;
	else
		left = false;

	if (left) parentIndex = childIndex;
	else parentIndex = childIndex - 1;

	if (left) {
		Process* borrowing = from->extractProcess(0);
		to->insertProcess(parent->nodes[parentIndex]);
		parent->nodes[parentIndex] = borrowing;
		to->children[to->count] = from->children[0];
		
		for (int i = 0; i < 3; i++) {
			from->children[i] = from->children[i + 1];
		}
		from->children[3] = nullptr;
	}
	else
	{
		Process* borrowing = from->extractProcess(1);
		to->insertProcess(parent->nodes[parentIndex]);
		parent->nodes[parentIndex] = borrowing;
		to->children[0] = from->children[from->count];
		from->children[from->count] = nullptr;
	}
}

void RBTree::borrowTwo(RBNode* to, RBNode* from, int childIndex) {
	RBNode* parent = to->parent;

	bool left;
	int parentIndex;

	if (childIndex != 3 && parent->children[childIndex + 1] == from)
		left = true;
	else
		left = false;

	if (left) parentIndex = childIndex;
	else parentIndex = childIndex - 1;

	Process* borrowMiddle, *borrowSecond;

	if (left) {
		borrowMiddle = from->extractProcess(1);
		borrowSecond = from->extractProcess(0);


		to->insertProcess(parent->nodes[parentIndex]);
		to->insertProcess(borrowSecond);
		parent->nodes[parentIndex] = borrowMiddle;
		to->children[1] = from->children[0];
		to->children[2] = from->children[1];

		for (int i = 0; i < 2; i++) {
			from->children[i] = from->children[i + 1];
			from->children[i + 1] = from->children[i + 2];
		}
		from->children[2] = nullptr;
		from->children[3] = nullptr;
	}
	else
	{
		borrowSecond = from->extractProcess(2);
		borrowMiddle = from->extractProcess(1);

		to->insertProcess(parent->nodes[parentIndex]);
		to->insertProcess(borrowSecond);
		parent->nodes[parentIndex] = borrowMiddle;
		to->children[0] = from->children[2];
		to->children[1] = from->children[3];
		from->children[2] = nullptr;
		from->children[3] = nullptr;
	}
}

void RBTree::merge(RBNode* parent, int id1, int id2) {
	RBNode* child = parent->children[id1];
	RBNode* sibling = parent->children[id2];

	int parentId;
	if (id1 < id2) parentId = id1;
	else parentId = id2;

	bool left = id1 < id2;

	Process* parentProcess = parent->extractProcess(parentId);
	Process* siblingProcess = sibling->extractProcess(0);

	child->insertProcess(parentProcess);
	child->insertProcess(siblingProcess);
	
	if (left) {
		child->children[2] = sibling->children[1];
		child->children[1] = sibling->children[0];

		for (int i = parentId + 1; i < 3; i++) {
			parent->children[i] = parent->children[i + 1];
		}
		parent->children[3] = nullptr;

	}
	else
	{
		child->children[0] = sibling->children[0];
		child->children[1] = sibling->children[1];

		for (int i = parentId; i < 3; i++) {
			parent->children[i] = parent->children[i + 1];
		}
		parent->children[3] = nullptr;
	}

	delete sibling;
}

Process* RBTree::deleteProcessLeaf(RBNode* node, int position) {
	//Red leaf
	if (node->blackPosition != position) {
		Process* p = node->extractProcess(position);
		return p;
	}

	//Black leaf
	Process* p = node->extractProcess(position);
	if (node->count > 0) {
		node->blackPosition = node->count - 1;
		return p;
	}

	//Borrow from real brother
	while (node->count == 0) {

		RBNode* parent = node->parent;
		//If process is leaf we dont have from who to borrow
		if (!parent) return p;

		int childIndex = -1;
		for (int i = 0; i < 4; i++) {
			if (parent->children[i] == node) {
				childIndex = i;
				break;
			}
		}

		RBNode* sibling = findRealBrother(node, childIndex);

		if (sibling && sibling->count == 2)
		{
			borrowOne(node, sibling, childIndex);
			return p;
		}

		if (sibling && sibling->count == 3) {
			borrowTwo(node, sibling, childIndex);
			return p;
		}

		if (sibling) {
			int realBroId = indexOfChild(parent, sibling);

			merge(parent, childIndex, realBroId);

			node = parent;
			continue;
		}
		

		//Non-real brother
		if (childIndex != 2 && parent->children[childIndex + 1] != nullptr) sibling = parent->children[childIndex + 1];
		else if (childIndex != 0) sibling = parent->children[childIndex - 1];

		//Doesn't have a sibling
		if (!sibling) return p;

		//Borrow one from non-real brother
		if (sibling && sibling->count == 2)
		{
			borrowOne(node, sibling, childIndex);
			return p;
		}

		//Borrow two from non-real brother
		if (sibling && sibling->count == 3) {
			borrowTwo(node, sibling, childIndex);
			return p;
		}

		int siblingId = indexOfChild(parent, sibling);

		merge(parent, childIndex, siblingId);
		

		node = parent;

	}

	return p;
}

RBNode* RBTree::findRealBrother(RBNode* node, int childIndex) {
	RBNode* realBrother = nullptr;
	RBNode* parent = node->parent;

	if (!parent) return nullptr;
	
	bool left = childIndex % 2;

	if (left) realBrother = parent->children[childIndex + 1];
	else realBrother = parent->children[childIndex - 1];

	if (parent->count == 2) {
		if ((childIndex / 2) == parent->blackPosition) return nullptr;
	}

	return realBrother;
}

RBNode* RBTree::findSuccessor(RBNode* node, int pid) {
	
	RBNode* curr = node->children[pid + 1];

	while (curr && !curr->isLeaf()) {
		curr = curr->children[0];
	}

	return curr;
}

RBNode* RBTree::findPredecessor(RBNode* node, int pid) {
	RBNode* curr = node->children[pid];

	while (curr && !curr->isLeaf()) {
		curr = curr->children[curr->count];
	}

	return curr;
}

int main() {
	Process* p1 = new Process("A", 10, 10);
	Process* p2 = new Process("B", 10, 10);
	Process* p3 = new Process("C", 10, 10);
	Process* p4 = new Process("D", 10, 10);
	
	RBTree* tree = RBTree::GetInstance();
	tree->insertProcess(p1);
	tree->insertProcess(p2);
	tree->insertProcess(p3);
	tree->insertProcess(p4);

	//cout << node->nodes[0]->getWaitingTime() << " " << node->nodes[1]->getWaitingTime() << " ";
	tree->printTree(cout);

	return 0;
}