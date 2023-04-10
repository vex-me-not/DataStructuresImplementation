
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////             Η υλοποιση του ADTDeque.h                                                              //////////////////////////////////////////////////
//////////////                                                                                                    //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#include <stdlib.h>
#include <assert.h>

#include "ADTDeque.h"
#include "ADTMap.h"


// Ένα DequeNode είναι pointer σε αυτό το struct.
struct deque_node {
        ////To vector_node δεν περιεχει τιποτα μιας και θα χρησιμοποιησουμε αποκλειστικα το Map
};


// Ενα Deque είναι pointer σε αυτό το struct
struct deque {
	Map deq_nodes;  ////Ενα map που εχει την αντιστοιχιση int => Pointer,προσομοιωνοντας τον vector
	int size;          ////Το μεγεθος του vector
	int front_size;    ////Τα στοιχεια "πριν" το 0
    DestroyFunc destroy_value;   ////H συναρτηση καταστροφης που θα καταστρεψει τα στοιχεια του vectorσ
};


static int* create_int(int value) {      ////Συναρτηση που δημιουργει ακεραιους,copy-paste απο προηγουμενα εργαστηρια							
	int* p = malloc(sizeof(int));       ////Static αφου χρησιμοποιειται μονο εντος αυτου module
	*p = value;
	return p;
}

static int compare_ints(Pointer a, Pointer b) {    ////Συναρτηση που συγκρινει ακεραιους,copy-past απο το ADTVector_test.c
	return *(int*)a - *(int*)b;                     ////Static αφου χρησιμοποιειται μονο εντος αυτου module	
}



Deque deque_create(int size, DestroyFunc destroy_value) {
	
	Deque deq=malloc(sizeof(*deq));                 ////Δεσμευουμε μνημη για το deque
	deq->front_size=0;
    deq->size=size;                                 ////Το μεγεθος που μας ζητηθηκε
	deq->destroy_value=destroy_value;              ////Η DestroyFunc που μας δοθηκε
	deq->deq_nodes=map_create(compare_ints,free,NULL);     
	
	for(int i =0;i<size;i++){
		map_insert(deq->deq_nodes,create_int(i),NULL);       ////Δημιουργουμε και αρχικοποιουμε με NULL
	}

	return deq;                     /////Επιστρεφουμε το vector
}

int deque_size(Deque deque){
    deque->size=map_size(deque->deq_nodes);     ////Το size του deque πρεπει να συμπιπτει με το size του map deq_nodes
    return deque->size;                       ////Αρα το επιστρεφουμε
}


void deque_insert_first(Deque deq, Pointer value){
    deq->front_size++;
    map_insert(deq->deq_nodes,create_int(-1*deq->front_size),value);
    deq->size++;
}


void deque_insert_last(Deque deq, Pointer value) {
	
	map_insert(deq->deq_nodes,create_int(deq->size-deq->front_size),value);      ////Το τελευταιο στοιχειο πριν την εισαγωγη εχει pos size-1,αρα το καινουργιο θα εχει pos size
	deq->size++;                                                    ////Το μεγεθος του deque αυξηθηκε αρα size++
}


void deque_remove_first(Deque deq){
	int* removed=create_int(-1*deq->front_size);           ////Το πρωτο στοιχειο εχει pos size -1
	
	if(deq->destroy_value!=NULL){                   ////Aν μας εχει δωθει καποια DestroyFunc στην αρχη
		deq->destroy_value(map_find(deq->deq_nodes,removed));       ////Διαγραφουμε το παλιο value
	}
	
	assert(map_remove(deq->deq_nodes,removed)==true);       ////Διαγραφουμε το key 
	free(removed);                   ////Διαγραφουμε και την "βοηθητικη" θεση

    deq->front_size--;      ////Εφυγε ενα στοιχειο απο μπροστα
	deq->size--;           ////Το μεγεθος του deque μειωθηκε αρα size--    
}


