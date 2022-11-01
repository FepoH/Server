#include "sem.h"

namespace fepoh{

Sem::Sem(int value,int pshared ){
    sem_init(&m_sem,value,pshared);
}
Sem::~Sem(){
    sem_destroy(&m_sem);
}

void Sem::post(){
    sem_post(&m_sem);
}
void Sem::wait(){
    sem_wait(&m_sem);
}
int Sem::getValue(){
    int value = 0;
    sem_getvalue(&m_sem,&value);
    return value;
}



}//namespace