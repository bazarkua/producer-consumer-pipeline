
/* I used code example provided on canvas page under 
Assignment4 description for my implemetation this is 
the link of example code provided from canvas page: https://replit.com/@cs344/65prodconspipelinec */


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>

#define LENGTH 1000

int num_lines = 0;

/* Buffer 1, for input */
char *buffer_1[LENGTH];
/* Number of items in the buffer */
int count_1 = 0;
int prod_idx_1 = 0;
int con_idx_1 = 0;
/* Initialize the mutex for buffer 1 */
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 1 */
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;

/* Buffer 2, shared resource between input thread and space delim thread */
char *buffer_2[LENGTH];
/* Number of items in the buffer */
int count_2 = 0;
/* Index where the space delim thread will put the next item */
int prod_idx_2 = 0;
/* Index where the space delim thread will pick up the next item */
int con_idx_2 = 0;
/* Initialize the mutex for buffer 2 */
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 2 */
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

/* Buffer 3, shared resource between space delim thread and replacement thread */
char *buffer_3[LENGTH];
/* Number of items in the buffer */
int count_3 = 0;
/* Index where the replacement thread will put the next item */
int prod_idx_3 = 0;
/* Index where the replacement thread will pick up the next item */
int con_idx_3 = 0;
/* Initialize the mutex for buffer 3 */
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 3 */
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/* Buffer 4, shared resource between replacemnt thread and output thread */
char *buffer_4[LENGTH];
/* Number of items in the buffer */
int count_4 = 0;
/* Index where the output thread will put the next item */
int prod_idx_4 = 0;
/* Index where the output thread will pick up the next item */
int con_idx_4 = 0;
/* Initialize the mutex for buffer 4 */
pthread_mutex_t mutex_4 = PTHREAD_MUTEX_INITIALIZER;
/* Initialize the condition variable for buffer 4 */
pthread_cond_t full_4 = PTHREAD_COND_INITIALIZER;

/*
 Put an item in buff_1
*/
void put_buff_1(char *item)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_1);
    /* Put the item in the buffer */
    buffer_1[prod_idx_1] = item;
    /* Increment the index where the next item will be put. */
    prod_idx_1 = prod_idx_1 + 1;
    count_1++;
    /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_1);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_1);
}

/*
 Put an item in buff_2
*/
void put_buff_2(char *item)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_2);
    /* Put the item in the buffer */
    buffer_2[prod_idx_2] = item;
    /* Increment the index where the next item will be put. */
    prod_idx_2 = prod_idx_2 + 1;
    count_2++;
     /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_2);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_2);
}

void put_buff_3(char *item)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_3);
    /* Put the item in the buffer */
    buffer_3[prod_idx_3] = item;
    /* Increment the index where the next item will be put. */
    prod_idx_3 = prod_idx_3 + 1;
    count_3++;
    /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_3);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_3);
}

void put_buff_4(char *item)
{
    /* Lock the mutex before putting the item in the buffer */
    pthread_mutex_lock(&mutex_4);
    /* Put the item in the buffer */
    buffer_4[prod_idx_4] = item;
    /* Increment the index where the next item will be put. */
    prod_idx_4 = prod_idx_4 + 1;
    count_4++;
    /* Signal to the consumer that the buffer is no longer empty */
    pthread_cond_signal(&full_4);
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_4);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer for further thread.
*/
void *get_input(void *args)
{
    size_t length = 1000;
    char *inputLine;
    char *item;
    item = (char *)malloc(LENGTH * sizeof(char));
    inputLine = (char *)malloc(LENGTH * sizeof(char));
    bool check = false;
    int i;
    char *modifiedLine;
    
    
    /* Get the input untill STOP line */
    while (strcmp(inputLine, "STOP\n") != 0)
    {
        getline(&inputLine, &length, stdin);
        num_lines++;
        if (strcmp(inputLine, "STOP\n") != 0)
        {
            strcat(item, inputLine);
        }
    }

    num_lines -= 1;

    put_buff_1(item);    

    return NULL;
}

/*
Get the next item from buffer 1
*/
char *get_buff_1()
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_1);
    while (count_1 == 0)
        /* Buffer is empty. Wait for the producer to signal that the buffer has data */
        pthread_cond_wait(&full_1, &mutex_1);
    char *item = buffer_1[con_idx_1];
    /* Increment the index from which the item will be picked up */
    con_idx_1 = con_idx_1 + 1;
    count_1--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_1);
    /* Return the item */
    return item;
}

