
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <iostream>


using namespace std;

/*
 This wrapper class for semaphore.h functions is from:
 http://stackoverflow.com/questions/2899604/using-sem-t-in-a-qt-project
 */
class Semaphore {
public:
    // Constructor
    Semaphore(int initialValue)
    {
        sem_init(&mSemaphore, 0, initialValue);
    }
    // Destructor
    ~Semaphore()
    {
        sem_destroy(&mSemaphore); /* destroy semaphore */
    }
    
    // wait
    void wait()
    {
        sem_wait(&mSemaphore);
    }
    // signal
    void signal()
    {
        sem_post(&mSemaphore);
    }
    
    
private:
    sem_t mSemaphore;
};

//code constants
#define MAX_READER 5
#define MAX_WRITER 5
#define MAX_PHIL 5


//helper data for reader/writer
struct Light_Switch{

  int            counter;
  Semaphore*      mutex;
};

//data for reader writers solution1
struct Reader_Writer_No_Strave{

  Light_Switch* readLightswitch; 
  Semaphore* roomEmpty;
  Semaphore* turnstile;
};

//data for reader writers solution 2
struct Reader_Writer_With_Priority{

  Light_Switch*  readSwitch;
  Light_Switch*  writeSwitch;

  Semaphore* noReaders;
  Semaphore* noWriters;
};

//data for dining philosophers solution 1
struct Dining_Phil_Sol{
  Semaphore* fork[MAX_PHIL];
  Semaphore* footman;
};

//high level functions, we call them directly from main
void reader_writer_no_strave();
void reader_writer_with_priority();
void dining_phil_sol1();
void dining_phil_sol2();

//helper functions
void light_switch_init(Light_Switch* ls){
  ls->counter=0;
  ls->mutex= new Semaphore(1);
}

void light_switch_lock(Light_Switch* ls, Semaphore* sem){

  ls->mutex->wait();
  ls->counter +=1;
  
  if(ls->counter==1){
    sem->wait();
  }
 
  ls->mutex->signal();
}

void light_switch_unlock(Light_Switch* ls, Semaphore* sem){

  ls->mutex->wait();
  ls->counter -=1;
  
  if(ls->counter==0){
    sem->signal();
  }
 
  ls->mutex->signal();
}

void dining_phil_sol_init(Dining_Phil_Sol* df){
 
  for(int i=0;i<MAX_PHIL;i++)
     df->fork[i]=new Semaphore(1);

   df->footman=new Semaphore(4);
}

//thread functions
void * reader_thread1(void *info);
void * reader_thread2(void *info);

void * writer_thread1(void *info);
void * writer_thread2(void *info);

void * dining_thread1(void *info);
void * dining_thread2(void *info);

////////////////////main function//////////////////////////////////
int main(int argc, char **argv )
{
   
  //parse user input
  if(argc<2){
     std::cout<<"Usage: "<< argv[0]<<"<problem #>\n"<<std::endl;
     return 0;
  }
	 
  //get user selection
  int problem=atoi(argv[1]);
    
  if(problem==1){
    reader_writer_no_strave();
  }
  else if(problem==2){
    reader_writer_with_priority();
  }
  else if(problem==3){
    dining_phil_sol1();
  }
  else if(problem==4){
    dining_phil_sol2();
  }
  
  return 0;
    
} 
//////////////////////////////////////////////////////////////////

