#include "list.h"

void _listnode_init(ListNode *node)
{
	node->prev = node->next = 0;
}

void _list_add(ListNode *list, ListNode *node)
{
	ListNode *tail = list;
	while (tail->next != 0)
		tail = tail->next;
	node->prev = tail;
	tail->next = node;
}

void _list_remove(ListNode *node)
{
	ListNode *prev = node->prev;
	ListNode *next = node->next;
	if (prev != 0)
		prev->next = next;
	if (next != 0)
		next->prev = prev;
	node->prev = 0;
	node->next = 0;
}

void _list_foreach(ListNode *list, list_callback callback)
{
	ListNode *node = list;
	while (node)
	{
		ListNode *next = node->next;
		callback(node);
		node = next;
	}
}

ListNode *_list_find(ListNode *list, list_callback_criteria callback)
{
	ListNode *node = list;
	while (node && !callback(node))
		node = node->next;
	return node;
}

unsigned _list_len(ListNode *list)
{
	unsigned len;
	for (len = 0; list != 0; ++len, list = list->next);
	return len;
}
