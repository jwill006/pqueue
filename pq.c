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

// Returns 1 if priority queue is NULL or empty -- O(1)
static int is_empty(PQ_PTR pq) {
    if (pq==NULL) return 1;
    return pq->size==0;
}

// Returns 1 if priority queue is not empty -- O(1)
static int is_not_empty(void *s) {
    return is_empty(s)==0;
}

// Returns 1 if node is NULL -- O(1)
static int is_null(nodePtr r) {
    return r==NULL;
}

// Returns 1 if node is not NULL -- O(1)
static int is_not_null(nodePtr r) {
    return is_null(r)==0;
}

// Prints an error message and returns 0 -- O(1)
static int id_out_of_bounds(PQ_PTR pq, int id) {
    if ((id < 0) || (id > pq->capacity)) {
        fprintf(stderr, "ID is out of bounds (0-%d)\n", pq->capacity-1);
        return 1;
    }
    return 0;
}

// Prints an error message and returns 0 -- O(1)
static int not_a_queue() {
    fprintf(stderr, "Not a queue\n");
    return 0;
}

// Returns priority of a node -- O(1)
static double priority(nodePtr r) {
    if (is_null(r)) return -999; // Should never happen
    return r->priority;
}

// Returns node from id index -- O(1)
static nodePtr retrieve_by_id(PQ_PTR pq, int id) {
    return pq->ids[id+1];
}

// Returns parent node or NULL -- O(1)
static nodePtr retrieve_parent(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    int parent_pos = r->heap_pos/2;    
    return (parent_pos>0) ? pq->heap[parent_pos] : NULL;
}

// Returns left child or NULL -- O(1)
static nodePtr retrieve_left_child(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    
    int left_pos = r->heap_pos*2;
    return (left_pos<1 || left_pos>pq->size) ? NULL : pq->heap[left_pos];
}

// Returns right child or NULL -- O(1)
static nodePtr retrieve_right_child(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return NULL;
    
    int right_pos = r->heap_pos*2+1;
    return (right_pos<1 || right_pos>pq->size) ? NULL : pq->heap[right_pos];
}

// Returns last element in priority queue array -- O(1)
static nodePtr retrieve_heap_tail(PQ_PTR pq) {
    if (is_empty(pq)) return NULL;
    return pq->heap[pq->size];
}

// Returns 1 if an id is in the priority queue -- O(1)
static int is_in_heap(PQ_PTR pq, int id) {
    if (is_empty(pq) || id_out_of_bounds(pq,id) || (retrieve_by_id(pq,id)->heap_pos == -1)) return 0;
    return 1;
}

// Returns 1 if an id is not in the priority queue -- O(1)
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

// Returns a new node with given ID -- O(1)
static nodePtr create_node(int id) {
    nodePtr tmp = malloc (sizeof(NODE));
    tmp->id = id;
    tmp->priority = 0;
    tmp->heap_pos = -1;
    return tmp;
}

// Returns 1 priority queue is a MIN heap -- O(1)
static int is_min_heap(PQ_PTR pq) {
    return pq->type==MIN;
}

// Returns 1 priority queue is a MAX heap -- O(1)
static int is_max_heap(PQ_PTR pq) {
    return pq->type==MAX;
}

// Returns 1 if a node with ID is in priority queue -- O(1)
static int heap_contains(PQ_PTR pq, int id, double *priority) {
    if (is_not_in_heap(pq,id)) return 0;
    if (priority!=NULL) (*priority) = retrieve_by_id(pq,id)->priority;
    return 1;
}

// Returns address of array with initialized nodes -- O(N)
static nodePtr *init_ids(PQ_PTR pq) {
    nodePtr *arr = malloc (sizeof(nodePtr) * (pq->capacity+1));
    
    int i, size=pq->capacity;
    for (i=0; i<size; i++)
        arr[i+1] = create_node(i);

    arr[0] = NULL;
    return arr;
}

// Returns address of array for heap implementation -- O(N)
static nodePtr *init_heap(PQ_PTR pq) {
    nodePtr *arr = malloc (sizeof(nodePtr) * (pq->capacity+1));
    
    int i, size=pq->capacity;
    for (i=0; i<size; i++)
        arr[i+1] = NULL;
    
    arr[0] = NULL;
    return arr;
}

