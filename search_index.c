#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///
////
/////   STRUCTURI
////
///

typedef struct FileRef {
    struct File *file;
    struct FileRef *next;
} FileRef;
// lista simplu inlantuita de referinte catre fisier

typedef struct Tree {
    char ch;               // caracterul din fiecare nod
    struct Tree *child;    // primul copil
    struct Tree *sibling;  // urmatorul frate
    FileRef *files;        // lista de referinte la fisiere, ce exista DOAR in nodurile terminale
    int is_terminal;       // constanta ce indica 1 daca nodul este terminal si 0 in caz contrar
} Tree;

typedef struct KeywordNode {
    char word[101];
    struct KeywordNode *next;
} KeywordNode;
// lista simplu inlantuita de cuvinte cheie

typedef struct File {
    char id[101];               // numele fisierului
    int score;                  // relevanta
    KeywordNode *keywords;      // lista cuvintelor cheie
    struct File *prev;
    struct File *next;
} File;

typedef struct {
    File *head;  // primul fisier din lista simplu inlantuita de fisiere
    File *tail;  // ultimul fisier din lista simplu inlantuita de fisiere
} FileList;

///
////
/////   LISTA DE FISIERE
////
///

FileList *createList() {
    FileList *list = malloc(sizeof(FileList));
    list->head = NULL;
    list->tail = NULL;
    return list;
}
// functie care creaza o lista goala

File *findFile(FileList *list, const char *id) {
    File *iter = list->head;

    while (iter != NULL) {
        if (strcmp(iter->id, id) == 0){
            return iter;
        }
        iter = iter->next;
    }

    return NULL;
}
// functie care parcurge lista de fisiere de la cap la coada si cauta fisierul cu id ul dat ca parametru si intoarce pointer la fisierul respectiv

File *createFile(const char *id, int score) {
    File *f = malloc(sizeof(File));

    strcpy(f->id, id);
    f->score = score;
    f->keywords = NULL;
    f->prev = f->next = NULL;

    return f;
}
// 

void addFileToList(FileList *list, File *file) {
    file->prev = list->tail;
    file->next = NULL;

    if(list->tail != NULL){
        list->tail->next = file;
    }
    else{
        list->head = file;
    }
    list->tail = file;
}
// functie care adauga un fisier la finalul listei de fisiere
// daca lista este goala file devine si head si tail

void removeFileFromList(FileList *list, File *file) {
    if(file->prev != NULL){
        file->prev->next = file->next;
    }
    else{
        list->head = file->next;
    }
    if(file->next != NULL){
        file->next->prev = file->prev;
    }
    else{
        list->tail = file->prev;
    }
}
// functie care scoate un fisier din lista de fisiere


int hasKeyword(File *file, const char *word) {
    KeywordNode *iter = file->keywords;

    while(iter != NULL){
        if(strcmp(iter->word, word) == 0){
            return 1;
        }
        iter = iter->next;
    }

    return 0;
}
// functie care returneaza 1 daca un cuvant cheie este in lista de cuvinte cheie a unui fisier sau 0 daca nu este

void addKeywordToFile(File *file, const char *word) {
    if(hasKeyword(file, word) == 1){
        return;
    }
    KeywordNode *k = malloc(sizeof(KeywordNode));

    strcpy(k->word, word);
    k->next = file->keywords;
    file->keywords = k;
}
// insereaza un cuvant dat ca parametru la inceputul listei de keywords a fisierului, daca acesta nu este deja in lista

void removeKeywordFromFile(File *file, const char *word) {
    KeywordNode *iter = file->keywords;
    KeywordNode *prev = NULL;

    while(iter != NULL){
        if (strcmp(iter->word, word) == 0){
            if(prev != NULL){
                prev->next = iter->next;
            }
            else{
                file->keywords = iter->next;
            }
            free(iter);
            return;
        }
        prev = iter;
        iter = iter->next;
    }
}
// functie care scoate din lista de keywords a unui fisier, un cuvant dat ca parametru

void freeFile(File *file) {
    KeywordNode *k = file->keywords;

    while(k != NULL){
        KeywordNode *tmp = k->next; 
        free(k);
        k = tmp; 
    }
    free(file);
}
// functie care elibereaza memoria alocata pentru un fisier dat ca parametru

///
////
/////   TRIE
////
///

