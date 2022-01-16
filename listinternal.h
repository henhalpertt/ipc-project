#ifndef __LISTINTERNAL_H__
#define __LISTINTERNAL_H__

typedef struct Node Node;

struct Node
{
	void* m_data;
	Node* m_next;
	Node* m_prev;
};

struct List
{
	Node m_head;
	Node m_tail;
};

void Insert(Node* _p, Node* _t);
void* Remove(Node* _t);


#endif /* __LISTINTERNAL_H__ */

