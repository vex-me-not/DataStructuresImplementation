
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "ADTDeque.h"

// Το αρχικό μέγεθος που δεσμεύουμε
#define DEQUE_MIN_CAPACITY 10

// Ένα DequeNode είναι pointer σε αυτό το struct. (το struct περιέχει μόνο ένα στοιχείο, οπότε θα μπροούσαμε και να το αποφύγουμε, αλλά κάνει τον κώδικα απλούστερο)
struct deque_node {
	Pointer value;				// Η τιμή του κόμβου.
};

// Ενα Deque είναι pointer σε αυτό το struct
struct deque {
	DequeNode array;			// Τα δεδομένα, πίνακας από struct deque_node
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	int front;                  ////Η "αρχη" του deque
    int rear;                   ////To "τελος" του deque
    int capacity;				// Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του array). Πάντα capacity >= size, αλλά μπορεί να έχουμε
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του deque.
};

Deque deque_create(int size, DestroyFunc destroy_value){
	Deque deq = malloc(sizeof(*deq));       ////Δεσμεουμε μνημη για ολο το deque

	deq->size = size;
    deq->front=0;       ////Σε ενα αδειο deque αρχη και τελος συμπιπτουν
	deq->rear=0;
    deq->destroy_value = destroy_value;

	// Δέσμευση μνήμης για τον πίνακα. Αρχικά το deque περιέχει size
	// μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για τουλάχιστον
	// DEQUE_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes.
	//
	deq->capacity = size < DEQUE_MIN_CAPACITY ? DEQUE_MIN_CAPACITY : size;
	deq->array = calloc(deq->capacity, sizeof(*deq->array));		// αρχικοποίηση σε 0 (NULL)

	return deq;
}

int deque_size(Deque deque){
    return deque->size;          ////Το μεγεθος του deque
}

void deque_insert_first(Deque deque, Pointer value){      

	if(deque->size==0){         ////Αν το deque ειναι αδειο τοτε το front πρεπει ν γινει ισο με την αρχη του πινακα deque->array
		deque->front=0;
		deque->rear=0;
		deque->array[deque->front].value=value;
	}else{

		if(deque->size==deque->capacity){      ////πρεπει να κανουμε resize
			deque->capacity *= 2;             ////διπλασιαζουμε το capacity
			
			deque->array= realloc(deque->array,deque->capacity * sizeof(*deque->array));     ////κανουμε realloc 			

			////Αν το front==0 τοτε το deque ειναι πρακτικα ενα vector αρα δεν χρειαζετα να κανουμε κατι αλλο
			if(deque->front!=0){      ////Αν ομως το front!=0 τοτε πρεπει να μετακινισουμε τα στοιχεια ετσι ωστε οι κενες θεσεις να ειναι στο διαστημα[rear+1,front-1]
				int dist=0;          ////Τα στοιχεια απο το front μεχρι το τελος του array
				for(int i=deque->size-1;i>=deque->front;i--){
					deque->array[deque->capacity-1-dist].value=deque->array[i].value;  
					if(deque->destroy_value!=NULL){       ////Αν υπαρχει DestroyFunc καταστρεφουμε το στοιχειο στην προηγουμενη θεση
						deque->destroy_value(deque->array[i].value);
					}
					dist++;
				}
			
				if(deque->front<deque->rear){        ////Αν το front σε αυτη τη διαδικασια κατεληξε πριν το rear
					deque->rear=deque->capacity-1;
				}
				deque->front=deque->capacity-dist;     ////Η νεα θεση του front στο array

			}
		}

		if(deque->front==0){            ////Αν το front δειχνει στην αρχη τοτε δεν μπορουμε να βαλουμε πισω απο αυτην
			deque->front=deque->capacity-1;    ////Αρα θα βαλουμε στο τελος του πινακα
		}else{
			deque->front--;           ////Αν δεν το front δεν ειναι στην αρχη του array τοτε θα βαλουμε μια θεση πισω
		}
		deque->array[deque->front].value=value;   ////Εισαγουμε το value
	}

	deque->size++;	         ////Το μεγεθος μεγαλωνει αφου προσθεσαμε στοιχειο
}