/*
 Function that the separate Input thread will run. 
 Consume an item from the buffer shared with the input thread.
 Find all new line signs and replace them with space signs.
 Produce an item in the buffer shared with the replace thread.
*/
void *separateInput(void *args)
{
    char *separateditem;

    separateditem = (char *)malloc(LENGTH * sizeof(char));

    int i;

    separateditem = get_buff_1();

    for (i = 0; i < strlen(separateditem); i++)
    {
        if (separateditem[i] == '\n')
        {
            separateditem[i] = ' ';
        }
    }

    put_buff_2(separateditem);

    return NULL;
}

/*
Get the next item from buffer 2
*/
char *get_buff_2()
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_2);
    while (count_2 == 0)
        /* Buffer is empty. Wait for the producer to signal that the buffer has data */
        pthread_cond_wait(&full_2, &mutex_2);
    char *item = buffer_2[con_idx_2];
    /* Increment the index from which the item will be picked up */
    con_idx_2 = con_idx_2 + 1;
    count_2--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_2);
    /* Return the item */
    return item;
}
char *get_buff_3()
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_3);
    while (count_3 == 0)
        /* Buffer is empty. Wait for the producer to signal that the buffer has data */
        pthread_cond_wait(&full_3, &mutex_3);
    char *item = buffer_3[con_idx_3];
    /* Increment the index from which the item will be picked up */
    con_idx_3 = con_idx_3 + 1;
    count_3--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_3);
    /* Return the item */
    return item;
}

char *get_buff_4()
{
    /* Lock the mutex before checking if the buffer has data */
    pthread_mutex_lock(&mutex_4);
    while (count_4 == 0)
        /* Buffer is empty. Wait for the producer to signal that the buffer has data */
        pthread_cond_wait(&full_4, &mutex_4);
    char *item = buffer_4[con_idx_4];
    /* Increment the index from which the item will be picked up */
    con_idx_4 = con_idx_4 + 1;
    count_4--;
    /* Unlock the mutex */
    pthread_mutex_unlock(&mutex_4);
    /* Return the item */
    return item;
}

/*
 Function that the repalce Input thread will run. 
 Consume an item from the buffer shared with the separate thread.
 Find all new line signs and replace them with space signs.
 Produce an item in the buffer shared with the replace thread.
*/
void *replaceInput(void *args)
{
    
   
    char* item = (char *)malloc(LENGTH * sizeof(char));
    item = get_buff_2();
    char* replaceditem = malloc(strlen(item)* sizeof(char));
    int i;
    int j = 0;
   
    
    for (i = 0; i < strlen(item);)
    {
        if (item[i] == '+' && item[i+1] == '+')
        {
            replaceditem[j] = '^';
            j++;
            i+=2;
        }
        else if(item[i] == '+' && item[i+1] != '+')
        {
            replaceditem[j] = item[i];
            j++;
            i++;
        }
        else
        {
            replaceditem[j] = item[i];
            j++;
            i++;
        }
    }

    put_buff_3(replaceditem);

    return NULL;
}

/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item with 80 chars and put new line delim.
*/
void *write_output(void *args)
{
    char *item;
    int i;
    char* outputstring;
    outputstring = malloc(85*sizeof(char));
    item = get_buff_3();
    int counter = 0;
    int counter2 = 0;
    int num_chars = strlen(item);

    while((num_chars - counter2)>=80){
    
    for (i = 0; i < 80; i++)
    {
        outputstring[i] = item[i+counter2];
        counter++;
    }
    counter2 = counter;
    strcat(outputstring, "\n");
    put_buff_4(outputstring);
    write(STDOUT_FILENO, outputstring, 81);
    }
    
    return NULL;
}

int main()
{
    srand(time(0));
    pthread_t input_t, separated_t, replaced_t, output_t;
    /* Create the threads */
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&separated_t, NULL, separateInput, NULL);
    pthread_create(&replaced_t, NULL, replaceInput, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);

    /* Wait for the threads to terminate */
    pthread_join(input_t, NULL);
    pthread_join(separated_t, NULL);
    pthread_join(replaced_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;
}