#include "queue.hpp"


HQueue::
HQueue(){
	first = NULL;
	last = NULL;
	elemNum = 0;
}

HQueue::
~HQueue(){
	destroy();
}

void HQueue::
push(uint32_t *data){
	QueueElement *newElem = (QueueElement *)malloc(sizeof(QueueElement));
	newElem->data = data;
	newElem->next = NULL;
	mutex.lock();
	if(elemNum == 0){
		first = newElem;
		last = first;
	}
	else{
		last->next = newElem;
		last = newElem;
	}
	elemNum++;
	mutex.unlock();

}
uint32_t *HQueue::
pull(){
	

	if(elemNum == 0){
		return NULL;
	}

	mutex.lock();

	if(elemNum == 0){
		mutex.unlock();
		return NULL;
	}

	QueueElement *next = first->next;
	uint32_t *data = first->data;
	free(first);

	first = next;
	elemNum--;
	mutex.unlock();
	return data;

}


void HQueue::
destroy(){
	struct QueueElement *tmp;
	while(elemNum > 0){
		tmp = first->next;
		free(first->data);
		free(first);
		elemNum--;
		int a = 0;
		first = tmp;
		
	}

}
uint32_t HQueue::
size(){
	return elemNum;
}



