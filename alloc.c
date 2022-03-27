#include "alloc.h"
#include <stdlib.h>

/********************************************//**
 *   Répartition des tâches
 ***********************************************/
/**
 Lydia+Melissa:définition de la stratégie à utiliser

 Lydia :
  o	Foncions initMemory et myalloc
  o	Fonctions d'affichage
  o Mode interactif
  o Mode batch
  o	Doxygen
  o	Document "guide utilisateur"


 Melissa :
  o	Fonctions  myfree et freememory
  o Test simple
  o Mode ligne de commande
  o	Makefile
  o	Document "guide développeur"

*/

/********************************************//**
 *   initMemory
 ***********************************************/
int initMemory(int nBytes){
	size_t initial_size = (size_t) nBytes;
	taille = (int) initial_size;
	size_t allocsize = initial_size;

	/**
	*  allouer un espace memoire avec la taille demandée + la taile de metaData
	   ave comme adresse de debut la constante HEAP_START
	*/
	head = mmap( HEAP_START, allocsize,
		PROT_READ | PROT_WRITE | PROT_EXEC,
		MAP_ANON | MAP_PRIVATE, -1, 0);

	/**
	*   initaliser la tete de liste avec le premier noeud
	*/
	struct node *tete = HEAP_START;
	tete->next = NULL;
	tete->capacity = allocsize - META_DATA_SIZE;
	tete->is_free = true;

	if(head == NULL){
		return 0 ;
	}else{
		return nBytes;
	}
}

 /********************************************//**
  *   myalloc
  ***********************************************/

void* myalloc(int nBytes){

	size_t query = (size_t) nBytes;

	/**
	/**
 * trouver un bloc vide avec une taille suffisante
 */
	struct node* block = find_block(HEAP_START, query);

	/**
 * s'il n'y a aucun bloc avec la taille suffisante : message d'erreur
 */
	if(block == NULL){
		printf("no space available \n");
	}

    /**
 *s'il y'a un bloc avec une taille plus grande que nBytes,on le
 divise en deux. Une partie qui va contenir la taille demandée et
 l'autre va etre un bloc libre pret à etre alloué en cas de besoin


  */
	else{
		trysplit(block, query);
		block->is_free = false;
	}
	return ((char*)block + META_DATA_SIZE);
}


void trysplit(struct node* blk, size_t query){
	struct node* newblock;

	/**
	*  calculer la differece entre  nbytes et le bloc à diviser
	*/
	size_t blksize = blk->capacity - query - META_DATA_SIZE;

	/**
	*  calculer la taille du nouveau bloc nBytes + metaData
	*/
	size_t totalblocksize = query + META_DATA_SIZE;

	/**
	*  si la differece entre nbytes et le bloc à diviser est <0 :erreur
	*/
	if(blksize <= 0)
		return;
		/**
		*  créer un nouveau bloc libre qui est le reste
		*/
	newblock = (struct node*)((char *)blk + totalblocksize);
	newblock->capacity = blksize;
	newblock->next = blk->next;
	newblock->is_free = true;
	blk->next = newblock;
	blk->capacity = query;

}



struct node* find_block(struct node* block, size_t query){
	/**
	*  parcourir la liste jusqu'a trouver un bloc avec une taille suffisante
	*/
	for(;block; block=block->next){
		if( block->is_free && block->capacity >= query )
			return block;
	}

	return NULL;
}
/********************************************//**
 *   myfree
 ***********************************************/
int myfree(void* p){
	void* blk = p ;

	/**
 *  	si le pointer est en dehors de notre plage d'adressage : erreur
 */
	if(blk == NULL && (int*)blk > ((int*)HEAP_START + taille) && blk < HEAP_START){
		return -1 ;
		}

	/**
 *  	 calcuer @ de debut de bloc sans metaData (META_DATA_SIZE )
 */
	struct node* block = (struct node*)( (char*)blk - META_DATA_SIZE ) ;
	struct node* nextblk = block->next;

	/**
 *  	si le bloc est libre : erreur
 */
	if(block->is_free){
		printf("Désallocation éfféctuée avec succés \n");
		return -1 ;
	}
	/**
 *  	marquer le bloc comme libre
 */
	block->is_free = true;

/**
*  si le bloc suivant est libre, on fusionne les deux
*/
	if(nextblk != NULL){
		if(nextblk->is_free == true){
			block->capacity += nextblk->capacity;
			block->next = nextblk->next;

		}
	}
}

/********************************************//**
 *   FreeMomory
 ***********************************************/
int FreeMemory(){

			/**
			*   // liberer la memoire allouée
			*/
			if (munmap(head, taille) == 0){
			    printf("la memoire est libre \n" );
			    return taille;
			}else{
			    return -1;
			}


}
/********************************************//**
 *    fonctions d'affichage
 ***********************************************/