Tree *createTrieNode(char ch) {
    Tree *t = malloc(sizeof(Tree));

    t->ch = ch; 
    t->child = NULL;
    t->sibling = NULL; 
    t->files = NULL; 
    t->is_terminal = 0;

    return t;
}
// functie care creaza un nod de arbore si il initailizeaza cu o valoare data ca paramteru

void addFileRef(Tree *node, File *file) {
    FileRef *ref = malloc(sizeof(FileRef));
    ref->file = file;
    ref->next = node->files;
    node->files = ref;
}
// adauga o referinta la fisierul file in lista files din nodul terminal
// creeaza un nou ref care pointeaza la fisier si il adauga la inceputul listei de fisiere

void removeFileRef(Tree *node, File *f) {
    FileRef *iter = node->files;
    FileRef *prev = NULL;

    while(iter != NULL){
        if(iter->file == f){
            if(prev != NULL){
                prev->next = iter->next;
            }
            else{
                node->files = iter->next;
            }
            free(iter); 
            return;
        }
        prev = iter; 
        iter = iter->next;
    }
}
// functie care elimina referinta la un fisier dintr un nod terminal
// cauta in lista files a nodului care pointeaza la f si elimina nodul

Tree *findChild(Tree *node, char ch) {
    Tree *iter = node->child;

    while (iter != NULL){
        if (iter->ch == ch){
            return iter;
        }
        iter = iter->sibling;
    }

    return NULL;
}
// functie care cauta nodul ch in arborele node si il returneaza
// daca caracterul ch nu este in niciun nod se returneaza NULL

void insertChildSorted(Tree *parent, Tree *newNode) {
    if(!parent->child || newNode->ch < parent->child->ch){
        newNode->sibling = parent->child;
        parent->child = newNode;
        return;
    }

    Tree *iter = parent->child;

    while(iter->sibling && iter->sibling->ch < newNode->ch){
        iter = iter->sibling;
    }

    newNode->sibling = iter->sibling;
    iter->sibling = newNode;
}
// functie care insereaza newNode ca copil al lui parent in ordine alfabetica
// ordinea alfabetica este mentinuta pentru ca printDFS parcurge fratii in ordine si vrem cuvintele scrise corect

void trieInsert(Tree *root, const char *word, File *file) {
    Tree *iter = root;

    for (int i = 0; word[i]; i++){
        Tree *found = findChild(iter, word[i]);
        if (found == NULL){
            found = createTrieNode(word[i]);
            insertChildSorted(iter, found);
        }
        iter = found;
    }

    iter->is_terminal = 1;
    addFileRef(iter, file);
}

// functie care adauga cuvantul word in arbore 
// parcurge cuvantul caracter cu caracter si daca caracterul nu exista il adauga in arbore

Tree *trieFind(Tree *root, const char *word) {
    Tree *iter = root;

    for (int i = 0; word[i]; i++){
        iter = findChild(iter, word[i]);
        if (iter == NULL){
            return NULL;
        }
    }

    if(iter != NULL && iter->is_terminal == 1){
        return iter;
    }else{
        return NULL;
    }
}
// functie care cauta cuvantul word in trie
// daca la final nodul nu este terminal inseamna ca am gasit doar un prefix al unui cuvant
// returneaza nodul doar daca este terminal

int trieDeleteHelper(Tree *node, const char *word, int depth, File *f) {
    if (word[depth] == '\0') {
        removeFileRef(node, f);
        if(node->files == NULL){
            node->is_terminal = 0;
        }
        if(node->is_terminal == 0 && node->child == NULL){
            return 1;
        }else{
            return 0;
        }
    }

    Tree *prev = NULL,
    *cur = node->child;

    while(cur && cur->ch != word[depth]){
        prev = cur;
        cur = cur->sibling;
    }
    if(cur == NULL){
        return 0;
    }

    int canDelete = trieDeleteHelper(cur, word, depth + 1, f);
    if(canDelete == 1){
        if(prev){
            prev->sibling = cur->sibling; 
        }
        else{
            node->child = cur->sibling;
        }
        free(cur);
    }

    if(node->is_terminal == 0 && node->child == NULL){
        return 1;
    }else{
        return 0;
    }
}
// functie recursiva care sterge referinta la fisier dintr un cuvant si sterge apoi nodurile inutile

