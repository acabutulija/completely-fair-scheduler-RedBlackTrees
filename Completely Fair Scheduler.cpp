#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <sstream>
#include <vector>
#include <stack>
using namespace std;

//Class declaraction

typedef unsigned int Time;

const size_t MaxStringSize = 256;

class Process {
public:

	Process(string name, Time timeToComplete, Time maxWaitingTime);

	Process(string name, Time timeToComplete, Time maxWaitingTime, Time waitingTime, Time execTime);

	/*static inline void setTimeSlice(Time timeSlice) {
		Process::timeSlice = timeSlice;
	}*/

	inline void setWaitingTime(Time waitingTime) {
		this->waitingTime = waitingTime;
	}

	inline Time getMaxWaitTime() {
		return this->maxWaitingTime;
	}

	inline void setMaxWaitTime(Time max) {
		this->maxWaitingTime = max;
	}

	inline Time getTimeToComplete() {
		return this->timeToComplete;
	}

	inline void setTimeToComplete(Time ttc) {
		this->timeToComplete = ttc;
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

	string toString();

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


	//static Time timeSlice;
};

class RBNode {
public:
	RBNode();

	~RBNode();

	bool isLeaf();

	bool isFull();

	bool isEmpty();

	string toString();

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

struct NodeId {
	RBNode* node;
	int id;

	NodeId(RBNode* node, int id) {
		this->node = node;
		this->id = id;
	}
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

	/*inline RBNode* getRoot() {
		return this->root;
	}*/

	Process* searchProcess(string name);

	Process* searchProcess(Time waitingTime, Time execTime);

	NodeId searchProcess(Process* process);

	void insertProcess(Process* process);

	void printTreeHorizontal(ostream& output);

	Process* removeProcess(RBNode* node, int id);

	void printTreeVertical(ostream& output);

	void printTreeRedBlack(ostream& output);

	bool insertFromFile(string filename);

	RBNode* findMin();

	void updateWaitTimes(Time increment);
	
	inline bool Empty() {
		return this->root == nullptr;
	}

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

class Simulator {
public:
	static Simulator* GetInstance() {
		if (!Simulator::instance) {
			Simulator::instance = new Simulator();
		}
		return Simulator::instance;
	}

	void executeOneStep(ostream& output);

	void executeSimulation(ostream& output);



protected:
	Simulator();

private:
	static Simulator* instance;

	//Process that is currently being tended to
	Process* currentProcess;
	Time totalTime;
	Time timeSlice;
	Time timeStep = 1;
};

//Class declaration end
// 
//Function definition

//Time Process::timeSlice = 5;

string Process::toString()
{
	string out = "";

	out += this->name + '\n';
	out += "Wait time: " + to_string(this->waitingTime) + '\n';
	out += "Exec time: " + to_string(this->executionTime) + '\n';
	out += "Max wait time: " + to_string(this->maxWaitingTime) + '\n';
	out += "Time to complete: " + to_string(this->timeToComplete);

	return out;
}

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

Process::Process(string name, Time timeToComplete, Time maxWaitingTime, Time waitingTime, Time execTime)
	: timeToComplete(timeToComplete), maxWaitingTime(maxWaitingTime), waitingTime(waitingTime), executionTime(execTime) {
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
	/*for (int i = 0; i < 3; i++) {
		if (nodes[i]) delete nodes[i];
	}*/
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

	if (pos <= blackPosition && count >= 2) blackPosition++;

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
	if (position < 0 || position > (count - 1)) return nullptr;
	
	Process* p = nodes[position];
	nodes[position] = nullptr;
	
	for (int i = position; i < count - 1; i++) {
		nodes[i] = nodes[i + 1];
	}
	
	count--;
	nodes[count] = nullptr;

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
Simulator* Simulator::instance = nullptr;

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
		for (j; j < curr->count; j++) {

			if (curr->nodes[j]->getWaitingTime() >= waitingTime) break;
		}

		//if (curr->nodes[2] && curr->nodes[2]->getWaitingTime() < waitingTime) j++;

		curr = curr->children[j];
	}

