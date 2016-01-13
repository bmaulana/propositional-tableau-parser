#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

int Fsize=50;
int inputs=6;


int i;
int j;

/*typedef struct tableau tableau;*/

struct tableau {
  char *root;
  struct tableau *left;
  struct tableau *right;
  struct tableau *parent;
}*tab, *node, *node1, *kid, *pa;

/*put all your functions here.  You will need
1.
int parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
2.
void complete(struct tableau *t)
which expands the root of the tableau and recursively completes any child tableaus.
3.
int closed(struct tableau *t)
which checks if the whole tableau is closed.
Of course you will almost certainly need many other functions.

You may vary this program provided it reads 6 formulas in a file called "input.txt" and outputs in the way indicated below to a file called "output.txt".
*/

/* List processing methods  */

char *mytail(char *list) {
    /*given non-empty string, returns string without the first char*/
    char *newlist = (char*) malloc(strlen(list) * sizeof(char));
    int id = 0;
    while(list[id+1] != '\0'){
        newlist[id] = list[id+1];
        id++;
    }
    newlist[id] = '\0';
    return newlist;
}

char *segment(char *list, int i, int j) {
    /* characters from pos i up to j-1, provided i<=j*/
    char *seg = (char*) malloc(j-i+1 * sizeof(char));
    int a = 0;
    for (a = 0; a < (j-i); a++) {
        char n = list[i+a];
        seg[a]= n;
    }
    seg[j-i] = '\0';
    return seg;
}

char *addneg(char *list) {
    //adds negation in front of a string
    char *newlist = (char*) malloc((strlen(list)+2) * sizeof(char));
    newlist[0] = '-';
    int id = 1;
    while(list[id-1] != '\0') {
        newlist[id] = list[id-1];
        id++;
    }
    newlist[id] = '\0';
    return newlist;
}

/* Basics.  Recognise propositions and connectives.  */

int prop(char x) {
    if((x=='p')||(x=='q')||(x=='r')||(x=='s')) return(1);
    else return(0);
}

int bc(char x) {
    if ((x=='v')||(x=='^')||(x=='>')) return(1);
    else return(0);
}


/* The actual parsing methods.  */

char bin(char *g) {
    /*for binary connective formulas, returns binary connective*/
    int brackets = 0, i = 0;
    while(1) {
        char c = g[i];
        if(c=='\0') { return '0'; }
        if(c=='(') { brackets += 1; }
        if(c==')') { brackets -= 1; }
        if(bc(c) && brackets==1) { return c; }
        i++;
    }
}

char *partone(char *g) {
    /* for binary connective formulas, returns first part*/
    int brackets = 0, i = 0;
    while(1) {
        char c = g[i];
        if(c=='(') { brackets += 1; }
        if(c==')') { brackets -= 1; }
        if(bc(c) && brackets==1) { return segment(g,1,i); }
        i++;
    }
}

char *parttwo(char *g) {
    /* for binary connective formulas, returns second part*/
    int brackets = 0, i = 0;
    while(1) {
        char c = g[i];
        if(c=='(') { brackets += 1; }
        if(c==')') { brackets -= 1; }
        if(bc(c) && brackets==1) { return segment(g,i+1,strlen(g)-1); }
        i++;
    }
}

int parse(char *g) {
    /* return 1 if a proposition, 2 if neg, 3 if binary, ow 0*/
    //Proposition
    if(prop(g[0]) && g[1] == '\0') {
        return 1;
    }

    //Negation
    if(g[0] == '-') {
        if(parse(mytail(g)) != 0) {
            return 2;
        }
    }

    //Binary
    if(g[0] == '(' && g[strlen(g)-1] == ')') {
        if(bin(g) != '0') {
            if(parse(partone(g)) != 0 && parse(parttwo(g)) != 0) {
                return 3;
            }
        }
    }

    return 0;
}

