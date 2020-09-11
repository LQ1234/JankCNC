#pragma once
#include "../../../shared/pair.h"
#define NUM_PAIRS 50
namespace Dictionary {

    Pair pairs[NUM_PAIRS];
    void begin(){
        memset(pairs,0,sizeof(pairs));//i am lazy
    }
    void set(char* key,char *value){
        for(int i=0;i<NUM_PAIRS;i++){
            if(strncmp(pairs[i].key,key,20)==0){
                strncpy(pairs[i].value,value,sizeof(pairs[i].value));
                return;
            }
        }
        for(int i=0;i<NUM_PAIRS;i++){
            if(pairs[i].key[0]==0){
                strncpy(pairs[i].key,key,sizeof(pairs[i].key));
                strncpy(pairs[i].value,value,sizeof(pairs[i].value));
                return;
            }
        }
    }
    Pair get(char* key){
        for(int i=0;i<NUM_PAIRS;i++){
            if(strncmp(key,pairs[i].key,20)==0){
                return(pairs[i]);
            }
        }
        Pair ret;
        ret.key[0]=0;
        return(ret);
    }


}
