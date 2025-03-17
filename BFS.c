#include<stdio.h>
#include<stdlib.h>
#include<time.h>

//#include "state.h"
//#include "list.h"
//#include "node.h"
//#include "io.h"
////////////////////////////
#define BLANK_CHARACTER '0'

//this enumerates available movements in the game relative to the blank character
typedef enum Move {
    UP, DOWN, LEFT, RIGHT, //values for moving up, down, left, right, respectively
    NOT_APPLICABLE         //value assigned for initial and goal input states
} Move;

typedef struct State {
    Move action;           //action that resulted to `this` board state
    char board[3][3];      //resulting board configuration after applying action
} State;

/**
 * DESCRIPTION:
 *    This creates a state if `move` is a valid move of the `state` board state.
 * PARAMETERS:
 *    state - pointer to the initial state
 *    move  - action to be applied to the given state
 * RETURN:
 *    Returns a pointer to a new `State` after the move is applied. Returns NULL
 *    upon failure.
**/
State* createState(State *state, Move move) {
    State *newState = malloc(sizeof(State));

    //copy the board configuration of `state` to `newState`
    //while searching for the row and column of the blank character
    char i, j;        //used for traversing the 3x3 arrays
    char row, col;    //coordinates of the blank character

    for(i = 0; i < 3; ++i) {
        for(j = 0; j < 3; ++j) {
            if(state->board[i][j] == BLANK_CHARACTER) {
                row = i;
                col = j;
            }

            newState->board[i][j] = state->board[i][j];
        }
    }

    //test if the coordinates are valid after translation based on the move
    //if it is, swap the concerned board values to reflect the move
    if(move == UP && row - 1 >= 0) {
        char temp = newState->board[row - 1][col];
        newState->board[row - 1][col] = BLANK_CHARACTER;
        newState->board[row][col] = temp;
        newState->action = UP;
        return newState;
    }
    else if(move == DOWN  && row + 1 < 3) {
        char temp = newState->board[row + 1][col];
        newState->board[row + 1][col] = BLANK_CHARACTER;
        newState->board[row][col] = temp;
        newState->action = DOWN;
        return newState;
    }
    else if(move == LEFT  && col - 1 >= 0) {
        char temp = newState->board[row][col - 1];
        newState->board[row][col - 1] = BLANK_CHARACTER;
        newState->board[row][col] = temp;
        newState->action = LEFT;
        return newState;
    }
    else if(move == RIGHT && col + 1 < 3) {
        char temp = newState->board[row][col + 1];
        newState->board[row][col + 1] = BLANK_CHARACTER;
        newState->board[row][col] = temp;
        newState->action = RIGHT;
        return newState;
    }

    free(newState);
    return NULL;
}

/**
 * DESCRIPTION: This frees memory of `s` and is reassigned to NULL
**/
void destroyState(State **state) {
    free(*state);
    state = NULL;
}

/**
 * DESCRIPTION:
 *    A heuristic function that assigns h-cost to nodes. Lower values
 *    indicate the more closer to the goal.
 * PARAMETER:
 *    curr - the current board configuration
 *    goal - a pointer to the goal configuration of the board
 * RETURN:
 *    Returns a heuristic value greater than or equal to 0.
**/
int manhattanDist(State * const curr, State * const goal) {
    int x0, y0; //used for indexing each symbol in `curr`
    int x1, y1; //correspoinding row and column of symbol from curr[y0, x0] at `goal`
    int dx, dy; //change in x0 and x1, and y0 and y1, respectively
    int sum = 0;

    //for each symbol in `curr`
    for(y0 = 0; y0 < 3; ++y0) {
        for(x0 = 0; x0 < 3; ++x0) {
            //find the coordinates of the same symbol in `goal`
            for(y1 = 0; y1 < 3; ++y1) {
                for(x1 = 0; x1 < 3; ++x1) {
                    if(curr->board[y0][x0] == goal->board[y1][x1]) {
                        dx = (x0 - x1 < 0)? x1 - x0 : x0 - x1;
                        dy = (y0 - y1 < 0)? y1 - y0 : y0 - y1;
                        sum += dx + dy;
                    }
                }
            }
        }
    }

    return sum;
}

