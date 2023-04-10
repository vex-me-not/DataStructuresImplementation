///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Vector μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTVector.h"
#include "ADTMap.h"





// Ένα VectorNode είναι pointer σε αυτό το struct.
struct vector_node {
        ////To vector_node δεν περιεχει τιποτα μιας και θα χρησιμοποιησουμε αποκλειστικα το Map
};

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	Map vector_nodes;  ////Ενα map που εχει την αντιστοιχιση int => Pointer,προσομοιωνοντας τον vector
	int size;          ////Το μεγεθος του vector
	DestroyFunc destroy_value;   ////H συναρτηση καταστροφης που θα καταστρεψει τα στοιχεια του vectorσ
};

static int* create_int(int value) {      ////Συναρτηση που δημιουργει ακεραιους,copy-paste απο προηγουμενα εργαστηρια							
	int* p = malloc(sizeof(int));       ////Static αφου χρησιμοποιειται μονο εντος αυτου module
	*p = value;
	return p;
}

static int compare_ints(Pointer a, Pointer b) {    ////Συναρτηση που συγκρινει ακεραιους,copy-past απο το ADTVector_test.c
	return *(int*)a -   *(int*)b;                     ////Static αφου χρησιμοποιειται μονο εντος αυτου module	
}


Vector vector_create(int size, DestroyFunc destroy_value) {
	
	Vector vec=malloc(sizeof(*vec));                 ////Δεσμευουμε μνημη για το vector
	vec->size=size;                                 ////Το μεγεθος που μας ζητηθηκε
	vec->destroy_value=destroy_value;              ////Η DestroyFunc που μας δοθηκε
	vec->vector_nodes=map_create(compare_ints,free,NULL);     
	
	for(int i =0;i<size;i++){
		map_insert(vec->vector_nodes,create_int(i),NULL);       ////Δημιουργουμε και αρχικοποιουμε με NULL
	}

	return vec;                     /////Επιστρεφουμε το vector
}


int vector_size(Vector vec) {
	vec->size=map_size(vec->vector_nodes);       ////To μεγεθος του vector συμβαδιζει με το μεγεθος του map

	return vec->size;
}


void vector_insert_last(Vector vec, Pointer value) {
	map_insert(vec->vector_nodes,create_int(vec->size),value);      ////Το τελευταιο στοιχειο πριν την εισαγωγη εχει pos size-1,αρα το καινουργιο θα εχει pos size

	vec->size++;                                                    ////Το μεγεθος του vector αυξηθηκε αρα size++
}


void vector_remove_last(Vector vec) {
	int* removed=create_int(vec->size-1);           ////Το τελευταιο στοιχειο εχει pos size -1
	
	if(vec->destroy_value!=NULL){                   ////Aν μας εχει δωθει καποια DestroyFunc στην αρχη
		vec->destroy_value(map_find(vec->vector_nodes,removed));       ////Διαγραφουμε το παλιο value
	}
	
	assert(map_remove(vec->vector_nodes,removed)==true);       ////Διαγραφουμε το key 
	free(removed);                   ////Διαγραφουμε και την "βοηθητικη" θεση

	vec->size--;           ////Το μεγεθος του vector μειωθηκε αρα size--
}


Pointer vector_get_at(Vector vec, int pos) {
	assert(pos >= 0 && pos < vec->size);           ////Πρεπει να βεβαιωθουμε οτι η θεση που μας ζητειται ειναι eligible
	
	int* getter=create_int(pos);                    ////Μιας και δεν θα εισαγουμε αυτο ως Key μας συμφερει να το σωσουμε καπου αλλου ετσι ωστε να μπορουμε να το διαγραψουμε
	Pointer value=map_find(vec->vector_nodes,getter);    ////H τιμη στη θεση που μας ζητηθηκε  
	free(getter);                         ////Διαγραφουμε τη "θεση",μιας και ποτε δεν την εισαγαμε παρα μονο μας βοηθησε για να βρουμε το στοιχειο            

	return value;                         ////Επιστρεφουμε το στοιχειο
}


void vector_set_at(Vector vec, int pos, Pointer value) {
	assert(pos >= 0 && pos < vec->size);    ////Πρεπει να βεβαιωθουμε οτι η θεση που μας ζητειται ειναι eligible
	
	int* removed=create_int(pos);          ////Παρομοια λογικη με την get_at
	if(vec->destroy_value!=NULL){          ////Αν μας εχει δωθει καποια DestroyFunc στην αρχη
		vec->destroy_value(map_find(vec->vector_nodes,removed));      ////Διαγραφουμε το παλιο value μιας και πλεον μας ειναι αχρηστο
	}
	free(removed);
	
	map_insert(vec->vector_nodes,create_int(pos),value);	////Εισαγουμε την καινουργια τιμη στη θεση pos

}


Pointer vector_find(Vector vec, Pointer value, CompareFunc compare) {
	
	for(MapNode mnode=map_first(vec->vector_nodes);mnode!=MAP_EOF;mnode=map_next(vec->vector_nodes,mnode)){ ////Διατρεχουμε το map για να βρουμε το value
		
		Pointer found=map_node_value(vec->vector_nodes,mnode);   ////Το value στο MapNode που βρισκομαστε
		if(compare(found,value)==0){     ////Αν με βαση την compare που μας δοθηκε ειναι ισα
			return value;                ////Επιστρεφουμε το value
		}
	
	}
	
	return NULL;                       ///Αν δεν βρουμε το στοιχειο επιστρεφουμε NULL
}


DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value) {
	DestroyFunc old_destroy_value=vec->destroy_value;      ////Η παλια DestroyFunc
	vec->destroy_value=destroy_value;        ////Αναθετουμε την καινουργια DestroyFunc
	return old_destroy_value;                          ////Επιστρεφουμε την παλια
}


void vector_destroy(Vector vec) {

	if(vec->destroy_value!=NULL){           ////Αν μας εχει δωθει καποια DestroyFunc στην αρχη ή στην πορεια μεσω set_destroy_value
		map_set_destroy_value(vec->vector_nodes,vec->destroy_value);     ////Αναθετουμε αυτη την DestroyFunc στο map για την καταστροφη των values
	}

	map_destroy(vec->vector_nodes);           ////Καλουμε την map_destroy για να καταστραφει το map
	free(vec);                              ////Κανουμε στο ΤΕΛΟΣ free και το vector
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec) {
	if(vec->size==0){        ////Αν εχουμε αδειο vector 
		return VECTOR_BOF;   ///Πρεπει να επιστρεψουμε VECTOR_BOF
	}else{
		int* first=create_int(0);
		MapNode first_node=map_find_node(vec->vector_nodes,first); ////Βρισκουμε ποιο MapNode εχει key==0
		free(first);
		return (VectorNode)first_node;       ////Το κανουμε cast σε VectorNode και το επιστρεφουμε
	}	
}


VectorNode vector_last(Vector vec) {
	if(vec->size==0){     ////Αν εχουμε αδειο vector
		return VECTOR_EOF;  ////Πρεπει να επιστρεψουμε VECTOR_EOF
	}else{
		int* last=create_int(vec->size-1);
		MapNode last_node=map_find_node(vec->vector_nodes,last);      ////Βρισκουμε ποιο MapNode εχει key==size-1
		free(last);
		return (VectorNode)last_node;         ////Το κανουμε cast σε VectorNode και το επιστρεφουμε
	}
}


VectorNode vector_next(Vector vec, VectorNode node) {
	MapNode next_node=(MapNode)node;        ////Κανουμε cast το δωθεν VectorNode σε MapNode
	int node_pos=*(int*)(map_node_key(vec->vector_nodes,next_node));   ////Παιρνουμε τη "θεση" του,δηλαδη κανουμε dereference το key του σε int
	
	if( node_pos == vec->size-1){     ////Αν ειναι το τελευταιο στοιχειο
		return VECTOR_EOF;           ////Δεν εχει επομενο και πρεπει να επιστεψουμε VECTOR_EOF
	}else{
		int* next=create_int(node_pos + 1);     ////Παιρνουμε την επομενη "θεση"
		next_node=map_find_node(vec->vector_nodes,next); //// Και βρισκουμε ποιο MapNode εχει key==next
		free(next);
		return (VectorNode)next_node;         ////Το κανουμε cast σε VectorNode και το επιστρεφουμε
	}
}


VectorNode vector_previous(Vector vec, VectorNode node) {
	MapNode previous_node=(MapNode)node;    ////Κανουμε cast το δωθεν VectorNode σε MapNode
	int node_pos=*(int*)(map_node_key(vec->vector_nodes,previous_node));   ////Παιρνουμε τη "θεση" του,δηλαδη κανουμε dereference το key του σε int
	
	if(node_pos==0){         ////Αν ειναι το πρωτο στοιχειο
		return VECTOR_BOF;   ////Δεν εχει προηγουμε και πρεπει να επιστρεψουμε VECTOR_BOF
	}else{
		int* previous=create_int(node_pos -1);     ////Παιρνουμε την προηγουμενη "θεση"
		previous_node=map_find_node(vec->vector_nodes,previous); ////Και βρισκουμε ποιο MapNode εχει key==previous
		free(previous);
		return (VectorNode)previous_node;	////Το κανουμε cast σε VectorNode και το επιστρεφουμε
	}
}


Pointer vector_node_value(Vector vec, VectorNode node) {
	MapNode mnode=(MapNode)node;    ////Κανουμε cast το δωθεν VectorNode σε MapNode      
	return map_node_value(vec->vector_nodes,mnode);   ////Και επιστρεφουμε το value του
}


VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare) {

	for(MapNode mnode=map_first(vec->vector_nodes);mnode !=MAP_EOF;mnode=map_next(vec->vector_nodes,mnode)){    ////Διατρεχουμε το map/vector
		Pointer found=map_node_value(vec->vector_nodes,mnode);   ////Το value στο MapNode που βρισκομαστε		
		
		if(compare(found,value)==0){       ////Aν με βση την compare ειναι ισα
			return (VectorNode)mnode;     ////Κανουμε cast το MapNode που βρηκαμε σε VectorNode και το επιστρεφουμε
		}
	
	}	

	return VECTOR_EOF;               ////Αν το value αυτο δεν υπαρχει στο map/vector πρεπει να επιστρεψουμε VECTOR_EOF
}