int type(char *g) {
    /*return 0 if not a formula, 1 for literal, 2 for alpha, 3 for beta, 4 for double negation*/
    if (parse(g)==1 || (parse(g)==2)&& parse(mytail(g))==1) return(1);
    if (parse(g)==3) {
        /*binary fmla*/
        switch(bin(g)) {
            case('v'): return(3); break;
            case('^'): return(2); break;
            case('>'): return(3); break;
            default: return(0); //should be unreachable
        }
    }
    if ((parse(g)==2)&& (parse(mytail(g))==2)) return(4); /*double neg*/

    if ((parse(g)==2)&& parse(mytail(g))==3) {
        /*negated binary*/
        switch(bin(mytail(g))) {
            case('v'): return(2); break;
            case('^'): return(3); break;
            case('>'): return(2); break;
        }
    }
    return(0);
}

char *firstexp(char *g) {
    /* for alpha and beta formulas*/
    if (parse(g)==3) {
        /*binary fmla*/
        switch(bin(g)) {
            case('v'): return(partone(g)); break;
            case('^'): return(partone(g)); break;
            case('>'): return(addneg(partone(g))); break;
            default: return(0); //should be unreachable
        }
    }
    if ((parse(g)==2)&& (parse(mytail(g))==2)) return(mytail(mytail(g))); /*double neg*/ /*throw away first two chars*/

    if ((parse(g)==2)&&parse(mytail(g))==3) {
        /*negated binary*/
        switch(bin(mytail(g))) {
            case('v'): return(addneg(mytail(partone(g))));break;
            case('^'): return(addneg(mytail(partone(g))));break;
            case('>'): return(mytail(partone(g)));break;
        }
    }
    return(0);
}


char *secondexp(char *g)
{/* for alpha and beta formulas, but not for double negations, returns the second expansion formula*/
    if (parse(g)==3) {
        /*binary fmla*/
        switch(bin(g)) {
            case('v'): return(parttwo(g)); break;
            case('^'): return(parttwo(g)); break;
            case('>'): return(parttwo(g)); break;
            default: return(0); //should be unreachable
        }
    }
    if ((parse(g)==2)&&parse(mytail(g))==3) {
        /*negated binary*/
        switch(bin(mytail(g))) {
            case('v'): return(addneg(parttwo(g)));break;
            case('^'): return(addneg(parttwo(g)));break;
            case('>'): return(addneg(parttwo(g)));break;
        }
    }
    return(0);
}

//Tableau methods

void new_node_l(struct tableau *t, char *g) {
    //adds g as a left child to a leaf node.
    struct tableau *left = malloc(sizeof(struct tableau));
    left->root = g;
    left->parent = t;
    left->left = NULL;
    left->right = NULL;
    (*t).left = left;
}

void new_node_r(struct tableau *t, char *g) {
    //adds g as a right child to a leaf node.
    struct tableau *right = malloc(sizeof(struct tableau));
    right->root = g;
    right->parent = t;
    right->left = NULL;
    right->right = NULL;
    (*t).right = right;
}

void add_one(struct tableau *t, char *g) {
    /* adds g as left child at every leaf below*/
    if((*t).left == NULL) { new_node_l(t, g); }
    else { add_one((*t).left, g); }
    if((*t).right != NULL) { add_one((*t).right, g); }
}

void alpha(struct tableau *t, char *g, char *h) {
    /*not for double negs, adds g then h at every leaf below*/
    add_one(t, g);
    add_one(t, h);
}

void beta(struct tableau *t, char *g, char *h) {
    /*for beta s, adds g, h on separate branches at every leaf below*/
    if((*t).left == NULL) {
        new_node_l(t, g);
        new_node_r(t, h);
    } else {
        beta((*t).left, g, h);
        if((*t).right != NULL) { beta((*t).right, g, h); }
    }
}

void expand(struct tableau *t) {
/*must not be null.  Checks the root.  If literal, does nothing.  If beta calls add_two with suitable fmlas, if alpha calls alpha with suitable formulas unless a double negation then … */
    char* root = (*t).root;
    switch(type(root)) {
        case(2): alpha(t,(char*) firstexp(root), (char*) secondexp(root)); break;
        case(3): beta(t,(char*) firstexp(root), (char*) secondexp(root)); break;
        case(4): (*t).root=firstexp(root); expand(t); break; //double negation
        default: break; //literals and non-formulas
    }
}

void complete(struct tableau *t) {
    if (t!=NULL) {
        expand(t);
        complete((*t).left);
        complete((*t).right);
    }
}