void deque_remove_last(Deque deq) {
	int* removed=create_int((deq->size-1)-deq->front_size);           ////Το τελευταιο στοιχειο εχει pos size -1
	
	if(deq->destroy_value!=NULL){                   ////Aν μας εχει δωθει καποια DestroyFunc στην αρχη
		deq->destroy_value(map_find(deq->deq_nodes,removed));       ////Διαγραφουμε το παλιο value
	}
	
	assert(map_remove(deq->deq_nodes,removed)==true);       ////Διαγραφουμε το key 
	free(removed);                   ////Διαγραφουμε και την "βοηθητικη" θεση

	deq->size--;           ////Το μεγεθος του deque μειωθηκε αρα size--
}


Pointer deque_get_at(Deque deq, int pos) {
	assert(pos >= 0 && pos < deq->size);           ////Πρεπει να βεβαιωθουμε οτι η θεση που μας ζητειται ειναι eligible
	
	int* getter=create_int(pos-deq->front_size);                    ////Μιας και δεν θα εισαγουμε αυτο ως Key μας συμφερει να το σωσουμε καπου αλλου ετσι ωστε να μπορουμε να το διαγραψουμε
	Pointer value=map_find(deq->deq_nodes,getter);    ////H τιμη στη θεση που μας ζητηθηκε  
	free(getter);                         ////Διαγραφουμε τη "θεση",μιας και ποτε δεν την εισαγαμε παρα μονο μας βοηθησε για να βρουμε το στοιχειο            

	return value;                         ////Επιστρεφουμε το στοιχειο
}


void deque_set_at(Deque deq, int pos, Pointer value) {
	assert(pos >= 0 && pos < deq->size);    ////Πρεπει να βεβαιωθουμε οτι η θεση που μας ζητειται ειναι eligible
	
	int* removed=create_int(pos - deq->front_size);          ////Παρομοια λογικη με την get_at
	if(deq->destroy_value!=NULL){          ////Αν μας εχει δωθει καποια DestroyFunc στην αρχη
		deq->destroy_value(map_find(deq->deq_nodes,removed));      ////Διαγραφουμε το παλιο value μιας και πλεον μας ειναι αχρηστο
	}
	free(removed);
	
	map_insert(deq->deq_nodes,create_int(pos - deq->front_size),value);	////Εισαγουμε την καινουργια τιμη στη θεση pos
}


Pointer deque_find(Deque deq, Pointer value, CompareFunc compare) {
	
	for(MapNode mnode=map_first(deq->deq_nodes);mnode!=MAP_EOF;mnode=map_next(deq->deq_nodes,mnode)){ ////Διατρεχουμε το map για να βρουμε το value
		
		Pointer found=map_node_value(deq->deq_nodes,mnode);   ////Το value στο MapNode που βρισκομαστε
		if(compare(found,value)==0){     ////Αν με βαση την compare που μας δοθηκε ειναι ισα
			return value;                ////Επιστρεφουμε το value
		}
	
	}
	
	return NULL;                       ///Αν δεν βρουμε το στοιχειο επιστρεφουμε NULL
}


DestroyFunc deque_set_destroy_value(Deque deq, DestroyFunc destroy_value) {
	DestroyFunc old_destroy_value=deq->destroy_value;      ////Η παλια DestroyFunc
	deq->destroy_value=destroy_value;        ////Αναθετουμε την καινουργια DestroyFunc
	return old_destroy_value;                          ////Επιστρεφουμε την παλια
}


void deque_destroy(Deque deq) {

	if(deq->destroy_value!=NULL){           ////Αν μας εχει δωθει καποια DestroyFunc στην αρχη ή στην πορεια μεσω set_destroy_value
		map_set_destroy_value(deq->deq_nodes,deq->destroy_value);     ////Αναθετουμε αυτη την DestroyFunc στο map για την καταστροφη των values
	}

	map_destroy(deq->deq_nodes);           ////Καλουμε την map_destroy για να καταστραφει το map
	free(deq);                              ////Κανουμε στο ΤΕΛΟΣ free και το vector
}


