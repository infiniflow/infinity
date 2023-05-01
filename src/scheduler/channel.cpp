#include "channel.h"

namespace infinity{
Channel::Channel(const std::uint16_t worker_id)
	:id_(worker_id){

}

void Channel::Put(const PipelineTaskPtr task){
	queue_.emplace_back(task);
}

PipelineTaskPtr Channel::Take(){
	while(!queue_.empty()){
		PipelineTaskPtr task = queue_.front();
		queue_.pop_front();
		return task;
	}
	return nullptr;
}

}