void trieDelete(Tree *root, const char *word, File *f) {
    if (word == NULL || !word[0]){
        return;
    }

    Tree *prev = NULL,
    *cur = root->child;

    while (cur && cur->ch != word[0]){
        prev = cur;
        cur = cur->sibling;
    }

    if(cur == NULL){
        return;
    }
    int canDelete = trieDeleteHelper(cur, word, 1, f);

    if(canDelete == 1){
        if(prev != NULL){
            prev->sibling = cur->sibling;
        }
        else{
            root->child = cur->sibling;
        }
        free(cur);
    }
}
// functie care sterge cuvantul word din arbore

///
////
/////   HEAP
////
///

typedef struct {
    File **data;
    int size, capacity;
}Heap;

Heap *createHeap(int cap) {
    if (cap < 1){
        cap = 1;
    }

    Heap *h = malloc(sizeof(Heap));
    h->data = malloc(cap * sizeof(File*));
    h->size = 0;
    h->capacity = cap;

    return h;
}
// functie care aloca un heap cu capacitatea data

int heapCmp(File *a, File *b) {
    if(a->score != b->score){
        return a->score > b->score;
    }

    return (strcmp(a->id, b->id) < 0);
}
// functie care returneaza 1 daca a are prioritate mai mare decat b

void heapSwap(Heap *h, int i, int j) {
    File *t = h->data[i];
    h->data[i] = h->data[j]; 
    h->data[j] = t;
}
// interschimba doua elemente

void heapUp(Heap *h, int i) {
    while (i > 0){
        int p;
        p = (i-1)/2;

        if (heapCmp(h->data[i], h->data[p])){
            heapSwap(h, i, p);
            i = p;
        }else{
            break;
        }
    }
}
// functie folosita pentru inserare
// cat timp elementul curent are prioritate mai mare decta parintele il urcam, ne oprim cand ajungem la root

void heapDown(Heap *h, int i) {
    while(1){
        int best = i;
        int l = 2*i+1;
        int r = 2*i+2;

        if (l<h->size && heapCmp(h->data[l], h->data[best])){
            best = l;
        }
        if (r < h->size && heapCmp(h->data[r], h->data[best])){
            best = r;
        }
        if (best == i){
            break;
        }

        heapSwap(h,i,best);
        i = best;
    }
}
// functie apelata la extract
// verif care e cel mai prioritar nod dintre nodul curent si cei doi copii ai lui, si restabilesc proprietatea de heap

void heapInsert(Heap *h, File *f){
    h->data[h->size++] = f;
    heapUp(h,h->size-1);
}
// functie care insereaza fisierul in heap
// pune elementul la finalul array-ului apoi apeleaza heapup ca sa-l puna la locul bun

File *heapExtract(Heap *h) {
    if(!h->size){
        return NULL;
    }

    File *top = h->data[0];
    h->data[0] = h->data[--h->size];
    if(h->size > 0){
        heapDown(h, 0);
    }

    return top;
}
// se extrage elementul maxim din heap, se scade size-ul
// ultimul element devine root, iar apoi se echilibreaza heap-ul folosind heapdown

void freeHeap(Heap *h){
    free(h->data);
    free(h);
}
// functie ce elibereaza memoria heap-ului

///
////
/////   OPERATII
////
///

void opAdd(FileList *list, Tree *root, FILE *out, char *id, int score, int t, char keywords[][101]){
    if(findFile(list, id)){
        fprintf(out, "EXISTS\n");
        return;
    }

    File *f = createFile(id, score);
    addFileToList(list, f);

    for (int i = 0; i < t; i++) {
        if(hasKeyword(f, keywords[i]) == 1){
            continue;
        }
        addKeywordToFile(f, keywords[i]);
        trieInsert(root, keywords[i], f);
    }

    fprintf(out, "OK\n");
}
// verifica prima data daca id-ul exista deja
// pentru fiecare cuvant cheie daca e duplicat il ignora, daca nu il adauga in fisier si il insereaza in trie

void opDel(FileList *list, Tree *root, FILE *out, char *id) {
    File *f = findFile(list, id);
    if(f == NULL){
        fprintf(out, "NOT FOUND\n");
        return;
    }

    KeywordNode *kn = f->keywords;
    while(kn != NULL){
        trieDelete(root, kn->word, f);
        kn = kn->next;
    }

    removeFileFromList(list, f);
    freeFile(f);
    fprintf(out, "OK\n");
}
// verifica daca exista fisierul
// daca fisierul exista parcurge toate keyword-urile si le elimina din trie, apoi scoate fisierul din lista dublu inlantuita

