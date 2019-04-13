#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
typedef struct tree_node{
  int children_no;
  char name;
  struct tree_node * children;
}tree_node;

void print_tree(tree_node * root);
void print_tree_recursive(tree_node * root, int level);
void read_tree_file(const char *filename);
tree_node * get_children();
void parse_file();
void create_processes(tree_node * parent);


FILE * fp;

tree_node * root;



void create_processes(tree_node * parent)
{
  printf("Starting process %c with pid = %d\n",parent->name,getpid());
  int i;
  int j;
  pid_t pids[parent->children_no];
  
  
  for (i = 0; i< parent->children_no; i++)
  {
    pids[i] = fork();
    if(pids[i] < 0)
      {
        printf("There is an error with process %c \n", parent->name);
      }
    else if (pids[i] == 0)
    {
      create_processes((tree_node *)parent->children + sizeof(tree_node)*i);
    }
  }
  printf("Process %c is waiting on its children to end\n", parent->name);
  for (j = 0; j < parent->children_no; j++){
      int status;
      if(pids[j] > 0){
        waitpid(pids[j],&status,0);
        printf("Process with pid %d has ended\n", pids[j]);
        //fprintf(fileOut, "Process %c has ended and returned: %d\n", processName, WEXITSTATUS(status));
      }
  }
  sleep(6);
  printf("Process %c is ending itself\n",parent->name);
  exit(0);
}


void read_tree_file(const char *filename)
{
  fp = fopen(filename, "r");
  parse_file();
  create_processes(root);
  //print_tree(root);
  //printf("Starting process %c with pid = %d\n",processName,getpid());
  
  
}

void parse_file()
{
  char buffer[255];
  
  fgets(buffer,255,fp); //get first line
  
  tree_node * node = (tree_node *)malloc(sizeof(tree_node));//allocate space for first node
  
  
  int numChilds = buffer[2] - '0';//convert char to int
  char name = buffer[0];
  
  node->name = name;
  node->children_no = numChilds; //copy int to struct
  root = node; //set root node as first line parsed
  
  if (root->children_no > 0)
  {
    int i;
    root->children = (tree_node *) malloc(sizeof(tree_node) * root->children_no);
    //printf("Root children start at %p\n", root->children);
    for (i = 0; i< numChilds; i++)
    {
    tree_node * child = get_children(root);
    memcpy(root->children + sizeof(tree_node)*i, child, sizeof(tree_node));
    }
  } 
}

tree_node * get_children()//perform DFS and return popluate child node
{
  char buffer[255];
  int i;
  fgets(buffer,255,fp);
  
  
  //declare and initialize child node struct
  tree_node * parent = (tree_node *)malloc(sizeof(tree_node));
  int numChilds = buffer[2] - '0';//convert char to int
  char name = buffer[0];
  parent->children_no = numChilds; //copy int to struct
  parent->name = name;
  
  
  
  
  if (numChilds > 0)
  {
    parent->children = (tree_node *) malloc(sizeof(tree_node) * numChilds);
    for (i= 0; i< numChilds; i++)
    { 
    //printf("Parent %c's children start at %p\n", parent->name,parent->children);
    tree_node * child = get_children();
    memcpy(parent->children + sizeof(tree_node)*i,child, sizeof(tree_node));
    }
  }
 return parent;
    
}

void print_tree(tree_node * root)
{
  print_tree_recursive(root, 0);
}

void print_tree_recursive(tree_node * root, int level)
{
   int i = 0;
   for (i = 0; i<level;i++)
   {
     printf("    ");
   } 
   printf("%c\n", root->name);
   
   for (i =0; i< root->children_no; i++)
   {
     print_tree_recursive((tree_node *)(root->children + i*sizeof(tree_node)) , level + i + 1);
   }
} 

int main(int argc, char * argv[])
{
  if (argc != 2)
  {
    puts("Error please give filepath as a console arguement");
  }
  read_tree_file(argv[1]);
  
}