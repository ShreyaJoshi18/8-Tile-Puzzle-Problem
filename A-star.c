#include<stdio.h>
#include<malloc.h>
#include<string.h>

typedef struct node_array
{
    int a[3][3];
    int depth;
    int f;
    char text[11];
    struct node_array *shift_top;
    struct node_array *shift_down;
    struct node_array *shift_left;
    struct node_array *shift_right;
    struct node_array *parent;
    struct node_array *next;
} node;

node *front=NULL,*rear=NULL,*front_list=NULL,*rear_list=NULL;

int goal[3][3],depth_flag=0,f_count=0;

void set_zero(int a[3][3])
{
    int i,j;
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            a[i][j]=0;
}

void copy_array(int a[3][3],int b[3][3])//copy b to a
{
    int i,j;
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            a[i][j]=b[i][j];
}

int is_equal(int a[3][3],int b[3][3])
{
    int i,j,flag=1;
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            if(a[i][j]!=b[i][j])
                flag=0;
    return flag;
}

void swap(int *a,int *b)
{
    int temp;
    temp=*a;
    *a=*b;
    *b=temp;
}

void print_array(int a[3][3])
{
    int i,j;
    for(i=0;i<3;i++)
    {                for(j=0;j<3;j++)
            printf("%d ",a[i][j]);
        printf("\n");
    }
}

int check_zero(int a[3][3])//returns 1 if 0
{
    int i,j,flag=1;
    for(i=0;i<3;i++)
    {                for(j=0;j<3;j++)
            if(a[i][j]!=0)
                flag=0;
    }
    return flag;
}

int check_list(node *nd)//returns 0 if it doesnt match the list
{
    node *new_node=NULL;
    if(front_list==NULL)
        return 0;
    else
    {
        new_node=front_list;
        while(new_node!=NULL)
        {
            if(is_equal(new_node->a,nd->a)==1)
                return 1;
            new_node=new_node->next;
        }
        return 0;
    }
}


void insert_queue(node *nd)
{
    nd->next=NULL;
    if(front==NULL)
    {
        front=nd;
        rear=nd;
    }
    else
    {
        rear->next=nd;
        rear=nd;
        rear->next=NULL;
    }
}

void insert_list(node *nd)
{
    nd->next=NULL;
    if(front_list==NULL)
    {
        front_list=nd;
        rear_list=nd;
    }
    else
    {
        rear_list->next=nd;
        rear_list=nd;
        rear_list->next=NULL;
    }
}


int mod(int a)
{
    if(a>=0)
        return a;
    else
        return (-1*a); // To make cost positive
}

int calc_heuristic(node *nd)
{
    int i,j,k,l,h=0;
    for(i=0;i<=2;i++)
    {
        for(j=0;j<=2;j++)
        {
            if(nd->a[i][j]!=0)
            {for(k=0;k<=2;k++)
                {
                    for(l=0;l<=2;l++)
                    {
                        if(nd->a[i][j]==goal[k][l])
                            goto xy;
                    }
                }
                xy:
                h+=(mod(i-k)+mod(j-l)); //Manhatton Distance
                // Those tiles which are matching with the goal state tiles
            }
        }
    }
    return h;
}