void opAddKw(FileList *list, Tree *root, FILE *out, char *id, const char *word) {
    File *f = findFile(list, id);
    if(f == NULL){
        fprintf(out, "NOT FOUND\n");
        return;
    }
    if(hasKeyword(f, word) == 0){
        addKeywordToFile(f, word);
        trieInsert(root, word, f);
    }
    fprintf(out, "OK\n");
}
// verifica daca exista fisierul
// daca fisierul exista si nu are deja cuvantul in lista de keywords il adauga in lista de keywords si in trie

void opDelKw(FileList *list, Tree *root, FILE *out, char *id, const char *word) {
    File *f = findFile(list, id);
    if(f == NULL){
        fprintf(out, "NOT FOUND\n");
        return;
    }
    if(hasKeyword(f, word) == 1){
        trieDelete(root, word, f);
        removeKeywordFromFile(f, word);
    }
    fprintf(out, "OK\n");
}
// verifica daca exista fisierul
// daca fisierul exista si are cuvantul in lista, il elimina din trie si din lista de keywords

int cmpIds(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(char **)b);
}
// functie de comparare pentru qsort

void opFind(Tree *root, FILE *out, char *word) {
    Tree *node = trieFind(root, word);

    if(node  == NULL || node->files == NULL){
        fprintf(out, "EMPTY\n");
        return;
    }

    int cnt = 0;
    FileRef *ref = node->files;

    while(ref != NULL){
        cnt++;
        ref = ref->next;
    }

    char **ids = malloc(cnt * sizeof(char *));
    ref=node->files;

    for(int i=0; i < cnt; i++){
        ids[i] = ref->file->id;
        ref = ref->next;
    }

    qsort(ids, cnt, sizeof(char *), cmpIds);
    fprintf(out, "%d", cnt);

    for(int i=0; i<cnt; i++){
        fprintf(out, " %s", ids[i]);
    }
    fprintf(out, "\n");
    free(ids);
}
// cauta nodul terminal al cuvantului
// numara fisierele din lista files si aloca un vector de pointeri la id-urile elem
// afiseaza numarul si id-urile sortate

void opTopK(Tree *root, FILE *out, char *word, int k) {
    Tree *node = trieFind(root, word);

    if (node == NULL || node->files == NULL){
        fprintf(out, "EMPTY\n");
        return;
    }
    int cnt = 0;
    FileRef *ref = node->files;

    while(ref != NULL){
        cnt++;
        ref = ref->next;
    }
    Heap *h = createHeap(cnt);
    ref = node->files;

    while(ref != NULL){
        heapInsert(h, ref->file);
        ref = ref->next;
    }
    int limit;
    if (k < cnt){
        limit = k;
    }else{
        limit = cnt;
    }
    fprintf(out, "%d", limit);

    for(int i=0; i < limit; i++){
        fprintf(out, " %s", heapExtract(h)->id);
    }
    fprintf(out, "\n");
    freeHeap(h);
}

void printDFS(Tree *node, char *buf, int depth, FILE *out) {
    if (node == NULL){
        return;
    }

    buf[depth] = node->ch;
    if (node->is_terminal == 1){
        buf[depth+1] = '\0';
        int cnt = 0;
        FileRef *ref=node->files;
        while(ref != NULL){
            cnt++;
            ref = ref->next;
        }
        char **ids = malloc(cnt * sizeof(char *));
        ref = node->files;

        for(int i=0; i < cnt; i++){
            ids[i] = ref->file->id;
            ref = ref->next;
        }

        qsort(ids, cnt, sizeof(char *), cmpIds);
        fprintf(out, "%s %d", buf, cnt);

        for(int i=0; i<cnt; i++){
            fprintf(out, " %s", ids[i]);
        }

        fprintf(out, "\n");
        free(ids);
    }

    printDFS(node->child, buf, depth+1, out);
    printDFS(node->sibling, buf, depth, out);
}

void opPrint(Tree *root, FILE *out) {
    if (!root->child) { fprintf(out, "EMPTY\n"); return; }
    char buf[300];
    printDFS(root->child, buf, 0, out);
}

