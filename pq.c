#include "stdlib.h"
#include "stdio.h"
#include "pq.h"

typedef enum {MIN, MAX} heap_type;

typedef struct NODESTRUCT {
    int id;
    int heap_pos;
    double priority;
} NODE;

typedef NODE* nodePtr;

struct pq_struct {
    int size;
    int capacity;
    nodePtr *heap;
    nodePtr *ids;
    heap_type type;
};

/************ Helper Functions ************/

static int is_empty(void *s) {
    if (s==NULL) return 1;
    if (sizeof(*s) == sizeof(struct pq_struct))
        return ((PQ_PTR) s)->size==0;    
}

static int is_not_empty(void *s) {
    return is_empty(s)==0;
}

// Given a node, return its parent
static nodePtr parent(PQ_PTR pq, nodePtr r) {
    if (is_empty(r)) return NULL;
    int parent_pos = r->heap_pos/2;
    
    return (parent_pos<1) ? NULL : pq->heap[parent_pos];
}

static nodePtr left_child(PQ_PTR pq, nodePtr r) {
    if (is_empty(r)) return NULL;
    
    int left_pos = r->heap_pos*2;
    return (left_pos<1 || left_pos>pq->size) ? NULL : pq->heap[left_pos];
}

static nodePtr right_child(PQ_PTR pq, nodePtr r) {
    if (is_empty(r)) return NULL;
    
    int right_pos = r->heap_pos*2+1;
    return (right_pos<1 || right_pos>pq->size) ? NULL : pq->heap[right_pos];
}

static int is_in_heap(PQ_PTR pq, int id) {
    if (is_empty(pq) || (id < 0) || (id > pq->capacity)) return 0;
    if (pq->ids[id+1]->heap_pos != -1) return 1;
    return 0;
}

static int is_not_in_heap(PQ_PTR pq, int id) {
    return is_in_heap(pq,id)==0;
}


/************ Heap Functions ************/

static nodePtr create_node(int id, double priority) {
    nodePtr tmp = malloc (sizeof(NODE));
    tmp->id = id;
    tmp->priority = priority;
    tmp->heap_pos = -1;
    return tmp;
}

static int is_min_heap(PQ_PTR pq) {
    return pq->type==MIN;
}

static int is_max_heap(PQ_PTR pq) {
    return pq->type==MAX;
}

static int heap_contains(PQ_PTR pq, int id, double *priority) {
    if (is_not_in_heap(pq,id)) return 0;
    if (is_not_empty(priority)) (*priority) = pq->ids[id+1]->priority;
    return 1;
}

static nodePtr *init_ids(PQ_PTR pq) {
    nodePtr *arr = malloc (sizeof(nodePtr) * (pq->capacity+1));
    
    int i, size=pq->capacity;
    for (i=0; i<size; i++)
        arr[i+1] = create_node(i, 0);

    arr[0] = NULL;
    return arr;
}

static nodePtr *init_heap(PQ_PTR pq) {
    nodePtr *arr = malloc (sizeof(nodePtr) * (pq->capacity+1));
    
    int i, size=pq->capacity;
    for (i=0; i<size; i++)
        arr[i+1] = NULL;
    
    arr[0] = NULL;
    return arr;
}

static int percolate_up(PQ_PTR pq) {
    return 1;
}

static int percolate_down(PQ_PTR pq) {
    return 1;
}


/************ Required Functions ************/

int pq_capacity(PQ_PTR pq) {
    return is_empty(pq) ? 0 : pq->capacity;
}

int pq_size(PQ_PTR pq) {
    return is_empty(pq) ? 0 : pq->size;
}

int pq_contains(PQ_PTR pq, int id) {
    return heap_contains(pq, id, NULL);
}

int pq_get_priority(PQ_PTR pq, int id, double *priority) {
    return heap_contains(pq, id, priority);
}

// Returns a pointer to a newly initialized max/min heap
PQ_PTR pq_create(int capacity, int min_heap) {
    PQ_PTR pq = malloc (sizeof(struct pq_struct));
    pq->capacity = capacity;
    pq->type = (min_heap ? MIN : MAX);
    pq->size = 0;
    pq->heap = init_heap(pq);    
    pq->ids = init_ids(pq);
    return pq;
}

int pq_insert(PQ_PTR pq, int id, double priority) {
    if (heap_contains(pq, id, NULL)) return 0;
    pq->ids[id]->priority = priority;
    
    // ADD to tree
    
    // Validate tree
    
    return 1;
}

int pq_change_priority(PQ_PTR pq, int id, double new_priority) {
    if (heap_contains(pq, id, NULL) == 0) return 0;
    
    nodePtr n = pq->ids[id+1];
    int old_priority = n->priority;
    n->priority = new_priority;
    
    if (new_priority == old_priority) return 1;
    
    if (is_min_heap(pq)) {
        if (new_priority < old_priority) percolate_up(pq);
        else percolate_down(pq);
    }
    else {
        if (new_priority > old_priority) percolate_up(pq);
        else percolate_down(pq);
    }
    return 1;
}


int main() {
    return 1;
}