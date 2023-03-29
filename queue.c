#include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "struct.h"

ll_node_t *new_node(void *data, int data_size, int priority)
{
    struct ll_node_t *node = malloc(sizeof(struct ll_node_t));

    node->next = NULL;
    node->priority = priority;
    node->data = data;
    return node;
}

linked_list_t *ll_create(unsigned int data_size)
{
    struct linked_list_t *list = malloc(sizeof(struct linked_list_t));

    list->head = NULL;
    list->size = 0;
    // data_size este sizeof(thread_t)
    list->data_size = data_size;

    return list;
}

ll_node_t *get_nth_node(linked_list_t *list, unsigned int n)
{
    struct ll_node_t *temp = list->head;

    unsigned int i = 0;

    while (temp != NULL) {
	if (i == n)
	    break;
	++i;
	temp = temp->next;
    }

    return temp;
}

void ll_add_node(linked_list_t *list, int n, void *new_data, int priority)
{
    // se creeaza un nou nod
    struct ll_node_t *node = new_node(new_data, list->data_size, priority);

    if (n == 0) {
	// daca n e 0, atunci nodul creat devine headul listei
	node->next = list->head;
	list->head = node;

    } else {
	// daca nu, se cauta pozitia n pe care este adaugat nodul creat
	struct ll_node_t *temp = list->head;

	if (n > list->size)
	    n = list->size;

	while (--n) {
	    temp = temp->next;
	}

	node->next = temp->next;
	temp->next = node;
    }

    ++list->size;
}

void ll_add_node_priority(linked_list_t *list, void *new_data, int priority)
{
    // se creeaza un nou nod
    struct ll_node_t *node = new_node(new_data, list->data_size, priority);

    if (list->size == 0) {
	// daca lista e goala, nodul creat devine headul listei
	node->next = list->head;
	list->head = node;

    } else if (list->head->priority < priority) {
	// daca prioritatea nodului creat este mai mare decat a headului
	// listei, atunci nodul creat devine headul listei
	node->next = list->head;
	list->head = node;

    } else {
	// se cauta pozitia pe care se va adauga nodul creat
	struct ll_node_t *temp = list->head;

	while (temp->next != NULL && temp->next->priority >= priority) {
	    temp = temp->next;
	}

	node->next = temp->next;
	temp->next = node;
    }

    ++list->size;
}

ll_node_t *ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
    if (list->head == NULL)
	return list->head;

    struct ll_node_t *temp = list->head;

    if (n == 0) {
	// daca n e 0, se sterge headul listei
	list->head = temp->next;
	--list->size;
	return temp;

    }

    // se cauta elementul de pe pozitia n
    if (n > list->size)
	n = list->size - 1;

    for (int i = 0; i < n - 1; ++i)
	temp = temp->next;

    struct ll_node_t *next = temp->next->next;
    struct ll_node_t *rez = temp->next;

    temp->next = next;
    --list->size;
    return rez;
}

unsigned int ll_get_size(linked_list_t *list)
{
    if (list == NULL) {
	return -1;
    }

    // numarul de noduri din lista
    return list->size;
}

void ll_free(linked_list_t **pp_list)
{
    struct ll_node_t *temp;

    // se elibereaza din memorie toate nodurile
    while ((*pp_list)->head != NULL) {
	temp = (*pp_list)->head;
	(*pp_list)->head = (*pp_list)->head->next;

	free(temp->data);
	free(temp);
    }

    // se elibereaza lista
    free(*pp_list);
}

queue_t *q_create(unsigned int data_size)
{
    struct queue_t *q = malloc(sizeof(struct queue_t));

    q->list = ll_create(data_size);

    return q;
}

unsigned int q_get_size(queue_t *q)
{
    if (q->list == NULL)
	return -1;

    // numarul de noduri din coada
    return ll_get_size(q->list);
}

unsigned int q_is_empty(queue_t *q)
{
    if (q_get_size(q) == 0) {
	return 1;
    }

	return 0;
}

void *q_peek(queue_t *q)
{
	return q->list->head;
}

void q_pop(queue_t *q)
{
    struct ll_node_t *del = ll_remove_nth_node(q->list, 0);

    free(del);
}

void q_push(queue_t *q, void *new_data, int priority)
{
    ll_add_node(q->list, q_get_size(q), new_data, priority);
}

void q_push_priority(queue_t *q, void *new_data, int priority)
{
    ll_add_node_priority(q->list, new_data, priority);
}

void q_free(queue_t *q)
{
    ll_free(&(q->list));
    free(q);
}
