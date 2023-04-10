


#include <stdlib.h>
#include <assert.h>

#include "ADTVector.h"


// Το αρχικό μέγεθος που δεσμεύουμε
#define VECTOR_MIN_CAPACITY 10

// Ένα VectorNode είναι pointer σε αυτό το struct. (το struct περιέχει μόνο ένα στοιχείο, οπότε θα μπροούσαμε και να το αποφύγουμε, αλλά κάνει τον κώδικα απλούστερο)
struct vector_node {
	Pointer value;				// Η τιμή του κόμβου.
	int pos;                   ////Η θεση του κομβου στο vector,βοηθαει με τις συναρτησεις next και previous
};

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	VectorNode lil_array;			//// O μικροτερος απο τους 2 πινακες ,που μαζι περιεχουν ολα τα στοιχεια του vector
	int lil_size;           		////το μεγεθος του "μικρου" πινακα
	
	VectorNode big_array;          ////Ο μεγαλυτερος απο τους 2 πινακες  
	int big_size;                 ////Το μεγεθος του μεγαλου πινακα
	int in_big;                 ////Η τελευταια θεση του lil_array που βαλαμε στο big_array
	
	int size;					//// Πόσα στοιχεία έχουμε προσθέσει στο vector
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του μεγαλυτερου πινακα). Πάντα capacity >= size1+size2, αλλά μπορεί να έχουμε
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};

Vector vector_create(int size, DestroyFunc destroy_value) {
	Vector vec=malloc(sizeof(*vec));

	vec->size=size;
	vec->in_big=-1;       ////-1 ,δεν  εχουμε βαλει καμια θεση του lil στον big

	vec->destroy_value=destroy_value;

	vec->capacity= size < VECTOR_MIN_CAPACITY ? VECTOR_MIN_CAPACITY : size;
	vec->lil_array=calloc(vec->capacity,sizeof(*vec->lil_array));
	vec->lil_size=vec->capacity;
	vec->big_array=NULL;   ////NULL γιατι αρχικα δεν εχουμε μεγαλο πινακα,μονο μικρο
	vec->big_size=0;       ///0 γιατι δεν αρχικα δεν εχουμε μεγαλο πινακα,αρα αυτο ο πινακας εχει 0 στοιχεια

	return vec;
}

int vector_size(Vector vec) {
	return vec->size;      ////Επιστρεφουμε το μεγεθος του vector
}

Pointer vector_get_at(Vector vec, int pos) {
	assert(pos >= 0 && pos < vec->size);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)

	if(vec->big_array==NULL){   ///Αν εχουμε μονο 1 πινακα,τον μικρο
		return vec->lil_array[pos].value;   ////Επιστρεφουμε την τιμη της θεσης στον μικρο πινακ
	}else{ ////Αν εχουμε 2 πιανακες,εναν μικρο και εναν μεγαλο
		if(pos>vec->in_big && pos<vec->lil_size){   ////Αν η θεση βρισκεται εντος του μικρου
			return vec->lil_array[pos].value;   ////Επιστρεφουμε τη τιμη της θεσης στον μικρο
		}else{   				////Αν η θεση βρισκεται εντος του μεγαλου
			return vec->big_array[pos].value; ////Επιστρεφουμε την τιμη της θεσης στον μεγαλο
		}
	}
}

void vector_set_at(Vector vec, int pos, Pointer value) {
	assert(pos >= 0 && pos < vec->size);	// LCOV_EXCL_LINE

	if(vec->big_array==NULL){   ///Αν εχουμε μονο 1 πινακα,τον μικρο
		if(vec->destroy_value!=NULL){   ////Διαγραφουμε το προηγουμενο value
			vec->destroy_value(vec->lil_array[pos].value);
		}
		vec->lil_array[pos].value=value;   ////Αναθετουμε το νεο value
	}else{ ////Αν εχουμε 2 πιανακες,εναν μικρο και εναν μεγαλο
		if(pos>vec->in_big && pos<vec->lil_size){   ////Αν η θεση βρισκεται εντος του μικρου
			if(vec->destroy_value!=NULL){
				vec->destroy_value(vec->lil_array[pos].value);
			}
			vec->lil_array[pos].value=value;
		
		}else{   ////Αν ηθεση βρισκεται εντος του μεγαλου
			if(vec->destroy_value!=NULL){
				vec->destroy_value(vec->big_array[pos].value);
			}
			vec->big_array[pos].value=value;
		}
	}	
}