/**
 * DESCRIPTION:
 *    This checks whether the two given states match against each other. An alternative
 *    way is to check if the manhattan distance between these two states is zero, but it
 *    would take a more significant amount of time and computational resource
 * PARAMETERS:
 *    state1 - state to match against `state2`
 *    state2 - state to match against `state1`
 * RETURN:
 *    Returns 1 if states match, 0 otherwise.
**/
char statesMatch(State const *testState, State const *goalState) {
    char row = 3, col;

    while(row--) {
        col = 3;
        while(col--) {
            if(testState->board[row][col] != goalState->board[row][col])
                return 0;
        }
    }

    return 1;
}
///////////////////////////
/////////////////////////////
typedef struct Node Node;
typedef struct NodeList NodeList;

/**
 * DESCRIPTION:
 *    The node in the linked list. Note that as a convention, the previous
 *    node of the list's head is NULL and so is the next node of the list's
 *    tail.
**/
typedef struct ListNode {
    Node *currNode;
    struct ListNode *prevNode; //the node before `this` instance
    struct ListNode *nextNode; //the next node in the linked list
} ListNode;

/**
 * DESCRIPTION:
 *    Contains the linked list of nodes, as well as other list information.
**/
struct NodeList {
    unsigned int nodeCount;    //the number of nodes in the list
    ListNode *head;            //pointer to the first node in the list
    ListNode *tail;            //pointer to the last node in the list
};

/**
 * DESCRIPTION:
 *    A structure for holding the solution.
**/
typedef struct SolutionPath {
    Move action;
    struct SolutionPath *next;
} SolutionPath;

/**
 * DESCRIPTION:
 *    This function is used to deallocate a list of type `SolutionPath`.
**/
void destroySolution(SolutionPath **list) {
    SolutionPath *next;
    while(*list) {
        next = (*list)->next;
        free(*list);
        *list = next;
    }
    *list = NULL;
}

/**
 * DESCRIPTION:
 *    This function pushes a node to the list of nodes.
 * PARAMETER:
 *    node - the node to add to the list
 *    list - a pointer to the list pointer to add the node into
 * RETURN:
 *    Returns 1 on success, 0 on failure.
**/
char pushNode(Node *node, NodeList** const list) {
    if(!node)
        return 0;

    ListNode *doublyNode = malloc(sizeof(ListNode));
    if(!doublyNode)
        return 0;

    doublyNode->currNode = node;

    if(*list && !(*list)->nodeCount) {
        (*list)->head = doublyNode;
        (*list)->tail = doublyNode;
        doublyNode->nextNode = NULL;
        doublyNode->prevNode = NULL;
        ++(*list)->nodeCount;
        return 1;
    }

    if(*list == NULL) {
        *list = malloc(sizeof(NodeList));
        if(*list == NULL)
            return 0;

        (*list)->nodeCount = 0;
        (*list)->head = NULL;
        (*list)->tail = doublyNode;
    }
    else {
        (*list)->head->prevNode = doublyNode;
    }

    doublyNode->nextNode = (*list)->head;
    doublyNode->prevNode = NULL;
    (*list)->head = doublyNode;

    ++(*list)->nodeCount;

    return 1;
}

/**
 * DESCRIPTION:
 *    This detatchs the node at the tail of the list. The previous
 *    node of the next node to detatch will be set to NULL.
 * PARAMETER:
 *    list - the list to pop
 * RETURN:
 *    Returns the address of the dettatched node; NULL if the list
 *    is empty.
**/
Node* popNode(NodeList** const list) {
    if(!*list || (*list)->nodeCount == 0)
        return NULL;

    Node *popped = (*list)->tail->currNode;
    ListNode *prevNode = (*list)->tail->prevNode;

    //free the list node pointing to node to be popped
    free((*list)->tail);

    if((*list)->nodeCount == 1) {
        (*list)->head = NULL;
    }
	else {
		prevNode->nextNode = NULL;
	}

    (*list)->tail = prevNode;
    --(*list)->nodeCount;
    return popped;
}

