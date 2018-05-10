#ifndef DATANODE_H
#define DATANODE_H


class DataNode {

private:
	double elem;
	DataNode* next;

	friend class Classifier;
};


#endif