void vector_insert_last(Vector vec, Pointer value) {


	if(vec->size==vec->capacity){     ////Πρεπει να κανουμε resize ,δηλαδη να δημιουργησουμε εναν μεγαλυτερο πινακα
		vec->capacity *= 2;
		vec->big_array=malloc(vec->capacity * sizeof(*vec->big_array));
	}
	
	if(vec->big_array==NULL){     ////Αν δεν εχουμε μεγαλο πινακα,δηλαδη εχουμε μονο εναν πινακα διαθεσιμο,τον μικρο
		vec->lil_array[vec->size].value=value;    ////Το στοιχειο μπαινει στον μικρο πινακα
		vec->lil_array[vec->size].pos=vec->size;  ////Kραταμε και τη θεση του στο vector
	}else{    ////Αν εχουμε μεγαλο πινακα πρεπει 
		vec->in_big++;     ////Το στοιχειο του μικρου που θα βαλουμε στον μεγαλο
		vec->big_array[vec->in_big].value=vec->lil_array[vec->in_big].value;    ////Α) να μεταφερουμε ενα στοιχειο του μικρου στον μεγαλο
		vec->big_array[vec->in_big].pos=vec->in_big;
		vec->big_size++;   ////Βαλαμε ενα στοιχειο απο τον μικρο στο μεγαλο αρα το μεγεθος του μεγαλου αυξηθηκε

		if(vec->destroy_value!=NULL){             ////Διαγραφουμε το στοιχειο του μικρου που μεταφεραμε στον μεγαλο 
			vec->destroy_value(vec->lil_array[vec->in_big].value);
		}
	
		vec->big_array[vec->size].value=value;                             ////Β) να βαλουμε το νεο στοιχειο στον μεγαλο
		vec->big_array[vec->size].pos=vec->size;
		vec->big_size++;          ////Βαλαμε ενα νεο στοιχειο στον μεγαλο αρα το μεγεθος του μεγαλου αυξηθηκε

		if(vec->big_size==2*vec->lil_size){         /////Αν ο μεγαλος εχει γεμισει,δηλαδη εχει ολα τα στοιχεια του μικρου + αυτα που προσθεσαμε
			
			free(vec->lil_array);                   ////Απελευθερωνουμε την μνημη που εχουμε δεσμευσει για τον μικρο αφου αδειασε
			vec->lil_array=vec->big_array;         ////Ο μεγαλος γινεται μικρος
			vec->lil_size=vec->big_size;          ////Το μεγεθος του μικρου γινεται το μεγεθος του μεγαλου
			
			vec->big_array=NULL;			////Δεν υπαρχει πλεον μεγαλος
			vec->big_size=0;                ////Αρα εχει μεγεθος 0
			vec->in_big=-1;          ////Και αφου δεν υπαρχει μεγαλος,δεν υπαρχει και στοιχειο του μικρου που να εχει μεταφερθει στον μεγαλο
		}

	}

	vec->size++;       ////Προσθεσαμε στοιχειο στο vector αρα το μεγεθος του αυξηθηκε
}