/**
 * DESCRIPTION:
 *    This function adds a list of nodes to `list`. It connects
 *    the head node of the list to be appended, to the tail of the list
 *    to append to. Connecting a list to itself will be ignored. After
 *    the connection, `toAppend` will be assigned to NULL. Deallocation of
 *    nodes from `toAppend` will be handled from `list`. This is mainly
 *    to avoid dangling pointer, or calling free multiple times to the
 *    same address.
 * PARAMETERS:
 *    toAppend - the list to be appended
 *    list     - the list to append `toAppend` into
**/
void pushList(NodeList **toAppend, NodeList *list) {
    //if either of the list is NULL, the head of the list to be appended is NULL,
    //or the list points to the same starting node
    if(!*toAppend || !list || !(*toAppend)->head || (*toAppend)->head == list->head) {
        return;
    }

    //if the list to append to has currently no element
    if(!list->nodeCount) {
        list->head = (*toAppend)->head;
        list->tail = (*toAppend)->tail;
    }
    else {
        //connect the lists
        (*toAppend)->tail->nextNode = list->head;
        list->head->prevNode = (*toAppend)->tail;
		list->head = (*toAppend)->head;
    }

    //update list information
    list->nodeCount += (*toAppend)->nodeCount;

    free(*toAppend);
    *toAppend = NULL;
}

int totalCost(Node *); //forward declaration for the next function

/**
 * DESCRIPTION:
 *    This is a special node insertion function used for A*. Unlike `insertNode()`,
 *    this inserts the node in order based on the sum of the node's heuristic and
 *    path cost values. Node with the lowest value is sitted to the tail of the queue.
 * PARAMETERS:
 *    toAppend - the list to be appended
 *    list     - the list to append `toAppend` into
**/
void pushListInOrder(NodeList **toAppend, NodeList *list) {
    //if either of the list is NULL, the head of the list to be appended is NULL,
    //or the list points to the same starting node
    if(!*toAppend || !list || !(*toAppend)->head || (*toAppend)->head == list->head) {
        return;
    }

    //if the list to append to has currently no element
    if(!list->nodeCount) {
        pushNode(popNode(toAppend), &list);
    }

    ListNode *toAppendNode; //list node to place in `list`
    ListNode *listNode;     //for traversing each node in `list`
    Node *node;

     while((toAppendNode = (*toAppend)->head)) {
        listNode = list->head;

        while(listNode && totalCost(toAppendNode->currNode) < totalCost(listNode->currNode)) {
            listNode = listNode->nextNode;
        }

        ListNode *temp = toAppendNode->nextNode;

        //if the head node of `toAppend` is to be inserted after the current tail of `list`
        if(!listNode) {
            list->tail->nextNode = toAppendNode;
            toAppendNode->prevNode = list->tail;
            toAppendNode->nextNode = NULL;
            list->tail = toAppendNode;
        }
        else {
            //if if the head node of `toAppend` is to be inserted somewhere in `list` except before its head
            if(listNode->prevNode) {
                toAppendNode->prevNode = listNode->prevNode;
                toAppendNode->nextNode = listNode;
                listNode->prevNode->nextNode = toAppendNode;
                listNode->prevNode = toAppendNode;
            }
            //if the head node of `toAppend` is to be inserted before the head of `list`
            else {
                toAppendNode->nextNode = list->head;
                toAppendNode->prevNode = NULL;
                list->head->prevNode = toAppendNode;
                list->head = toAppendNode;
            }
        }

        (*toAppend)->head = temp;
        --(*toAppend)->nodeCount;
        ++list->nodeCount;
    }

    free(*toAppend);
    *toAppend = NULL;
}

/////////////////////////////
//////////////////////

/////////////////////
//////////////////////////
extern unsigned int nodesGenerated; //declared from main.c

/**
 * DESCRIPTION: Defines the node structure used to create a search tree
**/
typedef struct Node Node;
struct Node {
    unsigned int depth; //depth of the node from the root. For A* search,
                        //this will also represent the node's path cost
    unsigned int hCost; //heuristic cost of the node
    State *state;       //state designated to a node
    Node *parent;       //parent node
    NodeList *children; //list of child nodes
};

