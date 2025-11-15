#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000

typedef struct WordStruct {
  char *word;
  struct WordProbability *prob_list;
  int recrs;  //times it appeared
  int probsiz; //words after this word
  //... Add your own fields here
} WordStruct;

typedef struct WordProbability {
  struct WordStruct *word_struct_ptr;
  int recrs; //times this event ouccured
  //... Add your own fields here
} WordProbability;


/************ LINKED LIST ************/
typedef struct Node {
  WordStruct *data;
  struct Node *next;
} Node;

typedef struct LinkList {
  Node *first;
  Node *last;
  int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add(LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL)
  {
    return 1;
  }
  *new_node = (Node){data, NULL};

  if (link_list->first == NULL)
  {
    link_list->first = new_node;
    link_list->last = new_node;
  }
  else
  {
    link_list->last->next = new_node;
    link_list->last = new_node;
  }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number) { //gets the main random number
    return rand()%max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word(LinkList *dictionary) { //gets first word randomly
    Node * frst;
    int rand;
    
    do {
        frst = dictionary->first;
        rand = get_random_number(dictionary->size);
        for(int i = 0 ; i < rand ; i++) {
            frst = frst->next;
        }
    } while (frst->data->word[strlen(frst->data->word) - 1] == '.');
    return frst->data;
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word(WordStruct *word_struct_ptr) { //get the nexxt word according to the probability
    
    if (word_struct_ptr->probsiz == 0) {
        return NULL;
    }
    
    int rand = get_random_number(word_struct_ptr->recrs);
    for (int i = 0 ; i < word_struct_ptr->probsiz ; i++) {
        rand -= word_struct_ptr->prob_list[i].recrs;
        if (rand < 0) {
            return  word_struct_ptr->prob_list[i].word_struct_ptr;
        }
    }
    printf("%s",word_struct_ptr->prob_list[0].word_struct_ptr->word);
    return NULL;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence(LinkList *dictionary) {   //generate random sentance by calling the other functions
    int num = 1;
    WordStruct * pnt = get_first_random_word(dictionary);
    printf("%s" , pnt->word);
    while (pnt->probsiz != 0 && num < MAX_WORDS_IN_SENTENCE_GENERATION) {
        num++;
        pnt = get_next_random_word(pnt);
        printf(" %s" , pnt->word);
    }
    
    return num;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list(WordStruct *first_word,
                                 WordStruct *second_word) {
    for (int i = 0 ; i < first_word->probsiz ; i++) {   //find if the word is already found
        if (strcmp(first_word->prob_list[i].word_struct_ptr->word,second_word->word) == 0) {
            first_word->prob_list[i].recrs++;
            return 0;
        }
    }
    
    if (first_word->probsiz == 0) { //check for empty array to add in
        first_word->prob_list = (WordProbability *) malloc(sizeof(WordProbability));
        if (first_word->prob_list == NULL) {
            printf("Allocation failure: malloc failure");
            exit(EXIT_FAILURE);
        }
    }
    else {  //add new word to list
        first_word->prob_list = (WordProbability *) realloc(first_word->prob_list, sizeof(WordProbability)*(first_word->probsiz + 1));
        if (first_word->prob_list == NULL) {
            printf("Allocation failure: malloc failure");
            exit(EXIT_FAILURE);
        }
    }
    first_word->prob_list[first_word->probsiz].recrs = 1;
    first_word->prob_list[first_word->probsiz].word_struct_ptr = second_word;
    first_word->probsiz++;
    
    return 1;
}

/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param words_to_read Number of words to read from file.
 *                      If value is bigger than the file's word count,
 *                      or if words_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */


void fill_dictionary(FILE *fp, int words_to_read, LinkList *dictionary) {
    
    WordStruct * poin = NULL;
    WordStruct * prev = NULL;
    
    int wrds = MAX_SENTENCE_LENGTH;
    if (words_to_read != -1 && words_to_read < MAX_SENTENCE_LENGTH) {   //how many words to read
        wrds = words_to_read*MAX_WORD_LENGTH;
    }
    
    char buf[wrds];
    memset(buf, '\0', wrds);
    char * tok;
    int rd = fread(buf , sizeof(char) , wrds , fp);
    tok = strtok(buf , " \n");  //split to words
    while (tok != NULL && (words_to_read > 0 || words_to_read == -1)) {
        Node * tmp;
        for (tmp = dictionary->first; tmp != NULL ; tmp = tmp->next) {
            if (strcmp(tok, tmp->data->word) == 0) {
                tmp->data->recrs++;
                if (prev != NULL && prev->word[strlen(prev->word)-1] != '.') {
                    add_word_to_probability_list(prev, tmp->data);
                }
                prev = tmp->data;
                break;
            }
        }
        if (tmp == NULL) {
            poin = (WordStruct*) malloc(sizeof(WordStruct));
            if (poin == NULL) {
                printf("Allocation failure: malloc failure");
                exit(EXIT_FAILURE);
            }
            poin->word = (char*) malloc(sizeof(char)*(strlen(tok)+1));
            if (poin->word == NULL) {
                printf("Allocation failure: malloc failure");
                exit(EXIT_FAILURE);
            }
            strcpy(poin->word, tok);
            poin->word[strlen(tok)] = '\0';
            poin->probsiz = 0;
            poin->recrs = 1;
            if (add(dictionary , poin) == 1){
                printf("Allocation failure: malloc failure");
                exit(EXIT_FAILURE);
            }
        }
        if (tmp == NULL && prev != NULL && prev->word[strlen(prev->word)-1] != '.') {
            add_word_to_probability_list(prev, poin);
        }
        if (tmp == NULL) {
            prev = poin;
        }
        
        tok = strtok(NULL , " \n");
        if (words_to_read != -1) {
            words_to_read--;
        }
        
    }
    if (words_to_read == 0) {
        return;
    }
    while (rd != 0 && words_to_read == -1) {

        rd = fread(buf , sizeof(char) , wrds , fp);
        if (rd == 0) {
            break;
        }
        tok = strtok(buf , " \n");
        if (rd < wrds) {
            for (int i = rd ; i < wrds ; i++) {
                buf[i] = '\0';
            }
        }
        
        while (tok != NULL) {
            Node * tmp;
            for (tmp = dictionary->first; tmp != NULL ; tmp = tmp->next) {
                if (strcmp(tok, tmp->data->word) == 0) {
                    tmp->data->recrs++;
                    if (prev != NULL && prev->word[strlen(prev->word)-1] != '.') {
                        add_word_to_probability_list(prev, tmp->data);
                    }
                    prev = tmp->data;
                    break;
                }
            }
            if (strcmp(tok,"a")==0) {
                
            }
            if (tmp == NULL) {
                poin = (WordStruct*) malloc(sizeof(WordStruct));
                if (poin == NULL) {
                    printf("Allocation failure: malloc failure");
                    exit(EXIT_FAILURE);
                }
                poin->word = (char*) malloc(sizeof(char)*(strlen(tok)+1));
                if (poin->word == NULL) {
                    printf("Allocation failure: malloc failure");
                    exit(EXIT_FAILURE);
                }
                strcpy(poin->word, tok);
                poin->word[strlen(tok)] = '\0';
                poin->probsiz = 0;
                poin->recrs = 1;
                if (add(dictionary , poin) == 1){
                    printf("Allocation failure: malloc failure");
                    exit(EXIT_FAILURE);
                }
            }
            if (tmp == NULL && prev != NULL && prev->word[strlen(prev->word)-1] != '.') {
                add_word_to_probability_list(prev, poin);
            }
            if (tmp == NULL) {
                prev = poin;
            }
            tok = strtok(NULL , " \n");
        }
    }
    
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary(LinkList *dictionary) {
    Node * nxt;
    for (Node * wrds = dictionary->first ; wrds != NULL ;) {
        free(wrds->data->word);     //free word
        if (wrds->data->probsiz != 0) {
            free(wrds->data->prob_list);    //free word list if found
        }
        free(wrds->data);
        nxt=wrds->next;     //get next data to free
        free(wrds);
        wrds=nxt;
    }
    free(dictionary);
    
}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main(int argc, char *argv[]) {
    
    int wrdstor = -1;
    
    if (argc < 3 || argc > 5) {
        printf("Usage: seed , number of tweets to generate , file path , number of words to read (optional)\n");
        exit(EXIT_FAILURE);
    }
    
    srand(atoi(argv[1]));
    
    if (argc == 5) {
        wrdstor = atoi(argv[4]);
    }
    
    FILE * filePointer = fopen( argv[3] , "r" );
    
    if (filePointer == NULL) {
        printf("Error: can't open file\n");
        exit(EXIT_FAILURE);
    }
    
    //make and initilize a linkedlist
    
    LinkList * allwrds;
    
    allwrds = (LinkList*) malloc(sizeof(LinkList));
    if (allwrds == NULL) {
        printf("Allocation failure: malloc failure");
        exit(EXIT_FAILURE);
    }
    
    allwrds->size = 0;
    allwrds->first = NULL;
    allwrds->last = NULL;
    
    fill_dictionary(filePointer , wrdstor , allwrds);
    //generates a scentence from the dictionary
    for (int i = 0 ; i < atoi(argv[2]); i++) {
        printf("Tweet %d: ",i+1);
        generate_sentence(allwrds);
        printf("\n");
    }
    //close and empty
    free_dictionary(allwrds);
    fclose(filePointer);
  return 0;
}