int check(struct tableau *t, int *literals) {
    //helper method for closed
    char* root = (*t).root;
    int newliterals[8];
    memcpy(newliterals, literals, 8 * sizeof(int));

    if(parse(root) == 1) {
        //proposition
        switch(root[0]) {
            case 'p': newliterals[0] = 1; break;
            case 'q': newliterals[1] = 1; break;
            case 'r': newliterals[2] = 1; break;
            case 's': newliterals[3] = 1; break;
        }
    }
    if(parse(root) == 2 && parse(mytail(root)) == 1) {
        //negated proposition
        switch(root[1]) {
            case 'p': newliterals[4] = 1; break;
            case 'q': newliterals[5] = 1; break;
            case 'r': newliterals[6] = 1; break;
            case 's': newliterals[7] = 1; break;
        }
    }

    //return closed if prop and -prop exists in branch
    int i;
    for(i = 0; i < 4; i++) {
        if(newliterals[i] == 1 && newliterals[i+4] == 1) { return 1; }
    }

    if((*t).left == NULL && (*t).right == NULL) { return 0; }
    if((*t).left != NULL && (*t).right == NULL) { return check((*t).left, newliterals); }
    if((*t).left != NULL && (*t).right != NULL) {
        int left = check((*t).left, newliterals);
        int right = check((*t).right, newliterals);
        return left && right;
    }
    return 0; //should be unreachable
}

int closed(struct tableau *t) {
    //checks if tableau is closed, returns 1 if it is, 0 otherwise
    int literals[8] = {0,0,0,0,0,0,0,0}; //p, q, r, s, -p, -q, -r, -s
    char* root = (*t).root;

    if(parse(root) == 0) { return 1; } //non-formula: non satisfiable
    if(parse(root) == 1) {
        //proposition
        switch(root[0]) {
            case 'p': literals[0] = 1; break;
            case 'q': literals[1] = 1; break;
            case 'r': literals[2] = 1; break;
            case 's': literals[3] = 1; break;
        }
    }
    if(parse(root) == 2 && parse(mytail(root)) == 1) {
        //negated proposition
        switch(root[1]) {
            case 'p': literals[4] = 1; break;
            case 'q': literals[5] = 1; break;
            case 'r': literals[6] = 1; break;
            case 's': literals[7] = 1; break;
        }
    }

    if((*t).left == NULL && (*t).right == NULL) { return 0; }
    if((*t).left != NULL && (*t).right == NULL) { return check((*t).left, literals); }
    if((*t).left != NULL && (*t).right != NULL) {
        int left = check((*t).left, literals);
        int right = check((*t).right, literals);
        return left && right;
    }
    return 0; //should be unreachable
}

//main

int main()
{ /*input 6 strings from "input.txt" */
  char *names[inputs];/*to store each of the input strings*/

  for (i=0;i<inputs;i++) names[i]=malloc(Fsize);/*create enough space*/



  FILE *fp, *fpout, *fopen();

  if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
  if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}/*ouputs to be sent to "output.txt"*/

  fscanf(fp,"%s %s %s %s %s %s",names[0],names[1], names[2], names[3],names[4],names[5]);/*read input strings from "input.txt"*/

  /*lets check your parser*/
  for(i=0;i<inputs;i++)
    {j=parse(names[i]);
      switch(j)
	{
	case(0):fprintf(fpout,"%s is not a formula\n", names[i]);break;
	case(1):fprintf(fpout,"%s is a proposition\n",names[i]);break;
	case(2):fprintf(fpout,"%s is a negation\n",names[i]);break;
	case(3):fprintf(fpout,"%s is a binary formula\n",names[i]);break;
	default:fprintf(fpout,"%s is not a formula\n",names[i]);break;
	}
    }

  /*make 6 new tableaus each with name at root, no children, no parent*/

  struct tableau tabs[inputs];

  for(i=0;i<inputs;i++)
    {
      tabs[i].root=names[i];
      tabs[i].parent=NULL;
      tabs[i].left=NULL;
      tabs[i].right=NULL;

      /*expand each tableau until complete, then see if closed */

     complete(&tabs[i]);
      if (closed(&tabs[i])) fprintf(fpout,"%s is not satisfiable\n", names[i]);
      else fprintf(fpout,"%s is satisfiable\n", names[i]);
    }

  fclose(fp);
  fclose(fpout);

  return(0);
}