/**
 * DESCRIPTION:
 *    This function creates a node, initializes it with the
 *    following parameters, and sets its children to NULL.
 * PARAMETERS:
 *    d - depth of the node
 *    h - heuristic value of the node
 *    s - state assignated to the node
 *    p - parent node
 * RETURN:
 *    Returns a `Node` pointer to the dynamically allocated node,
 *    or NULL on failure.
**/
Node* createNode(unsigned int d, unsigned int h, State *s, Node *p) {
    Node *newNode = malloc(sizeof(Node));
    if(newNode) {
        newNode->depth = d;
        newNode->hCost = h;
        newNode->state = s;
        newNode->parent = p;
        newNode->children = NULL;
        ++nodesGenerated; //update counter
    }
    return newNode;
}

/**
 * DESCRIPTION:
 *    This function is used to deallocate all nodes in a tree, including
 *    the root node.
 * PARAMETER:
 *    node - the root node of the tree to deallocate
**/
void destroyTree(Node *node) {
    if(node->children == NULL) {
        free(node->state);
        free(node);
        return;
    }

    ListNode *listNode = node->children->head;
    ListNode *nextNode;

    while(listNode) {
        nextNode = listNode->nextNode;
        destroyTree(listNode->currNode);
        listNode = nextNode;
    }

    //free(node->state);
    free(node->children);
    free(node);
}

/**
 * DESCRIPTION:
 *    This function 'expands' the node, links it to its children, and updates the
 *    expansion counter.
 * PARAMETER:
 *    parent    - the node to expand and search children for
 *    goalState - pointer to the goal state where heuristic values of each child will
 *                be based on
 * RETURN:
 *    Returns a pointer to `NodeList` on success, NULL on failure.
**/
NodeList* getChildren(Node *parent, State *goalState) {
    NodeList *childrenPtr = NULL;
    State *testState = NULL;
    Node *child = NULL;

    //attempt to create states for each moves, and add to the list of children if true
    if(parent->state->action != DOWN && (testState = createState(parent->state, UP))) {
        child = createNode(parent->depth + 1, manhattanDist(testState, goalState), testState, parent);
        pushNode(child, &parent->children);
        pushNode(child, &childrenPtr);
    }
    if(parent->state->action != UP && (testState = createState(parent->state, DOWN))) {
        child = createNode(parent->depth + 1, manhattanDist(testState, goalState), testState, parent);
        pushNode(child, &parent->children);
        pushNode(child, &childrenPtr);
    }
    if(parent->state->action != RIGHT && (testState = createState(parent->state, LEFT))) {
        child = createNode(parent->depth + 1, manhattanDist(testState, goalState), testState, parent);
        pushNode(child, &parent->children);
        pushNode(child, &childrenPtr);
    }
    if(parent->state->action != LEFT && (testState = createState(parent->state, RIGHT))) {
        child = createNode(parent->depth + 1, manhattanDist(testState, goalState), testState, parent);
        pushNode(child, &parent->children);
        pushNode(child, &childrenPtr);
    }

    return childrenPtr;
}

/**
 * DESCRIPTION:
 *    This simply evaluates the node's total cost, i.e. path cost + heuristic value.
 *    Created to abstract the proccess and reduce code cluttering. Note that
 *    a node's path cost in a tree depends purely on the node's depth, so the node's
 *    depth will be representing the path cost (to save space).
 * PARAMETER:
      node - the node to get its total cost
 * RETURN:
 *    Retuns the integer sum of the node's path cost and heuristic value
**/
int totalCost(Node * const node) {
    return node->depth + node->hCost;
}
/////////////////////////

//external variables declared from main.c
extern unsigned int nodesExpanded;
extern unsigned int nodesGenerated;
extern unsigned int solutionLength;
extern double runtime;

/**
 * DESCRIPTION: This displays the '8-Puzzle Solver' ASCII art to the screen
**/
void welcomeUser(void) {
    //style from: http://patorjk.com/software/taag/#p=display&f=Standard&t=8-Puzzle%20Solver
    printf("\n\
   ___        ____                _        ____        _\n\
  ( _ )      |  _ \\ _   _ _______| | ___  / ___|  ___ | |_   _____ _ __\n\
  / _ \\ _____| |_) | | | |_  /_  / |/ _ \\ \\___ \\ / _ \\| \\ \\ / / _ \\ '__|\n\
 | (_) |_____|  __/| |_| |/ / / /| |  __/  ___) | (_) | |\\ V /  __/ |\n\
  \\___/      |_|    \\__,_/___/___|_|\\___| |____/ \\___/|_| \\_/ \\___|_|\n");
}

