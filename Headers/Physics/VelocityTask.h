#ifndef VELOCITY_TASK_H
#define VELOCITY_TASK_H

#include "Physics/IPhysicsTask.h"
#include "Components/PhysicsComponent.h"

class VelocityTask : public IPhysicsTask
{
	void Task(ComponentRepository* componentRepository, 
		const string& collection1, const string& collection2, EventObservable* eventObservable) override final
	{
		auto physicsComponents = componentRepository->Select<PhysicsComponent>(collection1); 
		
		if (physicsComponents.Size() == 0) {
			return; 
		}

		for (auto& component : physicsComponents) {
			
			auto transformComponent = componentRepository->SelectId<TransformComponent>(component.transformComponentId); 

			// TODO: this should have a timestep and stuff
			component.velocity += component.acceleration; 
			transformComponent->position += component.velocity;

			component.angularVelocity += component.angularAcceleration; 
			transformComponent->orientation = Vector2D(component.angularVelocity + transformComponent->Angle());
		}
	}

public:

	explicit VelocityTask(const string& collection) : IPhysicsTask(collection, "") {}
};

#endif // VELOCITY_TASK_H