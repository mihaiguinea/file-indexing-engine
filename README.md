Tema 2 - Sistem de indexare a fisierelor

Structuri:

*Lista dublu inlantuita - retine fisierele in ordinea adaugarii. Fiecare
nod are id, scor, lista de cuvinte-cheie si pointeri prev/next.

*Trie (arbore multicai de regasire) - retine cuvintele-cheie caracter cu
caracter. Fiecare nod are caracterul sau, un pointer la primul copil (child) si un pointer la fratele urmator (sibling). Copiii sunt mentinuti in ordine alfabetica. Nodurile terminale contin o lista de referinte catre fisierele care au acel cuvant.

*Max-Heap - folosit doar la TOPK. Prioritatea e data de scorul fisierului, iar la scoruri egale de ordinea lexicografica a id-ului.


Operatii:

ADD - adauga fisierul in lista si cuvintele in trie
DEL - sterge referintele din trie apoi fisierul din lista
ADDKW - adauga cuvantul in lista fisierului si in trie
DELKW - sterge referinta din trie si cuvantul din lista fisierului
FIND - cauta cuvantul in trie, afiseaza fisierele sortat lexicografic
TOPK - construieste un heap cu fisierele gasite, extrage primele k
PRINT - parcurgere DFS a trieului, afiseaza cuvintele in ordine lexicografica
PREFIX - coboara in trie pe prefix, colecteaza toate fisierele din subarbore
