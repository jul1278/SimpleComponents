#ifndef I_PHYSICS_TASK_H
#define I_PHYSICS_TASK_H

#include "Components/Repository/ComponentRepository.h"
#include "Events/EventObserveable.h"

class IPhysicsTask
{
private:

	EventObservable eventObservable;
	
	string collection1; 
	string collection2; 

	virtual void Task(ComponentRepository* componentRepository, 
		const string& collection1, const string& collection2, EventObservable* eventObservable) = 0;

public:
	
	IPhysicsTask(const string& collection1, const string& collection2)
	{
		this->collection1 = collection1;
		this->collection2 = collection2; 
	}

	template<typename T>
	void RegisterListener(function<void(const T&)> handler)
	{
		eventObservable.RegisterListener<T>(handler); 
	}

	virtual ~IPhysicsTask() {}

	void Execute(ComponentRepository* componentRepository)
	{
		this->Task(componentRepository, this->collection1, this->collection2, &this->eventObservable); 
	}
};

#endif // I_PHYSICS_TASK_H