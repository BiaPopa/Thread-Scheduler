#ifndef QUEUE_H_
#define QUEUE_H_

// structura unui nod din lista
typedef struct ll_node_t
{
    void* data;
    int priority;
    struct ll_node_t* next;
} ll_node_t;

// structura unei liste
typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

// creare nod
ll_node_t *new_node(void* data, int data_size, int priority);

// creare lista
linked_list_t *ll_create(unsigned int data_size);

// returneaza nodul de pe pozitia n
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n);

// adauga un element pe pozitia n
void ll_add_node(linked_list_t* list, int n, void* new_data, int priority);

// adauga un element in functie de prioritati
void ll_add_node_priority(linked_list_t* list, void* new_data, int priority);

// sterge nodul de pe pozitia n
ll_node_t *ll_remove_nth_node(linked_list_t* list, unsigned int n);

// returneaza numarul de noduri din lista
unsigned int ll_get_size(linked_list_t* list);

// elibereaza din memorie lista
void ll_free(linked_list_t** pp_list);

// structura unei cozi
typedef struct queue_t
{
	struct linked_list_t *list;
} queue_t;

// creare coada
queue_t *q_create(unsigned int data_size);

// returneaza numarul de noduri din coada
unsigned int q_get_size(queue_t *q);

// returneaza 1 daca coada e goala, 0 altfel
unsigned int q_is_empty(queue_t *q);

// returneaza nodul de pe prima pozitie
void *q_peek(queue_t *q);

// sterge nodul de pe prima pozitie
void q_pop(queue_t *q);

// adauga un element la finalul cozii
void q_push(queue_t *q, void *new_data, int priority);

// adauga un element in functie de prioritate
void q_push_priority(queue_t *q, void *new_data, int priority);

// elibereaza din memorie coada
void q_free(queue_t *q);

#endif