/**
 * DESCRIPTION: This displays the input instructions for the user to read
**/
void printInstructions(void) {
    printf(
        "------------------------------------------------------------------------\n"
        "Instructions:\n"
        "    Enter the initial and goal state of the 8-puzzle board. Input\n"
        "    either integers 0-8, 0 representing the space character, to assign\n"
        "    symbols toeach board[row][col].\n"
        "------------------------------------------------------------------------\n"
    );
}

/**
 * DESCRIPTION:
 *    This function fills `state` with non-repeating numbers from 0 to 9
**/
void inputState(State * const state) {
    state->action = NOT_APPLICABLE;
    char row, col;
    int symbol;

    // flags for input validation
    char isNumUsed[9] = { 0 };

    for(row = 0; row < 3; ++row) {
        for(col = 0; col < 3; ++col) {
            printf("    board[%i][%i]: ", row, col);

            // to prevent scanning newline from the input stream
            scanf("%i", &symbol);

            // check if input is a blank character or is a number greater than 0 and less than 9
            if(symbol >= 0 && symbol < 9) {
                // check if input is repeated
                if(!isNumUsed[symbol]) {
                    state->board[row][col] = symbol + '0';
                    isNumUsed[symbol] = 1;
                }
                else {
                    printf("    ERROR: Number %c is already used. Try again with different input.\n", symbol);
                    --col;
                }
            }
            else {
                printf("    ERROR: Invalid input. Enter a number from 0 to 8.\n");
                --col;
            }
        }
    }
    printf("\n");
}

/**
 * DESCRIPTION: This displays contents of `board` to the standard output
**/
void printBoard(char const board[][3]) {
    char row, col;

    for(row = 0; row < 3; ++row) {
        printf("+---+---+---+\n");
        for(col = 0; col < 3; ++col) {
            printf("| %c ", board[row][col]);
        }
        printf("|\n");
    }
    printf("+---+---+---+\n");
}

/**
 * DESCRIPTION:
 *    This function interprets numerical instructions of the move to make,
 *    to it's verbal counterpart to be displayed to the screen.
 * PARAMETER:
 *    solution - the solution path consisting a list of nodes from the root
 *               to the goal
**/
void printSolution(struct SolutionPath *path) {
	//check if solution exists
    if(!path) {
        printf("No solution found.\n");
        return;
    }

	//if the initial state is already the goal state
	if(!path->next) {
		printf("No moves needed. The initial state is already the goal state.\n");
		return;
	}

    printf("SOLUTION: (Relative to the space character)\n");

    //will use hash map to speed up the proccess a bit
    char *move[4] = { "UP", "DOWN", "LEFT", "RIGHT" };
    int counter = 1;

    //will be skipping the first node since it represents the initial state with no action
    for(path = path->next; path; path = path->next, ++counter) {
        printf("%i. Move %s\n", counter, move[path->action]);
    }

    printf(
        "DETAILS:\n"
        " - Solution length : %i\n"
        " - Nodes expanded  : %i\n"
        " - Nodes generated : %i\n"
        " - Runtime         : %g milliseconds\n"
        " - Memory used     : %i bytes\n", //only counting allocated `Node`s
        solutionLength, nodesExpanded, nodesGenerated, runtime, nodesGenerated * sizeof(Node));
}


///////////////////

////////////////////////

////////////////////////////

/////////////////////////



unsigned int nodesExpanded;  //number of expanded nodes
unsigned int nodesGenerated; //number of generated nodes
unsigned int solutionLength; //number of moves in solution
double runtime;              //elapsed time (in milliseconds)

SolutionPath* BFS_search(State *, State *);
//SolutionPath* AStar_search(State *, State *);

