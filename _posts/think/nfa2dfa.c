/**
 * nfa转dfa算法实现
 * transfer_t.letter = '\0' 表示空字符
 */ 
#include <stdio.h>
#include <stdlib.h>

typedef struct state_s state_t;
typedef struct transfer_s transfer_t;
void initNfa(state_t **s);

char map[13][3] = {
    {'0','\0','1'},
    {'1','\0','2'},
    {'1','\0','4'},
    {'2','a','3'},
    {'4','b','5'},
    {'3','\0','6'},
    {'5','\0','6'},
    {'6','\0','1'},
    {'6','\0','7'},
    {'0','\0','7'},
    {'7','a','8'},
    {'8','b','9'},
    {'9','b','10'},
};

struct state_s {
    char name;
    int final;
    transfer_t* transfer;
}

struct transfer_s {
    char letter;
    state_t* state; 
    transfer_t* next;
}

int main(){
    state_t *s; 
    initNfa(&s);
}

void initNfa(state_t **s){
    state_t     *head, *state, *start;
    state_t* state_name[13];
    transfer_t **transfer_p,*transfer;
    int i,index,j;

    for(i = 0; i < 13; i++){
        state_name[i] = NULL;
    }

    head = *s;
    head = (state_t*)malloc(sizeof(state_t));

    head->name = map[0][0];
    head->final = 0;
    head->transfer = NULL;
    index = 0;
    state_name[index] = head;
    
    for(i = 0; i < 13; i++) {
        for(j = 0; j <= index; j++){
            if(state_name[j] && state_name[j]->name == map[i][2]){
                state = state_name[j];
                break;
            }
        } 
        if(!state){
            state = (state_t*)malloc(sizeof(state_t)); 
            state->name = map[i][2];
            state->final = 0;
            if(i == 12){
                state->final = 1;
            }
            state->transfer = NULL;
        }
        
        for(j = 0; j <= index; j++){
            if(state_name[j] && state_name[j]->name == map[i][0]){
                start = state_name[j];
                break;
            }
        } 
        for(transfer_p = &(start->transfer); *transfer_p; transfer_p = &((*transfer_p)->next)){
            
        }
        *transfer_p = (transfer_t*)malloc(transfer_t);
        (*transfer_p)->letter = map[i][1];
        (*transfer_p)->state = state;
        (*transfer_p)->next = NULL;
        state = NULL;
    }



}