Reader_Writer_No_Strave* rw1=NULL;
void reader_writer_no_strave(){

 //initialization code
 rw1=new Reader_Writer_No_Strave;
 rw1->readLightswitch=new Light_Switch;
 rw1->roomEmpty=new Semaphore(1);
 rw1->turnstile=new Semaphore(1);

 light_switch_init(rw1->readLightswitch);

  pthread_t thrd;
  for( long r = 0; r < MAX_READER; r++ ){

     int rc = pthread_create ( &thrd, NULL, reader_thread1, (void *) (r+1) );  
     if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //create writers
  for( long w = 0; w < MAX_WRITER; w++ ){

    int rc = pthread_create ( &thrd, NULL, writer_thread1, (void *) (w+1) );  
    if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //make this function to block until last created thread finishes
  pthread_join(thrd, NULL);
}

Reader_Writer_With_Priority* rw2;
void reader_writer_with_priority(){

  rw2=new Reader_Writer_With_Priority;
  rw2->readSwitch=new Light_Switch;
  rw2->writeSwitch=new Light_Switch;

  rw2->noReaders=new Semaphore(1);
  rw2->noWriters=new Semaphore(2);
  light_switch_init(rw2->readSwitch);
  light_switch_init(rw2->writeSwitch);
  

  pthread_t thrd;
  for( long r = 0; r < MAX_READER; r++ ){

     int rc = pthread_create ( &thrd, NULL, reader_thread2, (void *) (r+1) );  
     if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //create writers
  for( long w = 0; w < MAX_WRITER; w++ ){

    int rc = pthread_create ( &thrd, NULL, writer_thread2, (void *) (w+1) );  
    if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //make this function to block until last created thread finishes
  pthread_join(thrd, NULL);

}

Dining_Phil_Sol* df;

void dining_phil_sol1(){
  
  pthread_t thrd;
  df=new Dining_Phil_Sol;
  dining_phil_sol_init(df);
  
  for( long p = 0; p < MAX_PHIL; p++ ){

    int rc = pthread_create ( &thrd, NULL, dining_thread1, (void *) (p+1) );  
    if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //make this function to block until last created thread finishes
  pthread_join(thrd, NULL);
}
void dining_phil_sol2(){

  pthread_t thrd;
  df=new Dining_Phil_Sol;
  dining_phil_sol_init(df);

  for( long p = 0; p < MAX_PHIL; p++ ){

    int rc = pthread_create ( &thrd, NULL, dining_thread2, (void *) (p+1) );  
    if (rc){
        cout<<"Error while creating a thread\n";
        return;
     }
  }

  //make this function to block until last created thread finishes
  pthread_join(thrd, NULL);

}

void * reader_thread1(void *r){
  
  long id=(long)(r);

  while(1){

    rw1->turnstile->wait();
    rw1->turnstile->signal();

    light_switch_lock(rw1->readLightswitch, rw1->roomEmpty);
    printf("Reader# %ld reads\n", id);
    light_switch_unlock(rw1->readLightswitch, rw1->roomEmpty);

    sleep(2);
  }
 
  
  return NULL;
}

void * writer_thread1(void *w){

  long id=(long)(w);
  while(1){

   rw1->turnstile->wait();
     rw1->roomEmpty->wait();
     printf("Writer# %ld writes\n", id);

   rw1->turnstile->signal();
   rw1->roomEmpty->signal();

   sleep(2);
  }
  return NULL;
}

void * reader_thread2(void *r){

 long id=(long)(r);
  while(1){
  
    rw2->noReaders->wait();
    light_switch_lock(rw2->readSwitch, rw2->noWriters);
    rw2->noReaders->signal();
       printf("Reader# %ld reads\n", id);
  
     light_switch_unlock(rw2->readSwitch, rw2->noWriters);

     sleep(2);
  }

  return NULL;
}
void * writer_thread2(void *w){

  long id=(long)(w);
  while(1){

     light_switch_lock(rw2->writeSwitch, rw2->noReaders);
     rw2->noWriters->wait();
      printf("Writer# %ld writes\n", id);
     rw2->noWriters->signal();
     light_switch_unlock(rw2->writeSwitch, rw2->noReaders);

     sleep(2);
  }

  return NULL;
}

int left(int i){return i;}
int right(int i){return (i + 1)%(MAX_PHIL-1);}

void get_fork(int i){

  df->footman->wait();
  df->fork[right(i)]->wait();
  df->fork[left(i)]->wait();
}

void put_forks(int i){
 
  df->fork[right(i)]->signal();
  df->fork[left(i)]->signal();
  df->footman->signal();
}

void get_fork2(int i){
 
  if(i==1){
    df->fork[left(i)]->wait();
    df->fork[right(i)]->wait();
  }
  else{
    df->fork[right(i)]->wait();
    df->fork[left(i)]->wait();
  }
}

void put_forks2(int i){
 
  if(i==1){
    df->fork[left(i)]->signal();
    df->fork[right(i)]->signal();
  }
  else{
   df->fork[right(i)]->signal();
   df->fork[left(i)]->signal();
  }
  
}

void * dining_thread1(void *data){
  long id=(long)(data);

  while(1){

    printf("Philosopher %ld thinking\n", id);
    
    get_fork(id);
    printf("Philosopher %ld eating\n", id);
    put_forks(id);
    sleep(2);
  }

  return NULL;
}
void * dining_thread2(void *data){

  long id=(long)(data);

  while(1){

    printf("Philosopher %ld thinking\n", id);
    
    get_fork2(id);
    printf("Philosopher %ld eating\n", id);
    put_forks2(id);
    sleep(2);
  }

   return NULL;
}






