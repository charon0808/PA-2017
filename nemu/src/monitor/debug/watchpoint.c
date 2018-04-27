#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_up()
{
    int i=0;
    if (head!=NULL){
        while (wp_pool[i].next!=NULL){
              i++;
        }
        if (i>=NR_WP-1){
              printf("no more free watchpoint\n");
              return NULL;
        }
        wp_pool[i].next=&wp_pool[i+1];
        wp_pool[i+1].next=NULL;
        return &wp_pool[i+1];
    }   
    else {
        head=&wp_pool[0];
        wp_pool[0].next=NULL;
        return &wp_pool[0];
    }
}

bool free_wp(int delete_number)
{
    if (head==NULL){
        printf("%s\n","No watch point yet");
        return true;
    }
    if (wp_pool[0].next==NULL&&delete_number==0){
        head=NULL;
        return true;
    }
    int now_number=0;
    while (wp_pool[now_number].next!=NULL){
        now_number++;
    }
    if (delete_number>now_number){
        return false;
    }
    for (int i=delete_number;i<now_number;i++){
        strcpy(wp_pool[i].args,wp_pool[i].next->args);
        wp_pool[i].old_value=wp_pool[i].next->old_value;
    }
    wp_pool[now_number-1].next=NULL;
    return true;
}

bool hit_wp(){
    if (head==NULL){
        return false;
    }
      bool find_change=false;
      for(int i=0;i==0||wp_pool[i-1].next!=NULL;i++){
          bool success=true;
          char lx_cpoy2[128];
          strcpy(lx_cpoy2,wp_pool[i].args);
          int the_new_value=eval(0,strlen(wp_pool[i].args)-1,wp_pool[i].args,&success);
          strcpy(wp_pool[i].args,lx_cpoy2);
          if (the_new_value!=wp_pool[i].old_value){
              find_change=true;
              printf("hit the point %d \n",wp_pool[i].NO);
              printf("Expersion:        %s\n",wp_pool[i].args);
              printf("old value:        %d\n",wp_pool[i].old_value);
              printf("new value:        %d\n\n",the_new_value);
              wp_pool[i].old_value=the_new_value;
          }
      }
    if (find_change){
        return true;
    }
    else return false;
}

void print_wp()
{
    if (head==NULL){
        printf("%s\n","No watchpoint yet.");
        return;
    }
    int i=0;
    printf("NO        Expersion     old value\n");
    while (wp_pool[i].next!=NULL){
       printf("%d          %s        %d\n",wp_pool[i].NO,wp_pool[i].args,wp_pool[i].old_value);
       i++; 
    }
    printf("%d          %s        %d\n",wp_pool[i].NO,wp_pool[i].args,wp_pool[i].old_value);
    return;
}