// Validates node-parent relation is consistent with heap properities -- O(logN)
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

// Validates node-children relations are consistent with heap properities -- O(logN)
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

// Returns capacity of priority queue -- O(1)
int pq_capacity(PQ_PTR pq) {
    return is_empty(pq) ? 0 : pq->capacity;
}

// Returns current size of priority queue -- O(1)
int pq_size(PQ_PTR pq) {
    return is_empty(pq) ? 0 : pq->size;
}

// Returns 1 if process with ID is in priority queue -- O(1)
int pq_contains(PQ_PTR pq, int id) {
    return heap_contains(pq, id, NULL);
}

// Returns priority of a process with ID -- O(1)
int pq_get_priority(PQ_PTR pq, int id, double *priority) {
    return heap_contains(pq, id, priority);
}

// Returns a pointer to a newly initialized priority queue -- O(N)
PQ_PTR pq_create(int capacity, int min_heap) {
    PQ_PTR pq = malloc (sizeof(struct pq_struct));
    pq->capacity = capacity;
    pq->type = (min_heap ? MIN : MAX);
    pq->size = 0;
    pq->heap = init_heap(pq); // -- O(N)
    pq->ids = init_ids(pq);   // -- O(N)
    return pq;
}

// Deallocates priority queue -- O(1)
void pq_free(PQ_PTR pq) {
    int i, capacity=pq->capacity;
    
    for (i=0; i<capacity; i++) {
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

// Insert process with ID and PRIORITY into priority queue -- O(logN)
int pq_insert(PQ_PTR pq, int id, double priority) {
    if (heap_contains(pq, id, NULL) || id_out_of_bounds(pq,id)) return 0;  // -- O(1)
    retrieve_by_id(pq,id)->priority = priority;
    int size = ++(pq->size);
    
    pq->heap[size] = retrieve_by_id(pq,id); // -- O(1)
    pq->heap[size]->heap_pos = size;
    percolate_up(pq, pq->heap[size]);       // -- O(logN)
    return 1;
}

// Removes a process with ID from priority queue -- O(logN)
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
        if (new_priority > old_priority) percolate_down(pq, replacement); // -- O(logN)
        else percolate_up(pq, replacement); // -- O(logN)
    }
    else {
        if (new_priority < old_priority) percolate_down(pq, replacement); // -- O(logN)
        else percolate_up(pq, replacement); // -- O(logN)
    }
    
    return 1;
}

// Removes the process with the highest priority in the queue -- O(logN)
int pq_delete_top(PQ_PTR pq, int *id, double *priority) {
    if (is_empty(pq)) return 0;
    return pq_remove_by_id(pq, pq->heap[1]->id);
}

// Updates the priority of process with ID -- O(logN)
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

// Validates heap from top-down
int test_heap_associations_helper(PQ_PTR pq, nodePtr r) {
    if (is_null(r)) return 1;
    nodePtr left_child = retrieve_left_child(pq,r);
    nodePtr right_child = retrieve_right_child(pq,r);
    
    if (is_not_null(left_child))
        pq->type==MIN ? assert(priority(r) <= priority(left_child)) : assert(priority(r) >= priority(left_child));
    
    test_heap_associations_helper(pq, left_child);
    
    if (is_not_null(right_child))
        pq->type==MIN ? assert(priority(r) <= priority(right_child)) : assert(priority(r) >= priority(right_child));
    
    test_heap_associations_helper(pq, right_child);
}

// Random test generator for priority queue
int test_heap_associations() {
    srand(time(NULL));
    int i, size = 1000, psize = size*5;
    PQ_PTR pq = pq_create(size, 1);
    
    for (i=0; i<size; i++) {
        pq_insert(pq, i, rand()%psize);
        test_heap_associations_helper(pq, pq->heap[1]);
    }
    
    for (i=size-1; i>=0; i--) {
        pq_remove_by_id(pq,i);
        test_heap_associations_helper(pq, pq->heap[1]);
    }
    
    pq_free(pq);
    
    return 1;
}

int main () {
    test_heap_associations();
    return 1;
}