void vector_remove_last(Vector vec) {
	assert(vec->size != 0);		// LCOV_EXCL_LINE

	if(vec->big_array==NULL){   ////Εχουμε μονο ενα πινακα ,τον μικρο
		if(vec->destroy_value!=NULL){     ////Αρα διαγραφουμε απο αυτον
			vec->destroy_value(vec->lil_array[vec->size-1].value);
		}
	}else{            ////Εχουμε 2 πινακες, εναν μικρο και εναν μεγαλο
		if(vec->destroy_value!=NULL){           ////Αρχικα διαγραφουμε το τελευταιο στοιχειο
			vec->destroy_value(vec->big_array[vec->size-1].value);	
		}
		vec->big_size--; ////Αφου διαγραψαμε στοιχειο το μεγεθος του μεγαλου πινακα μειωνεται

		vec->lil_array[vec->in_big].value=vec->big_array[vec->in_big].value; ////Eπειτα επιστρεφουμε το στοιχειο που μεταφεραμε απο τον μικρο στον μεγαλο πινακα πισω στον μικρο

		if(vec->destroy_value!=NULL){        ////Διαγραφουμε το στοιχειο που επιστρεψαμε απο τον μεγαλο πιανακα
			vec->destroy_value(vec->big_array[vec->in_big].value);
		}
		vec->big_size--;   ////Αφου διαγραψαμε στοιχειο το μεγεθος του μεγαλου πινακα μειωνεται
		vec->in_big--;   ///Αφου επιστρεψαμε ενα στοιχειο τα στοιχεια του μικρου που ειναι στον μεγαλο μειωθηκαν

		if(vec->big_size==0){     ////Aν ο μεγαλος πινακας αδειασει σημαινει οτι δεν τον χρειαζομαστε πλεον
			free(vec->big_array); ////Απελευθερωνουμε τον πλεον αδειο μεγαλο πινακα
			vec->big_size=0;     ////Αρα το μεγεθος του ειναι 0
			vec->in_big=-1;      ////Αρα κανενα στοιχειο του μικρου δεν ειναι στον μεγαλο
			vec->capacity /=2;   ////Και το vector εχει πλεον τη μιση capacity

		}

	}
	// Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
	vec->size--; 
}

Pointer vector_find(Vector vec, Pointer value, CompareFunc compare) {
	// Διάσχιση του vector
	for(int i =0;i<vec->size;i++){

		if(vec->big_array==NULL){   ////Αν εχουμε μονο εναν πινακα,τον μικρο
			if(compare(vec->lil_array[i].value,value)==0){ ////Αν με βαση την compare ειναι ισα
				return vec->lil_array[i].value;
			}			
		}else{     ////Αν εχουμε 2 πινακες,εναν μικρο και εναν μεγαλο
			if(i>vec->in_big && i<vec->lil_size){  ////Αν η θεση που εξεταζουμε ειναι αποκλειστικα εντος του μικρου πινακα
				if(compare(vec->lil_array[i].value,value)==0){    ////Αν με βαση την compare ειναι ισα
					return vec->lil_array[i].value;
				}
			}else{     ////Αν η θεση που εξεταζουμε βρισκεται αποκλειστικα εντος του μεγαλου πινακα
				if(compare(vec->big_array[i].value,value)==0){        ////Αν με βαση την compare ειναι ισα
					return vec->big_array[i].value;
				}
			}		
		}
	}
	return NULL;				// δεν υπάρχει
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value) {
	DestroyFunc old = vec->destroy_value;     ////Η παλια DestroyFunc
	vec->destroy_value = destroy_value;      ////Αναθετουμε την καινουργια
	return old;                             ////Επιστρεφουμε την παλια
}

void vector_destroy(Vector vec) {

	if(vec->destroy_value!=NULL){    ////Αν μας εχει δωθει DestroyFunc θα καταστρεψουμε ολα τα values του vector
		for(int i =0;i<vec->size;i++){    ////Διασχιζουμε τον vector
			
			if(vec->big_array==NULL){    ////Αν εχουμε μονο 1 πινακα,τον μικρο   
				vec->destroy_value(vec->lil_array[i].value);  ////Διαγραφουμε απο τον μικρο
			}else{    ////Αν εχουμε 2 πινακες ,εναν μικρο και εναν μεγαλο
				if(i>vec->in_big && i<vec->lil_size){       ////Το στοιχειο βρισκεται εντος του μικρου
					vec->destroy_value(vec->lil_array[i].value);
				}else{         ////To στοιχειο βρισκεται εντος του μεγαλου
					vec->destroy_value(vec->big_array[i].value);					
				}
			}

		}
	}

	free(vec->big_array);    ////Διαγραφουμε τον μεγαλο
	free(vec->lil_array);   ////Διαγραφουμε τον μικρο
	free(vec);			// τελευταίο το vec!
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec) {
	if (vec->size == 0){          ////To vector ειναι αδειο
		return VECTOR_BOF;
	}else{         ////Το vector δεν ειναι αδειο
		if(vec->big_array==NULL){      ////Εχουμε μονο εναν πινακα,τον μικρο
			return &vec->lil_array[0];
		}else{     ////Εχουμε 2 πινακες,εναν μικρο και εναν μεγαλο
			return &vec->big_array[0];
		}	
	}
}