void deque_insert_last(Deque deque, Pointer value){       

	if(deque->size==0){           ////Αν εχουμε αδειο deque
		deque->rear=0;           ////Τα front και rear δειχνουν και τα 2 στην αρχη του array
		deque->front=0;
		deque->array[deque->rear].value=value;
	}else{
		
		if(deque->size==deque->capacity){      ////πρεπει να κανουμε resize
			deque->capacity *= 2;             ////διπλασιαζουμε το capacity
			
			deque->array= realloc(deque->array,deque->capacity * sizeof(*deque->array));			

			
			if(deque->front!=0){      ////Υλοποιηση και σχολια παρομοια με την insert_first
				int dist=0;
				for(int i=deque->size-1;i>=deque->front;i--){
					deque->array[deque->capacity-1-dist].value=deque->array[i].value;
					if(deque->destroy_value!=NULL){
						deque->destroy_value(deque->array[i].value);
					}
					dist++;
				}
			
				if(deque->front<deque->rear){
					deque->rear=deque->capacity-1;
				}
				deque->front=deque->capacity-dist;

			}					

		}

		if(deque->rear==deque->capacity-1){  ////Αν το rear δειχνει στο τελος του array δεν εχει επομενο
			deque->rear=0;              ////Αρα θα εισαγουμε στην αρχη του array
		}else{
			deque->rear++;     ////Αν δεν δειχνει στο τελος του array θα εισαγουμε στην θεση μετα το rear
		}

		deque->array[deque->rear].value=value;       ////Εισαγουμε το value
	}

	deque->size++;       ////Το μεγεθος μεγαλωνει αφου προσθεσαμε στοιχειο
}

void deque_remove_first(Deque deque){           
	assert(deque->size!=0);

	if(deque->destroy_value!=NULL){         ////Αν το deque εχει DestroyFunc την καλουμε για να καταστρεψουμε το value στην πρωτη θεση
		deque->destroy_value(deque->array[deque->front].value);
	}
	
	if(deque->front==deque->capacity-1){  ////Αν το front δειχνει στο τελος του array 
		deque->front=0;                  ////Το επομενο του ειναι το πρωτο στοιχειο του array
	}else{
		deque->front++;               ////Αλλιως το επομενο του ειναι το επομενο στοιχειο του array
	}

	deque->size--;             ////To μεγεθος μειωνεται αφου αφαιρεσαμε στοιχειο

	/////////////Εδω ξεκινα το resize////////////////////

	if (deque->capacity > deque->size * 4 && deque->capacity > 2* DEQUE_MIN_CAPACITY) {

		
		////Παλι αν το front == 0 τοτε εχουμε πρακτικα ενα vector αρα δεν χρειαζεται να κανουμε κατι πριν το realloc
		if(deque->front!=0){  ////Αν ομως front != 0 πρεπει να μετακινησουμε τα στοιχεια απο front μεχρι το τελος του array στο 1ο μισο του array 
			int dist=0;       ////Τα στοιχεια απο φροντ μεχρι τελος του array
			int half_capacity=(deque->capacity)/2;     ////To μισο μεγεθος του array
			for(int i=deque->capacity-1;i>=deque->front;i--){
				deque->array[half_capacity-1-dist].value=deque->array[i].value;
				if(deque->destroy_value!=NULL){
					deque->destroy_value(deque->array[i].value);
				}
				dist++;
			}
			if(deque->front<deque->rear){   ////Αν το front κατεληξε μικροτερο του rear 
				deque->rear=half_capacity-1;  ////Το rear πρεπει να δειξει στη μεση του array -1
			}
			deque->front=half_capacity-dist;   ////H νεα θεση του deque

		}
 
		deque->capacity /= 2;          ////Υποδιπλασιζουμε το capacity
		deque->array = realloc(deque->array, deque->capacity * sizeof(*deque->array));
	}
}

void deque_remove_last(Deque deque){       
	assert(deque->size!=0);

	if(deque->destroy_value!=NULL){     ////Αν υπαρχει DestroyFunc την καλουμε και καταστρεφουμε το value του rear
		deque->destroy_value(deque->array[deque->rear].value);
	}

	if(deque->rear==0){     ////Αν το rear εδειχνε στην αρχη του array  τοτε δεν μπορουμε να κανουμε rear--
		deque->rear=deque->capacity-1;  ////Οποτε το rear θα δειχνει στο τελος του array
	}else{
		deque->rear--;      ////Ειδαλλως το rear θα δειχνει στην προηγουμενη θεση
	}

	deque->size--;       ////Το μεγεθος του deque πρεπει να μειωθει αφου αφαιρεσαμε στοιχειο

     ///////Εδω ξεκινα το resize ////////////


	if (deque->capacity > deque->size * 4 && deque->capacity > 2* DEQUE_MIN_CAPACITY) {
		////Σχολια και υλοποιηση παρομοια με την remove_first			

		if(deque->front!=0){
			int dist=0;
			int half_capacity=(deque->capacity)/2;
			for(int i=deque->capacity-1;i>=deque->front;i--){
				deque->array[half_capacity-1-dist].value=deque->array[i].value;
				if(deque->destroy_value!=NULL){
					deque->destroy_value(deque->array[i].value);
				}
				dist++;
			}
			if(deque->front<deque->rear){
				deque->rear=half_capacity-1;
			}
			deque->front=half_capacity-dist;
			
		}

		deque->capacity /= 2;
		deque->array = realloc(deque->array, deque->capacity * sizeof(*deque->array));

	}
}

