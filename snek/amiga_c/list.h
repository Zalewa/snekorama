#ifndef idd3591841_d957_4b04_8299_1da673873d4f
#define idd3591841_d957_4b04_8299_1da673873d4f

#define LIST_NODE \
	ListNode *next; \
	ListNode *prev;

typedef struct _ListNode ListNode;
typedef void(*list_callback)(ListNode*);
typedef int(*list_callback_criteria)(ListNode*);

struct _ListNode
{
	LIST_NODE
};

void _listnode_init(ListNode *node);
#define listnode_init(node) \
	_listnode_init((ListNode*) node)

void _list_add(ListNode *list, ListNode *node);
#define list_add(list, node) \
	_list_add((ListNode*) list, (ListNode*) node)

void _list_remove(ListNode *node);
#define list_remove(node) \
	_list_remove((ListNode*) node)

/** List contents should not be modified in the callback. */
#define list_foreach(list, callback) \
	_list_foreach((ListNode*)(list), (list_callback)(callback))
void _list_foreach(ListNode *list, list_callback);

#define list_find(list, callback_criteria) \
	_list_find((ListNode*)(list), (list_callback_criteria)(callback_criteria))
ListNode *_list_find(ListNode *list, list_callback_criteria);

unsigned _list_len(ListNode *list);
#define list_len(list) \
	_list_len((ListNode*)(list))


#endif