VectorNode vector_last(Vector vec) {
	if (vec->size == 0){	 ////To vector ειναι αδειο
		return VECTOR_EOF;
	}else{                  ////Το vector δεν ειναι αδειο
		if(vec->big_array==NULL){      ////Εχουμε μονο εναν πινακα,τον μικρο
			return &vec->lil_array[vec->size-1];
		}else{         ////Εχουμε 2 πινακες,εναν μικρο και εναν μεγαλο
			return &vec->big_array[vec->size-1];
		}
	}
}

VectorNode vector_next(Vector vec, VectorNode node) {
	int node_pos=node->pos;    ////H θεση του node μεσα στο vector,τοβαζουμε σε μεταβλητη για λογους ευαναγνωσιας

	if (node_pos==vec->size-1){       ////To node ειναι το τελευταιο του vector
		return VECTOR_EOF;
	}else{        ////Το node δεν ειναι στην τελευταια θεση
		if(node_pos + 1 <= vec->in_big || node_pos + 1 > vec->lil_size-1){      ////Το επομενο του node βρισκεται στον μεγαλο πινακα
			return &vec->big_array[node_pos + 1];
		}else{     ////Το επομενο του node βρισκεται στον μικρο πινακα
			return &vec->lil_array[node_pos + 1];
		}

	}
}

VectorNode vector_previous(Vector vec, VectorNode node) {
	int node_pos=node->pos;   ////H θεση του node μεσα στο vector,τοβαζουμε σε μεταβλητη για λογους ευαναγνωσιας
	
	if (node_pos==0){         ////To node ειναι το πρωτο  του vector
		return VECTOR_EOF;
	}else{          ////To node δεν ειναι το πρωτο του vector
		if(node_pos - 1 <= vec->in_big || node_pos - 1 > vec->lil_size-1){   ////Το προηγουμενο του node βρισκεται στον μεγαλο πινακα
			return &vec->big_array[node_pos - 1];
		}else{             ////Το προηγουμενο του node βρισκεται στον μικρο πινακα
			return &vec->lil_array[node_pos - 1];
		}
	}
}

Pointer vector_node_value(Vector vec, VectorNode node) {
	return node->value;   ////Επιστρεφουμε την τιμη του κομβου
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare) {
	// Διάσχιση του vector
	for (int i = 0; i < vec->size; i++){
		
		if(vec->big_array==NULL){   ////Αν εχουμε μονο εναν πινακα,τον μικρο
			if(compare(vec->lil_array[i].value,value)==0){ ////Αν με βαση την compare ειναι ισα
				return &vec->lil_array[i];   ////Επιστρεφουμε τον κομβο αυτο,ο οποιος βρισκεται στον μικρο πινακα
			}			
		}else{  ////Αν εχουμε 2 πινακες,εναν μικρο και εναν μεγαλο
			if(i>vec->in_big && i<vec->lil_size){  ////Αν η θεση που εξεταζουμε ειναι αποκλιστεικα εντος του μικρο πινακα
				if(compare(vec->lil_array[i].value,value)==0){   ////Αν με βαση την compare ειναι ισα
					return &vec->lil_array[i];      ////Επιστρεφουμε τον κομβο αυτο,ο οποιος βρισκεται στον μικρο πινακα
				}
			}else{       ////Αν η θεση που εξεταζουμε ειναι αποκλιστεικα εντος του μεγαλου πινακα
				if(compare(vec->big_array[i].value,value)==0){   ////Αν με βαση την compare ειναι ισα
					return &vec->big_array[i];    ////Επιστρεφουμε τον κομβο αυτο,ο οποιος βρισκεται στον μεγαλο πινακα
				}
			}		
		}		    
    }
	return VECTOR_EOF;				// δεν υπάρχει
}