DequeNode deque_first(Deque deq) {
	if(deq->size==0){        ////Αν εχουμε αδειο deque
		return DEQUE_BOF;   ///Πρεπει να επιστρεψουμε DEQUE_BOF
	}else{
		int* first=create_int(-1*deq->front_size);
		MapNode first_node=map_find_node(deq->deq_nodes,first); ////Βρισκουμε ποιο MapNode εχει key==1*deq->front_size
		free(first);
		return (DequeNode)first_node;       ////Το κανουμε cast σε VectorNode και το επιστρεφουμε
	}	
}


DequeNode deque_last(Deque deq) {
	if(deq->size==0){     ////Αν εχουμε αδειο deque
		return DEQUE_EOF;  ////Πρεπει να επιστρεψουμε DEQUE_EOF
	}else{
		int* last=create_int(deq->size-1);
		MapNode last_node=map_find_node(deq->deq_nodes,last);      ////Βρισκουμε ποιο MapNode εχει key==size-1
		free(last);
		return (DequeNode)last_node;         ////Το κανουμε cast σε DequeNode και το επιστρεφουμε
	}
}


DequeNode deque_next(Deque deq, DequeNode node) {
	MapNode next_node=(MapNode)node;        ////Κανουμε cast το δωθεν DeqeuNode σε MapNode
	int node_pos=*(int*)(map_node_key(deq->deq_nodes,next_node));   ////Παιρνουμε τη "θεση" του,δηλαδη κανουμε dereference το key του σε int
	
	if( node_pos == deq->size-1){     ////Αν ειναι το τελευταιο στοιχειο
		return DEQUE_EOF;           ////Δεν εχει επομενο και πρεπει να επιστεψουμε DEQUE_EOF
	}else{
		int* next=create_int(node_pos + 1);     ////Παιρνουμε την επομενη "θεση"
		next_node=map_find_node(deq->deq_nodes,next); //// Και βρισκουμε ποιο MapNode εχει key==next
		free(next);
		return (DequeNode)next_node;         ////Το κανουμε cast σε DequeNode και το επιστρεφουμε
	}
}


DequeNode deque_previous(Deque deq, DequeNode node) {
	MapNode previous_node=(MapNode)node;    ////Κανουμε cast το δωθεν DequeNode σε MapNode
	int node_pos=*(int*)(map_node_key(deq->deq_nodes,previous_node));   ////Παιρνουμε τη "θεση" του,δηλαδη κανουμε dereference το key του σε int
	
	if(node_pos==(-1 * deq->front_size)){         ////Αν ειναι το πρωτο στοιχειο
		return DEQUE_BOF;   ////Δεν εχει προηγουμενο και πρεπει να επιστρεψουμε DEQUE_BOF
	}else{
		int* previous=create_int(node_pos -1);     ////Παιρνουμε την προηγουμενη "θεση"
		previous_node=map_find_node(deq->deq_nodes,previous); ////Και βρισκουμε ποιο MapNode εχει key==previous
		free(previous);
		return (DequeNode)previous_node;	////Το κανουμε cast σε DequeNode και το επιστρεφουμε
	}
}


Pointer deque_node_value(Deque deq, DequeNode node) {
	MapNode mnode=(MapNode)node;    ////Κανουμε cast το δωθεν DequeNode σε MapNode      
	return map_node_value(deq->deq_nodes,mnode);   ////Και επιστρεφουμε το value του
}


DequeNode deque_find_node(Deque deq, Pointer value, CompareFunc compare) {

	for(MapNode mnode=map_first(deq->deq_nodes);mnode !=MAP_EOF;mnode=map_next(deq->deq_nodes,mnode)){    ////Διατρεχουμε το map/deque
		Pointer found=map_node_value(deq->deq_nodes,mnode);   ////Το value στο MapNode που βρισκομαστε		
		
		if(compare(found,value)==0){       ////Aν με βαση την compare ειναι ισα
			return (DequeNode)mnode;     ////Κανουμε cast το MapNode που βρηκαμε σε DequeNode και το επιστρεφουμε
		}
	
	}	

	return DEQUE_EOF;               ////Αν το value αυτο δεν υπαρχει στο map/deque πρεπει να επιστρεψουμε DEQUE_EOF
}