	return nullptr;
}

NodeId RBTree::searchProcess(Process* process) {
	//RBNode* curr = this->root;

	//while (curr) {
	//	for (int i = 0; i < curr->count; i++) {

	//		if (curr->nodes[i] == process) return NodeId(curr, i);

	//	}

	//	int j = 0;
	//	for (j; j < curr->count; j++) {

	//		if (curr->nodes[j]->getWaitingTime() > process->getWaitingTime()) break;
	//	}

	//	//if (curr->nodes[2] && curr->nodes[2]->getWaitingTime() < waitingTime) j++;

	//	curr = curr->children[j];
	//}

	//return NodeId(nullptr, -1);

	RBNode* curr = this->root;
	queue<RBNode*> Q;

	if (curr) Q.push(curr);

	while (!Q.empty()) {
		curr = Q.front();
		Q.pop();

		for (int i = 0; i < curr->count; i++) {
			//if (!curr->nodes[i]) continue;

			if (curr->nodes[i] == process) return NodeId(curr, i);
		}

		for (int j = 0; j < 4; j++) {
			if (curr->children[j]) Q.push(curr->children[j]);
		}

	}

	return NodeId(nullptr, -1);
}

Process* RBTree::searchProcess(string name) {
	RBNode* curr = this->root;
	queue<RBNode*> Q;

	if (curr) Q.push(curr);
	
	while (!Q.empty()) {
		curr = Q.front();
		Q.pop();

		for (int i = 0; i < curr->count; i++) {
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
		for (j = 0; j < curr->count; j++) {
			if (process->getWaitingTime() < curr->nodes[j]->getWaitingTime()) break;
		}
		
		//if (curr->nodes[2] && process->getWaitingTime() > curr->nodes[2]->getWaitingTime()) j++;

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
		
		if (rightSibling->children[0])
			rightSibling->children[0]->parent = rightSibling;
		if (rightSibling->children[1])
			rightSibling->children[1]->parent = rightSibling;

		toBeSplit->nodes[1] = nullptr;
		toBeSplit->nodes[2] = nullptr;
		toBeSplit->children[2] = nullptr;
		toBeSplit->children[3] = nullptr;

		toBeSplit->count = 1;
		toBeSplit->blackPosition = 0;


		//Vulnaruble to bug, needs to be sure that middle is going to be inserted on the same side as the child.
		if (toInsert->getWaitingTime() < middle->getWaitingTime()) {
			int insertPos = toBeSplit->insertProcess(toInsert);
			toBeSplit->children[insertPos] = leftChild;
			toBeSplit->children[insertPos + 1] = rightChild;
			if (leftChild) leftChild->parent = toBeSplit;
			if (rightChild) rightChild->parent = toBeSplit;
		}
		else
		{
			int insertPos = rightSibling->insertProcess(toInsert);
			rightSibling->children[insertPos] = leftChild;
			rightSibling->children[insertPos + 1] = rightChild;
			if (leftChild) leftChild->parent = rightSibling;
			if (rightChild) rightChild->parent = rightSibling;
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
			break;
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



void RBTree::printTreeHorizontal(ostream& output) {
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

Process* RBTree::removeProcess(RBNode* node, int id) {
	if (!node->isLeaf()) {
		RBNode* replacement = findSuccessor(node, id);
		int replacementId = 0;
		
		if (!replacement) {
			replacement = findPredecessor(node, id);
			replacementId = replacement->count - 1;
		}

		Process* forDeletion = node->nodes[id];
		node->nodes[id] = replacement->nodes[replacementId];
		replacement->nodes[replacementId] = forDeletion;

		return deleteProcessLeaf(replacement, replacementId);

	}

	return deleteProcessLeaf(node, id);
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
		if (to->children[to->count]) to->children[to->count]->parent = to;
		
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
		if (to->children[0]) to->children[0]->parent = to;
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
		if (to->children[1]) to->children[1]->parent = to;
		if (to->children[2]) to->children[2]->parent = to;

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
		if (to->children[0]) to->children[1]->parent = to;
		if (to->children[1]) to->children[1]->parent = to;
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
	int sibKey = left ? 0 : sibling->count - 1;
	Process* siblingProcess = sibling->extractProcess(sibKey);

	child->insertProcess(parentProcess);
	child->insertProcess(siblingProcess);
	
	if (left) {
		child->children[2] = sibling->children[1];
		child->children[1] = sibling->children[0];
		if (child->children[2]) child->children[2]->parent = child;
		if (child->children[1]) child->children[1]->parent = child;

		for (int i = parentId + 1; i < 3; i++) {
			parent->children[i] = parent->children[i + 1];
		}
		parent->children[3] = nullptr;

	}
	else
	{
		child->children[0] = sibling->children[0];
		child->children[1] = sibling->children[1];
		if (child->children[0]) child->children[0]->parent = child;
		if (child->children[1]) child->children[1]->parent = child;

		for (int i = parentId; i < 3; i++) {
			parent->children[i] = parent->children[i + 1];
		}
		parent->children[3] = nullptr;
	}

	int ind = indexOfChild(parent, sibling);
	if (ind != -1) parent->children[ind] = nullptr;
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
		if (!parent) {
			delete node;
			return p;
		}

		int childIndex = -1;
		for (int i = 0; i < 4; i++) {
			if (parent->children[i] == node) {
				childIndex = i;
				break;
			}
		}

		if (childIndex == -1) {
			cout << "Howw?";
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
	
	bool left = (childIndex % 2) == 0;

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



string RBNode::toString() {
	string output = "";

	for (int i = 0; i < this->count; i++) {
		Process* p = this->nodes[i];
		output += p->getName() + '\n';
		output += "Wait time: " + to_string(p->getWaitingTime()) + '\n';
		output += "Exec time: " + to_string(p->getExecTime()) + '\n';
	}
	
	output.pop_back();

	return output;
}

struct NodePadding {
	RBNode* node;
	int paddingAmount;
	string numbering;


	NodePadding(RBNode* node, int paddingAmount, string numbering) {
		this->node = node;
		this->paddingAmount = paddingAmount;
		this->numbering = numbering;

	}
};

void RBTree::printTreeVertical(ostream& output) {
	NodePadding rootPadding = NodePadding(root, 0, "");
	string outputString = "";
	int tabSize = 5;


	stack<NodePadding> outputStack;
	outputStack.push(rootPadding);

	while (!outputStack.empty()) {
		NodePadding next = outputStack.top();
		outputStack.pop();

		//outputString += next.padding + next.node->toString() + '\n';
		RBNode* node = next.node;

		string outputNode = "";
		for (int i = 0; i < node->count; i++) {
			Process* p = node->nodes[i];
			if (i == 0) {
				outputNode += string(next.paddingAmount, ' ') + next.numbering;
			}
			
			if (next.numbering != "") outputNode += '\n';

			if (node->blackPosition != i && output.rdbuf() == cout.rdbuf()) outputNode += "\033[31m";

			outputNode += string(next.paddingAmount + next.numbering.length(), ' ');

			outputNode += p->getName();

			if (node->blackPosition != i && output.rdbuf() == cout.rdbuf()) outputNode += "\033[0m";

			if (node->blackPosition == i && output.rdbuf() != cout.rdbuf()) outputNode += " (BLK)";

			outputNode += '\n';

			outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Wait time: " + to_string(p->getWaitingTime()) + '\n';

			outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Exec time: " + to_string(p->getExecTime()) + '\n';

			outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Max wait time: " + to_string(p->getMaxWaitTime()) + '\n';

			outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Time to complete: " + to_string(p->getTimeToComplete()) + '\n';

		}

		output << outputNode;

		for (int i = node->count; i >= 0; i--) {
			
			if (!node->children[i]) continue;

			string childNumbering = next.numbering + to_string(i + 1) + '.';
			RBNode* child = node->children[i];

			
			outputStack.push(NodePadding(child, next.paddingAmount + next.numbering.length(), childNumbering));
		}
	}

	//outputString.pop_back();
	//output << outputString;
}

struct NodePaddingRB {
	RBNode* node;
	int paddingAmount;
	string numbering;
	int currentPos;


	NodePaddingRB(RBNode* node, int paddingAmount, string numbering, int currentPos) {
		this->node = node;
		this->paddingAmount = paddingAmount;
		this->numbering = numbering;
		this->currentPos = currentPos;
	}
};

void RBTree::printTreeRedBlack(ostream& output) {
	stack<NodePaddingRB> outputStack;
	outputStack.push(NodePaddingRB(root, 0, "", root->blackPosition));
	string outputString = "";
	int tabSize = 3;

	while(!outputStack.empty()) {
		NodePaddingRB next = outputStack.top();
		outputStack.pop();

		//outputString += next.padding + next.node->toString() + '\n';
		RBNode* node = next.node;

		string outputNode = "";
		
		Process* p = node->nodes[next.currentPos];
		
		outputNode += string(next.paddingAmount, ' ') + next.numbering + ' ';

		if (node->blackPosition != next.currentPos && output.rdbuf() == cout.rdbuf()) outputNode += "\033[31m";

		outputNode += p->getName();

		if (node->blackPosition != next.currentPos && output.rdbuf() == cout.rdbuf()) outputNode += "\033[0m";

		if (node->blackPosition == next.currentPos && output.rdbuf() != cout.rdbuf()) outputNode += " (BLK)";

		outputNode += '\n';

		outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Wait time: " + to_string(p->getWaitingTime()) + '\n';

		outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Exec time: " + to_string(p->getExecTime()) + '\n';

		outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Max wait time: " + to_string(p->getMaxWaitTime()) + '\n';

		outputNode += string(next.paddingAmount + next.numbering.length(), ' ') + "Time to complete: " + to_string(p->getTimeToComplete()) + '\n';
		

		outputString += outputNode;

		
		RBNode* left, *right;
		int leftPos = -1, rightPos = -1;

		if (node->blackPosition == next.currentPos) {
			if (node->blackPosition != 0 && node->nodes[node->blackPosition - 1]) {
				left = node;
				leftPos = node->blackPosition - 1;
			}
			else {
				left = node->children[node->blackPosition];
				if (left) leftPos = left->blackPosition;
			}

			if (node->nodes[node->blackPosition + 1]) {
				right = node;
				rightPos = node->blackPosition + 1;
			}
			else {
				right = node->children[node->blackPosition + 1];
				if (right) rightPos = right->blackPosition;
			}
		}
		else
		{
			left = node->children[next.currentPos];
			if (left) leftPos = left->blackPosition;

			
			right = node->children[next.currentPos + 1];
			if (right) rightPos = right->blackPosition;
			
		}

		string childNumberingLeft = next.numbering + to_string(1) + '.';
		string childNumberingRight = next.numbering + to_string(2) + '.';
		


		if (right) outputStack.push(NodePaddingRB(right, next.paddingAmount + tabSize, childNumberingRight, rightPos));
		if (left) outputStack.push(NodePaddingRB(left, next.paddingAmount + tabSize, childNumberingLeft, leftPos));
		
	}

	output << outputString;
}

bool RBTree::insertFromFile(string filename) {
	ifstream file(filename);


	if (!file.is_open()) return false;

	string row;
	while (getline(file, row)) {
		istringstream iss(row);
		string name;
		Time maxWT, timeToComp;

		iss >> name >> timeToComp >> maxWT;

		Process* p = new Process(name, timeToComp, maxWT);
		this->insertProcess(p);
	}

	file.close();
	return true;
}

RBNode* RBTree::findMin()
{
	if (!root) return nullptr;

	RBNode* curr = root;
	while (!curr->isLeaf()) {
		if (!curr->children[0]) break;

		curr = curr->children[0];
	}
	return curr;
}

void RBTree::updateWaitTimes(Time increment)
{
	/*struct ProcLoc {
		RBNode* node;
		int pos;

		ProcLoc(RBNode* node, int pos) {
			this->node = node;
			this->pos = pos;
		}
	};*/

	if (!root) return;

	vector<Process*> reinserts;
	

	//Update all times;
	queue<RBNode*> Q;
	RBNode* curr = this->root;

	if (curr) Q.push(curr);

	while (!Q.empty()) {
		curr = Q.front();
		Q.pop();

		for (int i = 0; i < curr->count; i++) {
			
			bool addToReinserts = curr->nodes[i]->increaseWaitingTime(increment);
			if (addToReinserts) reinserts.push_back(curr->nodes[i]);
			
		}

		for (int j = 0; j < 4; j++) {
			if (curr->children[j]) Q.push(curr->children[j]);
		}

	}

	//Deal with nodes that need to be readjusted
	while (!reinserts.empty()) {
		Process* toReinsert = reinserts.back();
		reinserts.pop_back();

		//Search process toReinsert
		NodeId nid = searchProcess(toReinsert);
		Process* p; 
		if (nid.node) p = nid.node->nodes[nid.id];
		
		if (nid.node) p = this->removeProcess(nid.node, nid.id);
		else throw exception("Error, process not found");

		
		this->insertProcess(p);
	}
}

Simulator::Simulator()
{
	this->currentProcess = nullptr;
	totalTime = 0;
	timeSlice = 5;
}

void Simulator::executeOneStep(ostream& output) {
	RBTree* tree = RBTree::GetInstance();

	if (tree->Empty()) return;


	RBNode* leftmost = tree->findMin();
	Process* process = tree->removeProcess(leftmost, 0);

	Time inc = timeSlice;
	Time leftover = process->getTimeToComplete() - process->getExecTime();

	if (leftover < timeSlice) inc = leftover;

	Time currWaitTime = process->getWaitingTime();
	Time currExecTime = process->getExecTime();

	process->setWaitingTime(currWaitTime + inc);
	process->setExecTime(currExecTime + inc);

	//Update tree times
	//tree->updateWaitTimes(inc);


	this->totalTime += inc;

	output << "Current simulationn time: " << this->totalTime << '\n';
	output << "Processing:" << process->toString() << '\n';


	
	if (process->getExecTime() < process->getTimeToComplete()) {
		tree->insertProcess(process);
		output << "Status: Unfinished, returned to tree.\n";
		
	}
	else
	{
		//delete process;
		output << "Status: Finished & deleted." << '\n';
	}

	output << "Process tree:\n";

	tree->printTreeVertical(output);
}

void Simulator::executeSimulation(ostream& output) {
	RBTree* tree = RBTree::GetInstance();
	
	while (!tree->Empty()) {
		executeOneStep(output);
	}

	output << "\nSimulataion finished.\nTotal time elapsed: " << this->totalTime << "\n";
}

int main() {
	
	//RBTree* tree = RBTree::GetInstance();
	//Simulator* sim = Simulator::GetInstance();
	//tree->insertFromFile("test2.txt");
	
	/*tree->insertProcess(new Process("8", 8, 2));
	tree->insertProcess(new Process("9", 9, 2));
	tree->insertProcess(new Process("10", 9, 2));*/

	//tree->printTreeVertical(cout);

	//tree->insertProcess(new Process("Novi", 9, 2));

	//cout << node->nodes[0]->getWaitingTime() << " " << node->nodes[1]->getWaitingTime() << " ";
	//ofstream file("output.txt");
	//ofstream file2("deletion.txt");
	//tree->printTreeVertical(file);
	//sim->executeSimulation(file);
	
	//int i = 1;
	//while (!tree->Empty()) {
	//	file2 << "Iteration: " << i;
	//	i++;
	//	RBNode* leftmost = tree->findMin();
	//	file2 << "Process to be removed: " << leftmost->nodes[0]->getName();
	//	tree->removeProcess(leftmost, 0);
	//	//tree->printTreeVertical(file2);
	//}

	//file.close();
	//file2.close();

	//tree->printTreeRedBlack(cout);
	cout << "======MENU======\n"
		"1. Create new tree\n"
		"2. Insert from file\n"
		"3. Insert process\n"
		"4. Search process\n"
		"5. Delete process\n"
		"6. Print tree (Normal)\n"
		"7. Print tree (RB)\n"
		"8. Simulation - One step\n"
		"9. Simulation - Complete\n"
		"0. Exit";
	int input;

	do {
		cout << ">";
		cin >> input;

		switch (input) {
		case 1: {
			RBTree* tree = RBTree::GetInstance();
			cout << "New tree initalized.\n";
			break;
		}
		case 2: {
			string filename;
			cout << "Filename: ";
			cin >> filename;
			
			RBTree::GetInstance()->insertFromFile(filename);
			break;
		}
		case 3: {
			string name;
			Time MWT, TTC;
			cout << "Process name:";
			cin >> name;
			cout << "Max waiting time: ";
			cin >> MWT;
			cout << "Time to complete: ";
			cin >> TTC;
			RBTree::GetInstance()->insertProcess(new Process(name, TTC, MWT));
			break;
		}
		case 4: {
			cout << "Search by:\n"
				"1. Waiting time + Execution time\n"
				"2. Name\n"
				">";
			int choice;
			cin >> choice;
			Process* p;
			switch (choice) {
			case 1: {
				Time wt, et;
				cout << ">";
				cin >> wt >> et;
				p = RBTree::GetInstance()->searchProcess(wt, et);
				break;
			}
			case 2: {
				string name;
				cout << ">";
				cin >> name;
				p = RBTree::GetInstance()->searchProcess(name);
				break;
			}
			}

			cout << p->toString();

			cout << "Edit process?\n"
				"1. Yes\n"
				"2. No\n"
				">";
			cin >> choice;
			switch (choice) {
			case 1: {
				Time et, ttc, mwt;
				string name;
				cout << "Enter new data (Name, MWT, TTC, ET):";
				cin >> name >> mwt >> ttc >> et;
				p->setName(name);
				p->setMaxWaitTime(mwt);
				p->setTimeToComplete(ttc);
				p->setExecTime(et);


				break;
			}
			case 2: {
				
				break;
			}
			}

			break;
		}
		case 5: {
			RBTree* tree = RBTree::GetInstance();
			string name;
			cout << "Name: ";
			cin >> name;
			Process* p = tree->searchProcess(name);
			NodeId nid = tree->searchProcess(p);
			Process* del = tree->removeProcess(nid.node, nid.id);
			delete del;
			cout << "Process removed";
			break;
		}
		case 6:
		{
			string filename;
			cout << "Filename: ";
			cin >> filename;
			ofstream file(filename);
			RBTree::GetInstance()->printTreeVertical(file);

			file.close();
				break;
		}
		case 7:
		{
			string filename;
			cout << "Filename: ";
			cin >> filename;
			ofstream file(filename);
			RBTree::GetInstance()->printTreeRedBlack(file);

			file.close();
			break;
		}
		}

	} while (input != 0);
	
	
	return 0;
}