Pointer deque_get_at(Deque deque, int pos){     
    assert(pos >= 0 && pos < deque->size);
	
	int act_pos=pos + deque->front;   ////Η θεση που μας ζητειται ειναι θεωρει οτι front==0.Αν front!=0 απλα τα προσθετουμε στην θεση

	return deque->array[(act_pos % deque->capacity)].value;        ////Η θεση  του στοιχειο βρισκεται με την πραξη act_pos % deque->capacity
}

void deque_set_at(Deque deque, int pos, Pointer value){    
    assert(pos >= 0 && pos < deque->size);

	int act_pos=deque->front+pos;    ////Η θεση που μας ζητειται ειναι θεωρει οτι front==0.Αν front!=0 απλα τα προσθετουμε στην θεση

	if(deque->destroy_value!=NULL){       ////Αν υπαρχει DestroyFunc τοτε καταστρεφουμε το προηγουμενο στοιχειο που υπηρχε στην ζητουμενη θεση
		deque->destroy_value(deque->array[act_pos].value);
	}

	deque->array[act_pos % deque->capacity].value=value;   ////Η θεση  του στοιχειο βρισκεται με την πραξη act_pos % deque->capacity
}

Pointer deque_find(Deque deque, Pointer value, CompareFunc compare){
	// Διάσχιση του deque
	for (int i = 0; i < deque->size; i++)        ////Διασχιζουμε ολο το deque και συγκρινου με την compare μεχρι να επιστρεψει 0.Πολυπλοκοτητα Ο(n)
		if (compare(deque->array[i].value, value) == 0)
			return deque->array[i].value;		// βρέθηκε

	return NULL;				// δεν υπάρχει
}

DestroyFunc deque_set_destroy_value(Deque deque, DestroyFunc destroy_value){
	DestroyFunc old = deque->destroy_value;       ////Η παλια DestroyFunc
	deque->destroy_value = destroy_value;     ////Αναθετουμε την νεα DestroyFunc
	return old;                     ////Επιστρεφουμε την παλια DestroyFunc

}

void deque_destroy(Deque deque){
	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για όλα τα στοιχεία
	if (deque->destroy_value != NULL)
		for (int i = 0; i < deque->size; i++)
			deque->destroy_value(deque->array[i].value);

	// Πρέπει να κάνουμε free τόσο τον πίνακα όσο και το struct!
	free(deque->array);
	free(deque);			// τελευταίο το deque!
}

// Διάσχιση του deque ////////////////////////////////////////////////////////////

DequeNode deque_first(Deque deque){       
	if(deque->size==0){   ////Αδειο deque
		return DEQUE_BOF;  ////Αρα πρεπει να επιστρεψουμε DEQUE_BOF
	}else{
		return &(deque->array[deque->front]); ////επιστρεφουμε το array[deque->front] μιας και το front ειναι η αρχη του deque
	}
}

DequeNode deque_last(Deque deque){      
	if(deque->size==0){     ////Αδειο deque
		return DEQUE_EOF;   ////Αρα πρεπει να επιστρεψουμε DEQUE_EOF
	}else{
		return &(deque->array[deque->rear]);   ////επιστρεφουμε το array[deque->rear] μιας και το rear ειναι το τελος του deque
	}
}

DequeNode deque_next(Deque deque, DequeNode node){      
	if (node == &deque->array[deque->size-1]){  ////Αν ο κομβος που δοθηκε ειναι ο τελευταιος δεν εχει επομενο
		return DEQUE_EOF;       ////Αρα πρεπει να επιστρεψουμε DEQUE_EOF
	}else{
		return node + 1;     ///Ο επομενος κομβος (COPY-PASTE απο την υλοποιηση του vector)
	}
}

DequeNode deque_previous(Deque deque, DequeNode node){  
	if (node == &deque->array[0]){    ////Αν ο κομβος που δοθηκε ειναι ο πρωτος δεν εχει προηγουμενο
		return DEQUE_BOF;        ////Αρα πρεπει να επιστρεψουμε DEQUE_BOF
	}else{
		return node - 1;    ///Ο προηγουμενος κομβος (COPY-PASTE απο την υλοποιηση του vector)
	}
}

Pointer deque_node_value(Deque deque, DequeNode node){
    return node->value;        ////Επιστροφουμε το πεδιο value του struct deque_node
}

DequeNode deque_find_node(Deque deque, Pointer value, CompareFunc compare){
	// Διάσχιση του deque
	for (int i = 0; i < deque->size; i++)
		if (compare(deque->array[i].value, value) == 0)
			return &deque->array[i];		// βρέθηκε

	return DEQUE_EOF;				// δεν υπάρχει
}