int main(void) {
    welcomeUser();           //display welcome message
    printInstructions();     //display instructions

    State initial;           //initial board state
    State goalState;         //goal board configuration

    //solution path of each search method
    SolutionPath *bfs;
    //SolutionPath *aStar;

    //input initial board state
    printf("INITIAL STATE:\n");
    inputState(&initial);

    //input the goal state
    printf("\nGOAL STATE:\n");
    inputState(&goalState);

    printf("INITIAL BOARD STATE:\n");
    printBoard(initial.board);

    printf("GOAL BOARD STATE:\n");
    printBoard(goalState.board);

    //perform A* search
    /*aStar = AStar_search(&initial, &goalState);
    printf("\n-------------------------- USING A* ALGORITHM --------------------------\n");
    printSolution(aStar);

    //reset the counters
    nodesExpanded = 0;
    nodesGenerated = 0;
    solutionLength = 0;
    runtime = 0; */

    //perform breadth-first search
    bfs = BFS_search(&initial, &goalState);
    printf("\n------------------------- USING BFS ALGORITHM --------------------------\n");
    printSolution(bfs);

    //free resources
    destroySolution(&bfs);
    //destroySolution(&aStar);

    return 0;
}

/**
 * DESCRIPTION:
 *    Our breadth-first search implemetation.
 * PARAMETERS:
 *    initial - address to the initial state
 *    goal    - address to the goal state
 * RETURN:
 *    Returns the solution in a linked list; NULL if the solution is not found.
**/
SolutionPath* BFS_search(State *initial, State *goal) {
    NodeList *queue = NULL;
    NodeList *children = NULL;
    Node *node = NULL;

    //start timer
    clock_t start = clock();

    //initialize the queue with the root node of the search tree
    pushNode(createNode(0, manhattanDist(initial, goal), initial, NULL), &queue);
    Node *root = queue->head->currNode; //for deallocating the generated tree

    //while there is a node in the queue to expand
    while(queue->nodeCount > 0) {
        //pop the last node (tail) of the queue
        node = popNode(&queue);

        //if the state of the node is the goal state
        if(statesMatch(node->state, goal))
            break;

        //else, expand the node and update the expanded-nodes counter
        children = getChildren(node, goal);
        ++nodesExpanded;

        //add the node's children to the queue
        pushList(&children, queue);
    }

    //determine the time elapsed
    runtime = (double)(clock() - start) / CLOCKS_PER_SEC;

    //get solution path in order from the root, if it exists
    SolutionPath *pathHead = NULL;
    SolutionPath *newPathNode = NULL;

    while(node) {
        newPathNode = malloc(sizeof(SolutionPath));
        newPathNode->action = node->state->action;
        newPathNode->next = pathHead;
        pathHead = newPathNode;

        //update the solution length and move on the next node
        ++solutionLength;
        node = node->parent;
    }

    --solutionLength; //uncount the root node

    //deallocate the generated tree
    destroyTree(root);

    return pathHead;
}

/**
 * DESCRIPTION:
 *    Our A* implemetation.
 * PARAMETERS:
 *    initial - address to the initial state
 *    goal    - address to the goal state
 * RETURN:
 *    Returns the solution in a linked list; NULL if the solution is not found.
**/
/*SolutionPath* AStar_search(State *initial, State *goal) {
    NodeList *queue = NULL;
    NodeList *children = NULL;
    Node *node = NULL;

    //start timer
    clock_t start = clock();

    //initialize the queue with the root node of the search tree
    pushNode(createNode(0, manhattanDist(initial, goal), initial, NULL), &queue);
    Node *root = queue->head->currNode; //for deallocating generated tree

    //while there is a node in the queue to expand
    while(queue->nodeCount > 0) {
        //pop the last node (tail) of the queue
        node = popNode(&queue);

        //if the state of the node is the goal state
        if(statesMatch(node->state, goal))
            break;

        //else, expand the node and update the expanded-nodes counter
        children = getChildren(node, goal);
        ++nodesExpanded;

        //add the node's children to the queue
        pushListInOrder(&children, queue);
    }

    //determine the time elapsed
    runtime = (double)(clock() - start) / CLOCKS_PER_SEC;

    //get solution path in order from the root, if it exists
    SolutionPath *pathHead = NULL;
    SolutionPath *newPathNode = NULL;

    while(node) {
        newPathNode = malloc(sizeof(SolutionPath));
        newPathNode->action = node->state->action;
        newPathNode->next = pathHead;
        pathHead = newPathNode;

        //update the solution length and move on the next node
        ++solutionLength;
        node = node->parent;
    }

    --solutionLength; //uncount the root node

    //deallocate the generated tree
    destroyTree(root);

    return pathHead;
}*/



/*
1	2	3
8	0	4
7	6	5

1	3	4
8	6	2
7	0	5

0*/