void next_move(node *nd)
{
    int i,j,x,y;
    for(i=0;i<3;i++)
    {
        for(j=0;j<3;j++)
        {
            if(nd->a[i][j]==0)  //searching for blank tile coordinates
                goto label;
        }
    }
    label:
    // Blank tile coordinaes
    x=i;
    y=j;
    if(y+1 >2)
        nd->shift_right=NULL;  //This move is not possible
    else
    {
        nd->shift_right=(node*)malloc(sizeof(node));
        copy_array(nd->shift_right->a,nd->a);
        swap(&nd->shift_right->a[x][y],&nd->shift_right->a[x][y+1]);
        if(check_list(nd->shift_right)==1)
        {
            nd->shift_right=NULL;
            free(nd->shift_right);
        }
        else
        {
            nd->shift_right->parent=nd;
            nd->shift_right->depth=nd->depth+1;
            strcpy(nd->shift_right->text,"move right");
            nd->shift_right->f=nd->depth+calc_heuristic(nd->shift_right)+1; // f(n) =g(n) +h(n)
            insert_list(nd->shift_right);
            insert_queue(nd->shift_right);
        }
    }
    if(y-1 <0)
        nd->shift_left=NULL; //This move is not possible
    else
    {
        nd->shift_left=(node*)malloc(sizeof(node));
        copy_array(nd->shift_left->a,nd->a);
        swap(&nd->shift_left->a[x][y],&nd->shift_left->a[x][y-1]);
        if(check_list(nd->shift_left)==1)
        {
            nd->shift_left=NULL;
            free(nd->shift_left);
        }
        else
        {
            nd->shift_left->parent=nd;
            strcpy(nd->shift_left->text,"move left");
            nd->shift_left->depth=nd->depth+1;
            nd->shift_left->f=nd->depth+calc_heuristic(nd->shift_left)+1;// f(n) =g(n) +h(n)
            insert_list(nd->shift_left);
            insert_queue(nd->shift_left);
        }
    }
    if(x+1 >2)
        nd->shift_down=NULL; //This move is not possible
    else
    {
        nd->shift_down=(node*)malloc(sizeof(node));
        copy_array(nd->shift_down->a,nd->a);
        swap(&nd->shift_down->a[x][y],&nd->shift_down->a[x+1][y]);
        if(check_list(nd->shift_down)==1)
        {
            nd->shift_down=NULL;
            free(nd->shift_down);
        }
        else
        {
            nd->shift_down->parent=nd;
            strcpy(nd->shift_down->text,"move down");
            nd->shift_down->depth=nd->depth+1;
            nd->shift_down->f=nd->depth+calc_heuristic(nd->shift_down)+1;// f(n) =g(n) +h(n)
            insert_list(nd->shift_down);
            insert_queue(nd->shift_down);
        }
    }
    if(x-1 <0)
        nd->shift_top=NULL; //This move is not possible
    else
    {
        nd->shift_top=(node*)malloc(sizeof(node));
        copy_array(nd->shift_top->a,nd->a);
        swap(&nd->shift_top->a[x][y],&nd->shift_top->a[x-1][y]);

        if(check_list(nd->shift_top)==1)
        {
            nd->shift_top=NULL;
            free(nd->shift_top);
        }
        else
        {
            nd->shift_top->parent=nd;
            strcpy(nd->shift_top->text,"move up");
            nd->shift_top->depth=nd->depth+1;
            nd->shift_top->f=nd->depth+calc_heuristic(nd->shift_top)+1;// f(n) =g(n) +h(n)
            insert_list(nd->shift_top);
            insert_queue(nd->shift_top);
        }
    }
}
// Function to check if initial state is the goal state
int is_goal(node *nd) //return 1 if goal
{
    return is_equal(nd->a,goal);
}

node *pop_queue()
{
    node *nd;
    nd=front;
    front=front->next;
    if(front==NULL)
        rear=NULL;
    return nd;
}

void final_print(node *nd)
{
    while(nd!=NULL)
    {
        printf("\n");
        print_array(nd->a);
        printf("\n\n%s\n",nd->text);
        nd=nd->parent; // for path

        f_count++;
    }
    printf("\nNo of steps %d",f_count-1);
}

int count_queue()
{
    int count=0;
    node *temp;
    temp=front;
    while(temp!=NULL)
    {
        temp=temp->next;
        count++;
    }
    return count;
}
void arrange()
{
    node *t1,*t2,*t3;
    int i,j;
    i=count_queue();
    j=i;
    for(int k=0;k<i;k++)
    {
        t1=front; //initial
        t2=front->next; //left

        for(int l=1;l<j;l++)
        {
            if(t1->next->f > t2->next->f)  //t2->left is middle
            {
                t3=t2; //t3 is left
                t1->next=t2->next; // put middle value to left
                t3->next=t3->next->next;
                t1->next->next=t2;
            }
            j--;
            t1=t1->next;
            t2=t2->next;
        }
    }
    t1=front;
    while(t1->next!=NULL)
        t1=t1->next;
    rear=t1;
    rear_list=t1;
}


void A_star(node *root)
{

    {
        node *nd;
        insert_queue(root);
        insert_list(root);
        nd=root;
        while(1)
        {
            nd=front;
            if(is_goal(nd)==1)
            {
                final_print(nd);
                break;
            }
            next_move(nd);
            arrange(); // function acting as  priority queue
            nd=pop_queue(); //popping live node
        }
    }
}





int main()
{
    node *n;
    int i,j;
    n=(node*)malloc(sizeof(node));

    printf("Enter the goal state\n");
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            scanf("%d",&n->a[i][j]);

    n->parent=NULL;
    n->next=NULL;
    n->shift_top=NULL;
    n->shift_down=NULL;
    n->shift_left=NULL;
    n->shift_right=NULL;
    n->depth=0;


    printf("Enter the current state\n");
    for(i=0;i<3;i++)
        for(j=0;j<3;j++)
            scanf("%d",&goal[i][j]);

    n->f=calc_heuristic(n);
    printf("\nThe seq of steps are\n",f_count);
    A_star(n);

    return 0;
}

