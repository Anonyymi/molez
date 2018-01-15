#include <thread>
#include <mutex>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>


typedef struct QueueElement{
	//To make a general purpose q change to void *data
	uint32_t *data;
	struct QueueElement *next;
} QueueElement;

class HQueue{
private:
	QueueElement *first, *last;
	uint32_t elemNum;
	std::mutex mutex;
public:
	HQueue();
	~HQueue();
	void push(uint32_t *data);
	uint32_t *pull();
	void destroy();
	uint32_t size();
};

