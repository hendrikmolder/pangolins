#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  char* object;
  char* question;
  struct node *yes;
  struct node *no;
};

/* Print the tree top-down left-to-right */
void treePrint(FILE* fp, struct node *ptr) {
  if (ptr != NULL) {
    if (ptr->question) {
      fprintf(fp, "question: %s\n",
        ptr->question);
        // now print its children, left-to-right:
        treePrint(fp, ptr->no);
        treePrint(fp, ptr->yes);
    } else {
      fprintf(fp, "object: %s\n", ptr->object);
    }
  }
}

struct node* treeRead(FILE* fp) {
  char line[64];
  fgets(line, sizeof(line), fp);

  if (line == NULL) {
    return NULL;
  } else {
    /* Remove newline (\n) */
    int end = strlen(line) - 1;
    if (line[end] == '\n') line[end] = '\0';

    struct node* ptr = malloc(sizeof(struct node));

    /* If it is a question */
    if (strncmp(line, "question: ", 10) == 0) {
      char* question = malloc(sizeof(line));

      /* Remove "question: " */
      memmove(line, line+10, strlen(line));
      strcpy(question, line);

      ptr->question = question;
      ptr->no = treeRead(fp);
      ptr->yes = treeRead(fp);
    } else { /* If it is an object */
      char* object = malloc(sizeof(line));

      /* Remove "object: " */
      memmove(line, line+8, strlen(line));
      strcpy(object, line);

      ptr->object = object;
      ptr->no = NULL;
      ptr->yes = NULL;
    }

    return ptr;
  }
}

void freeTree(struct node* tree) {
  if (tree != NULL) {
    if (tree->object) {
      free(tree);
    } else {
      freeTree(tree->no);
      freeTree(tree->yes);
    }
  }
}

int main(int argc, char const *argv[]) {
  FILE* fp;
  fp = fopen("pango.tree", "r");

  /* Return error if there's a problem with opening the file */
  if (fp == NULL) {
    perror("Error opening file: pango.tree");
    return -1;
  }

  /* Initialise head */
  struct node* pangolin = malloc(sizeof(struct node));

  /* read tree from the file */
  pangolin = treeRead(fp);


  /* Close the file */
  fclose(fp);

  struct node* current = pangolin;
  struct node* previous;
  struct node* root = current;
  char input_buffer[64];
  char answer[64];

  /* Play until question is NULL i.e when we get to
   * an object
   */
  int finished = 0;
  while (finished == 0) {

    if (current->object) {
      printf("Is it %s?\n[yes/no]: ", current->object);
      fgets(answer, sizeof(input_buffer), stdin);
      printf("\n");

      if (strcmp(answer,"yes\n") == 0) {
        printf("I won!\n");
      } else {
        /* Ask for new object and question */
        char newObjectInput[64];
        char newQuestionInput[64];
        char newLocation[64];
        printf("You won! What were you thinking about? \nObject: [a/an] <string>\nObject: ");
        fgets(newObjectInput, sizeof(input_buffer), stdin);
        printf("\n");

        /* Remove newline (\n) */
        int objEnd = strlen(newObjectInput) - 1;
        if (newObjectInput[objEnd] == '\n') newObjectInput[objEnd] = '\0';

        printf("What's the question for %s?\nQuestion: <string>?\nQuestion: ", newObjectInput);
        fgets(newQuestionInput, sizeof(input_buffer), stdin);
        printf("\n");

        /* Remove newline (\n) */
        int qEnd = strlen(newQuestionInput) - 1;
        if (newQuestionInput[qEnd] == '\n') newQuestionInput[qEnd] = '\0';

        printf("What's the answer for %s?\n[yes/no]: ", newObjectInput);
        fgets(newLocation, sizeof(input_buffer), stdin);
        printf("\n");

        /* Remove newline (\n) */
        int lEnd = strlen(newLocation) - 1;
        if (newLocation[lEnd] == '\n') newLocation[lEnd] = '\0';

        /* Create new struct */
        struct node* newObject = malloc(sizeof(struct node));
        struct node* newQuestion = malloc(sizeof(struct node));

        newQuestion->question = newQuestionInput;
        newObject->object = newObjectInput;

        /* If location in yes */
        if (strcmp(newLocation, "yes") == 0) {
          /* Check if it is HEAD and keep track of it if it is */
          if (previous == NULL) {
            root = newQuestion;
            newQuestion->no = current;
            newQuestion->yes = newObject;
          } else {
            previous->no = newQuestion;
            newQuestion->no = current;
            newQuestion->yes = newObject;
          }

          /* Save new tree to file */
          FILE* fp;
          fp = fopen("pango.tree", "w+");
          treePrint(fp, root);
          fclose(fp);

        } else {
          if (previous == NULL) {
            root = newQuestion;
            newQuestion->no = newObject;
            newQuestion->yes = current;
          } else {
            previous->yes = newQuestion;
            newQuestion->no = newObject;
            newQuestion->yes = current;
          }

          /* Save new tree to file */
          FILE* fp;
          fp = fopen("pango.tree", "w+");
          treePrint(fp, root);
          fclose(fp);
        }
      }

      finished = 1;
    } else {
        char input_buffer[64];
        char answer[64];

        printf("%s\n[yes/no]: ", current->question);
        fgets(answer, sizeof(input_buffer), stdin);
        printf("\n");

        /* Remove newline (\n) */
        int answerEnd = strlen(answer) - 1;
        if (answer[answerEnd] == '\n') answer[answerEnd] = '\0';

        /* Check for invalid input */
        if (strcmp(answer, "yes") != 0 && strcmp(answer, "no") != 0) {
          printf("Input: %s\nERROR: Answer has to be yes or no.", answer);
          return -1;
        }

        if (strcmp(answer, "yes") == 0) {
          previous = current;
          current = current->yes;
        }
        else if (strcmp(answer, "no") == 0) {
          previous = current;
          current = current->no;
        }
      }
    }

  freeTree(root);
  freeTree(current);
  freeTree(pangolin);
  freeTree(previous);
  return 0;
}
