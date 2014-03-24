#include "stdlib.h"
#include "stdio.h"
#include "pq.h"
#include "time.h"
#include "assert.h"

typedef enum {MIN=1, MAX=0} heap_type;

typedef struct NODESTRUCT {
    int id;
    int heap_pos;
    double priority;
} NODE;

typedef NODE *nodePtr;

struct pq_struct {
    int size;
    int capacity;
    nodePtr *heap;
    nodePtr *ids;
    heap_type type;
};


/************ Helper Functions ************/

static int is_empty(PQ_PTR pq) {
    if (pq==NULL) return 1;
    return pq->size==0;
}

static int is_not_empty(void *s) {
    return is_empty(s)==0;
}

static int is_null(nodePtr r) {
    return r==NULL;
}

static int is_not_null(nodePtr r) {
    return is_null(r)==0;
}

static int id_out_of_bounds(PQ_PTR pq, int id) {
    if ((id < 0) || (id > pq->capacity)) {
        fprintf(stderr, "ID is out of bounds (0-%d)\n", pq->capacity-1);
        return 1;
    }
    return 0;
}

static int not_a_queue() {
    fprintf(stderr, "Not a queue\n");
    return 0;
}

static double priority(nodePtr r) {
    if (is_null(r)) return -999;
    return r->priority;
}

static nodePtr retrieve_by_id(PQ_PTR pq, int id) {
    return pq->ids[id+1];
}

// Given a node, return its parent
static nodePtr retrieve_parent(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    int parent_pos = r->heap_pos/2;    
    return (parent_pos>0) ? pq->heap[parent_pos] : NULL;
}

static nodePtr retrieve_left_child(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    
    int left_pos = r->heap_pos*2;
    return (left_pos<1 || left_pos>pq->size) ? NULL : pq->heap[left_pos];
}

static nodePtr retrieve_right_child(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    
    int right_pos = r->heap_pos*2+1;
    return (right_pos<1 || right_pos>pq->size) ? NULL : pq->heap[right_pos];
}

static nodePtr retrieve_heap_tail(PQ_PTR pq) {
    if (is_empty(pq)) return NULL;
    return pq->heap[pq->size];
}

static int is_in_heap(PQ_PTR pq, int id) {
    if (is_empty(pq) || id_out_of_bounds(pq,id) || (retrieve_by_id(pq,id)->heap_pos == -1)) return 0;
    return 1;
}

static int is_not_in_heap(PQ_PTR pq, int id) {
    return is_in_heap(pq,id)==0;
}

static void indent(int m) {
    int i;
    
    for(i=0; i<m; i++)
        printf("-");
}

static void preorder(PQ_PTR pq, int margin, int n) {
  if(n>pq->size) {
    // indent(margin);
    // printf("NULL \n");
    return;
  } 
  
    else {
        indent(margin);
        printf("Priority: %f, ID: %d\n", pq->heap[n]->priority, pq->heap[n]->id);
        preorder(pq, margin+3, n*2);
        preorder(pq, margin+3, n*2+1);
    }
}

void pq_preorder(PQ_PTR pq) {
    printf("========BEGIN PREORDER============\n");
    preorder(pq, 0, 1);
    printf("=========END PREORDER============\n");
}


/************ Heap Functions ************/

static nodePtr create_node(int id) {
    nodePtr tmp = malloc (sizeof(NODE));
    tmp->id = id;
    tmp->priority = 0;
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
    if (priority!=NULL) (*priority) = retrieve_by_id(pq,id)->priority;
    return 1;
}