void collectFiles(Tree *node, File ***arr, int *cnt, int *cap) {
    if(node == NULL) return;
    if(node->is_terminal == 1){
        FileRef *ref = node->files;
        while(ref != NULL){
            int dup = 0;
            for(int i = 0; i < *cnt; i++){
                if((*arr)[i] == ref->file){
                    dup=1;
                    break;
                }
            }
            if(!dup){
                if(*cnt == *cap){
                    *cap*=2;
                    *arr=realloc(*arr, *cap*sizeof(File*));
                }
                (*arr)[(*cnt)++] = ref->file;
            }
            ref = ref->next;
        }
    }
    collectFiles(node->child, arr, cnt, cap);
    collectFiles(node->sibling, arr, cnt, cap);
}

int cmpFiles(const void *a, const void *b) {
    return strcmp((*(File **)a)->id, (*(File **)b)->id);
}
// functie de comparare pentru qsort

void opPrefix(Tree *root, FILE *out, const char *prefix) {
    Tree *cur = root;
    for (int i = 0; prefix[i]; i++){
        cur = findChild(cur, prefix[i]);
        if(cur == NULL){
            fprintf(out, "EMPTY\n");
            return;
        }
    }
    int cnt=0;
    int cap=16;
    File **arr = malloc(cap * sizeof(File *));
    if(cur->is_terminal == 1){
        FileRef *ref = cur->files;
        while(ref != NULL){
            if(cnt==cap){
                cap = cap * 2;
                arr = realloc(arr,cap*sizeof(File*));
            }
            arr[cnt++] = ref->file;
            ref = ref->next;
        }
    }
    collectFiles(cur->child, &arr, &cnt, &cap);

    if(!cnt){
        fprintf(out, "EMPTY\n");
        free(arr);
        return;
    }

    qsort(arr, cnt, sizeof(File *), cmpFiles);
    fprintf(out, "%d", cnt);

    for(int i = 0; i<cnt; i++){
        fprintf(out, " %s", arr[i]->id);
    }

    fprintf(out, "\n");
    free(arr);
}




int main(){
    FILE *in  = fopen("indexare.in",  "r");
    FILE *out = fopen("indexare.out", "w");
    FileList *list = createList();
    Tree *root = createTrieNode('\0');

    int p;
    fscanf(in, "%d\n", &p);

    char line[4096];
    while (p > 0 && fgets(line, sizeof(line), in)){
        int len = strlen(line);

        while (len>0 && (line[len-1]=='\n')){
            len--;
            line[len]='\0';
        }

        char cmd[20];
        int pos = 0;
        int delta = 0;

        sscanf(line, "%s%n", cmd, &pos);

        if(strcmp(cmd, "ADD") == 0){
            char id[101];
            int score;
            int t;

            sscanf(line + pos, " %s %d %d%n", id, &score, &t, &delta);

            pos += delta;
            char keywords[200][101];

            for (int i=0; i<t; i++){

                sscanf(line+pos," %s%n",keywords[i],&delta);
                pos += delta; 
            }
            opAdd(list, root, out, id, score, t, keywords);

        }else if (strcmp(cmd, "DEL") == 0){
            char id[101];
            sscanf(line+pos, " %s", id);
            opDel(list, root, out, id);

        }else if (strcmp(cmd, "ADDKW") == 0){
            char id[101];
            char word[101];
            sscanf(line+pos, " %s %s", id, word);
            opAddKw(list, root, out, id, word);

        }else if (strcmp(cmd, "DELKW") == 0){
            char id[101];
            char word[101];
            sscanf(line+pos, " %s %s", id, word);
            opDelKw(list, root, out, id, word);

        }else if (strcmp(cmd, "FIND") == 0){
            char word[101];
            sscanf(line+pos, " %s", word);
            opFind(root, out, word);

        }else if (strcmp(cmd, "TOPK") == 0){
            char word[101];
            int k;
            sscanf(line+pos, " %s %d", word, &k);
            opTopK(root, out, word, k);

        }else if (strcmp(cmd, "PRINT") == 0){
            opPrint(root, out);

        }else if (strcmp(cmd, "PREFIX") == 0){
            char prefix[101];
            sscanf(line+pos, " %s", prefix);
            opPrefix(root, out, prefix);
        }
        p--;
    }

    fclose(in); 
    fclose(out);
    return 0;
}