void nodealloc_debug_struct_info(FILE* f, struct node const* const address) {
    size_t i;
    fprintf(f, "adress: %p\nsize: %lu\nis_free: %d\n", (void*) address, address->capacity, address->is_free);
    putc( '\n', f );
}

void nodealloc_debug_heap( FILE* f, struct node const* ptr ) {
    for(; ptr; ptr = ptr->next)
        nodealloc_debug_struct_info( f, ptr );
    printf("================\n");
}


/********************************************//**
 *   main
 ***********************************************/
int main(int argc , char *argv[]) {
/********************************************//**
 *  test simple
 ***********************************************/
		if(argc < 2 ){

		 initMemory(20000);
		 void* p1 = myalloc(100);
		 void* p2 = myalloc(350);
		 void* p3 = myalloc(400);
		 void* p4 = myalloc(200);
		 printf("Etat de la memoire apres l'allocation de 300 , 350 , 400 et 200 bits \n");
		 nodealloc_debug_heap(stdout, HEAP_START);
		 myfree(p4);
		 myfree(p2);
		 myfree(p3);
		 printf("Etat de la memoire apres la liberation des blocs 4,2 et 3 \n");
		 nodealloc_debug_heap(stdout, HEAP_START);
		 FreeMemory();
			return 0;
		}
		void* blocs[20];

		/********************************************//**
		 *   -i interafctif
		 ***********************************************/
		if(strcmp(argv[1], "-i") ==0 ){
			char t[20] ;
			printf("Veuillez initialiser la zone mémoire:\n");
			gets(t);
			initMemory(atoi(t));
			int cpt = 0 ;
			while(1){
				printf("\noption 1: pour l'allocation d'un bloc avec une taile n tapez: +n\n");
				printf("option 2: pour la desallocation d'un bloc avec un numero Numb tapez: -NumB\n");
				printf("option 3: pour terminer tapez : e\n\n");
				char commande[50];
				gets(commande);
				if(strcmp(commande, "e") ==0 ){
					    FreeMemory();
						return 0 ;
					}
					if(atoi(commande) > 0){
						blocs[cpt] = myalloc(atoi(commande)) ;
						printf("Etat de la memoire apres l'allocation de %d bits :\n", atoi(commande));
									nodealloc_debug_heap(stdout, HEAP_START);
						cpt++;
					}
					if(atoi(commande) <=0){
						int index = -1 * atoi(commande);
						myfree(blocs[index]);
						printf("Etat de la memoire apres la liberation du bloc num %d :\n", index);
						nodealloc_debug_heap(stdout, HEAP_START);
					}
					if(strcmp(commande, "e") ==0 ){
					    FreeMemory();
						return 0 ;
					}
			}
		}

			/********************************************//**
			 *   -l linge de commande
			 ***********************************************/
		if(strcmp(argv[1], "-l") ==0 ){
			int t = atoi(argv[2]);
			initMemory(t);
			printf("Etat de memoire apres l'initialisation:  \n");
			nodealloc_debug_heap(stdout, HEAP_START);
			int cpt = 0 ;
			for(int i = 3 ; i < argc ; i++){
				if(atoi(argv[i]) > 0){
					blocs[cpt] = myalloc(atoi(argv[i])) ;
					            printf("Etat de la memoire apres l'allocation de %d bits :\n", atoi(argv[i]));
								nodealloc_debug_heap(stdout, HEAP_START);
					cpt++;
				}
				if(atoi(argv[i]) <=0){
					int index = -1 * atoi(argv[i]);
					myfree(blocs[index]);
				printf("Etat de la memoire apres la liberation du bloc num %d :\n", index);
					nodealloc_debug_heap(stdout, HEAP_START);
				}
			}
		}
		/********************************************//**
		 *   -f batch
		 ***********************************************/
		if(strcmp(argv[1], "-f") ==0 ){
			FILE *fp = fopen(argv[2],"r");

			  char buf[10];
				fscanf(fp, "%s", buf);
				initMemory(atoi(buf));
				printf("Etat de la memoire apres l'initialisation:  \n");
				nodealloc_debug_heap(stdout, HEAP_START);
				int cpt = 0 ;
				while( fscanf(fp, "%s", buf) != EOF )
				{
						if(atoi(buf) > 0){
							blocs[cpt] = myalloc(atoi(buf)) ;
							            printf("Etat de la memoire apres l'allocation de %d bits :\n", atoi(buf));
										nodealloc_debug_heap(stdout, HEAP_START);
							cpt++;
						}
						if(atoi(buf) <=0){
							int index = -1 * atoi(buf);
							myfree(blocs[index]);
							printf("Etat de la memoire apres la liberation du bloc num %d :\n", index);
							nodealloc_debug_heap(stdout, HEAP_START);
						}
				}
		}
		FreeMemory();
    return 0;
}