static nodePtr *init_ids(PQ_PTR pq) {
    nodePtr *arr = malloc (sizeof(nodePtr) * (pq->capacity+1));
    
    int i, size=pq->capacity;
    for (i=0; i<size; i++)
        arr[i+1] = create_node(i);

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

static int percolate_up(PQ_PTR pq, nodePtr r) {
    nodePtr parent = retrieve_parent(pq,r);
    
    if ((is_null(parent)) || (is_min_heap(pq) && priority(parent)<=priority(r)) || (is_max_heap(pq) && priority(parent)>=priority(r)))
        return 1;
    
    int parent_pos = parent->heap_pos;
    pq->heap[parent_pos] = r;
    pq->heap[r->heap_pos] = parent;
    
    parent->heap_pos = r->heap_pos;
    r->heap_pos = parent_pos;
    return percolate_up(pq,r);
}

static int percolate_down(PQ_PTR pq, nodePtr r) {
    nodePtr left_child = retrieve_left_child(pq,r);
    nodePtr right_child = retrieve_right_child(pq,r);
    nodePtr swp;
    
    if (is_null(left_child) && is_null(right_child)) 
        return 1;
    
    else if (is_null(left_child) || is_null(right_child)) {
        swp = (is_null(left_child) ? right_child : left_child);
    }
    
    else {
        if (is_min_heap(pq)) 
            swp = (priority(left_child)<priority(right_child) ? left_child : right_child);
        else 
            swp = (priority(left_child)>priority(right_child) ? left_child : right_child);
    }
    
    if ((is_min_heap(pq) && priority(swp)<priority(r)) || (is_max_heap(pq) && priority(swp)>priority(r))) {
        int child_pos = swp->heap_pos;
        pq->heap[child_pos] = r;
        pq->heap[r->heap_pos] = swp;

        swp->heap_pos = r->heap_pos;
        r->heap_pos = child_pos;
        return percolate_down(pq,r);
    }
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

// 
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

void pq_free(PQ_PTR pq) {
    int i, capacity=pq->capacity;
    
    for (i=1; i<=capacity; i++) {
        nodePtr r = retrieve_by_id(pq,i);
        r->id = 0;
        r->priority = 0;
        r->heap_pos = 0;
        free(r);
    }
    
    free(pq->ids);
    free(pq->heap);
    pq->size = 0;
    pq->capacity = 0;
    free(pq);
}

int pq_insert(PQ_PTR pq, int id, double priority) {
    if (heap_contains(pq, id, NULL) || id_out_of_bounds(pq,id)) return 0;
    retrieve_by_id(pq,id)->priority = priority;
    int size = ++(pq->size);
    
    pq->heap[size] = retrieve_by_id(pq,id);
    pq->heap[size]->heap_pos = size;
    percolate_up(pq, pq->heap[size]);
    return 1;
}

int pq_remove_by_id(PQ_PTR pq, int id) {
    if (heap_contains(pq, id, NULL) == 0) return 0;
    
    nodePtr target = retrieve_by_id(pq,id);
    nodePtr replacement = retrieve_heap_tail(pq);
    int old_priority = priority(target);
    int new_priority = priority(replacement);
    int position = target->heap_pos;
    
    target->heap_pos = -1;
    target->priority = 0;
    pq->heap[position] = replacement;
    pq->heap[pq_size(pq)] = NULL;
    replacement->heap_pos = position;
    --(pq->size);
    
    if (is_min_heap(pq)) {
        if (new_priority > old_priority) percolate_down(pq, replacement);
        else percolate_up(pq, replacement);
    }
    else {
        if (new_priority < old_priority) percolate_down(pq, replacement);
        else percolate_down(pq, replacement);
    }
    
    return 1;
}

int pq_delete_top(PQ_PTR pq, int *id, double *priority) {
    if (is_empty(pq)) return 0;
    
    nodePtr target = pq->heap[1];
    if (id!=NULL) *id = target->id;
    if (priority!=NULL) *priority = target->priority;
    target->heap_pos = -1;
    target->priority = 0;
    
    pq->heap[1] = retrieve_heap_tail(pq);
    pq->heap[1]->heap_pos = 1;
    pq->heap[pq_size(pq)] = NULL;
    --(pq->size);
    percolate_down(pq, pq->heap[1]);
}

int pq_change_priority(PQ_PTR pq, int id, double new_priority) {
    if (heap_contains(pq, id, NULL) == 0) return 0;
    
    nodePtr r = retrieve_by_id(pq,id);
    int old_priority = priority(r);
    r->priority = new_priority;
    
    if (new_priority == old_priority) return 1;
    
    if (is_min_heap(pq)) {
        if (new_priority < old_priority) percolate_up(pq,r);
        else percolate_down(pq,r);
    }
    else {
        if (new_priority > old_priority) percolate_up(pq,r);
        else percolate_down(pq,r);
    }
    return 1;
}

int test_heap_associations(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return 1;
    nodePtr left_child = retrieve_left_child(pq,r);
    nodePtr right_child = retrieve_right_child(pq,r);
    
    if (is_not_null(left_child))
        pq->type==MIN ? assert(priority(r) <= priority(left_child)) : assert(priority(r) >= priority(left_child));
    
    test_heap_associations(pq, left_child);
    
    if (is_not_null(right_child))
        pq->type==MIN ? assert(priority(r) <= priority(right_child)) : assert(priority(r) >= priority(right_child));
    
    test_heap_associations(pq, right_child);
}


int main() {
    srand(time(NULL));
    int i, size = 20000;
    PQ_PTR pq = pq_create(size, 0);
    
    for (i=0; i<size; i++) {
        pq_insert(pq, i, rand()%size);
        test_heap_associations(pq, pq->heap[1]);
    }
    
    for (i=0; i<size; i++) {
        pq_delete_top(pq, NULL, NULL);
        test_heap_associations(pq, pq->heap[1]);
    }
    
    pq_free(pq);
    
    return 1;
}