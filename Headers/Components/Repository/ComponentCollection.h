#ifndef COMPONENT_COLLECTION_H
#define COMPONENT_COLLECTION_H

#include "Components/Repository/VectorContainer.h"
#include "Components/BaseComponent.h"

#include <string>
#include <unordered_map>  
#include <vector>
#include <typeindex>
#include <type_traits>
#include <algorithm>
#include <iterator>

using namespace std; 

// ComponentCollection
class ComponentCollection
{
	static unsigned int id; 

	unordered_map<type_index, IVectorContainer*> componentCollection;
	unordered_map<unsigned int, tuple<type_index*, IVectorContainer*>> idToComponent;

	// new
	// unordered_map<string, shared_ptr<IVectorContainer>> componentContainerMap; 
	// unordered_map<unsigned int, tuple<string, shared_ptr<IVectorContainer>> idToComponentContainerMap; 

public:

	ComponentCollection()
	{
	} 
	
	~ComponentCollection()
	{
		for (auto pair : this->componentCollection)
		{
			delete pair.second;
		}

		for (auto pair : this->idToComponent) {
			auto tup = get<1>(pair); 
			auto type = get<0>(tup); 

			delete type; 
		}
	}
	
	//---------------------------------------------------------------------------------------------
	// Name: NewComponent
	// Desc:
	//---------------------------------------------------------------------------------------------
	template <typename T, typename = typename enable_if<is_base_of<BaseComponent, T>::value>::type>
	T* NewComponent()
	{
		if (this->componentCollection[type_index(typeid(T))] == nullptr) {
			auto vectorContainer = new VectorContainer<T>();
			this->componentCollection[type_index(typeid(T))] = vectorContainer;
		}

		VectorContainer<T>* container = dynamic_cast<VectorContainer<T>*>(this->componentCollection[type_index(typeid(T))]);

		container->vec.push_back(T());
		T* component = &container->vec.back();

		component->id = ComponentCollection::GenerateId();

		auto type = new type_index(typeid(T));
		this->idToComponent[component->id] = tuple<type_index*, IVectorContainer*>(type, container);

		return &container->vec.back();
	}

	void DeleteId(unsigned int id); 

	//---------------------------------------------------------------------------------------------
	// Name: Select
	// Desc:
	//---------------------------------------------------------------------------------------------
	template<typename T, typename = typename enable_if<is_base_of<BaseComponent, T>::value>::type>
	vector<T>* Select()
	{
		auto iContainer = componentCollection[type_index(typeid(T))];
		auto container = static_cast<VectorContainer<T>*>(iContainer);
		return &(container->vec);
	}
	//---------------------------------------------------------------------------------------------
	// Name: Select
	// Desc:
	//---------------------------------------------------------------------------------------------
	template<typename T, typename = typename enable_if<is_base_of<BaseComponent, T>::value>::type> 
	T* Select(unsigned int id)
	{
		auto baseContainer = this->componentCollection[type_index(typeid(T))];

		if (baseContainer == nullptr) {
			return nullptr; 
		}

		VectorContainer<T>* container = static_cast<VectorContainer<T>*>(baseContainer);
		auto component = find_if(container->vec.begin(), container->vec.end(), [id](const T& t){return t.id == id; });

		return &(*component); 
	}

	//-------------------------------------------------------------------------------
	// Name: SelectBase
	// Desc: select id as BaseComponent 
	//-------------------------------------------------------------------------------
	BaseComponent* SelectBase(unsigned int id)
	{
		auto pair = this->idToComponent[id]; 
		auto container = get<1>(pair);
		return container->SelectBase(id); 
	}
	//-------------------------------------------------------------------------------
	// Name: Type
	// Desc: get the type_index for specified id
	//-------------------------------------------------------------------------------
	type_index Type(unsigned int id)
	{
		if (this->idToComponent.find(id) != this->idToComponent.end()) {
			return *get<0>(this->idToComponent[id]);
		}

		// could throw an exception?
		return type_index(typeid(void)); 
	}

	static unsigned int GenerateId()
	{
		return id++;
	}
};

//---------------------------------------------------------------------------------------------
// Name: Delete
// Desc:
//---------------------------------------------------------------------------------------------
inline void ComponentCollection::DeleteId(unsigned int id)
{
	auto typeComponentPair = this->idToComponent[id]; 

	auto type = get<0>(typeComponentPair); 
	this->componentCollection[*type]->RemoveId(id); 

	auto it = this->idToComponent.find(id);

	if (it != this->idToComponent.end()) {
		this->idToComponent.erase(it); 
	}
}

#endif // COMPONENT_COLLECTION_H
