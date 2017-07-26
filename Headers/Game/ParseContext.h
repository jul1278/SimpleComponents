// ParseContext.h
#ifndef PARSE_CONTEXT_H
#define PARSE_CONTEXT_H


#include "Utility/FileUtility.h"
#include "Components/Repository/ComponentContainerFactory.h"
#include "Components/Repository/ComponentRepository.h"
#include "Graphics/Graphics.h"
#include "Components/Component.h"

#include <list>
#include <string>
#include <exception>
#include <memory>

//-------------------------------------------------------------------------------
// Name: ParseContext
// Desc:
//-------------------------------------------------------------------------------
class ParseContext
{
	shared_ptr<ComponentRepository> componentRepository; 
	shared_ptr<Graphics> graphics; 

	unordered_map<string, unsigned int> namedIdMap; 
	unordered_map<string, unsigned int> namedEntityMap; 

	unsigned int parentId; 
	unsigned int parentEntityId; 
	std::string parentCollectionName;

public:

	//----------------------------------------------------------------- 
	// Name: ParseContext
	// Desc:
	//-----------------------------------------------------------------
	ParseContext(shared_ptr<ComponentRepository> componentRepository, 
		shared_ptr<Graphics> graphics, 
		unsigned int parentId = 0, 
		unsigned int parentEntityId = 0, 
		std::string parentCollectionName = "") : 
		
		componentRepository(componentRepository), 
		graphics(graphics), 
		parentId(parentId), 
		parentEntityId(parentEntityId), 
		parentCollectionName(parentCollectionName)
	{}

	//------------------------------------------------------------
	// Name: ComponentRepository
	// Desc:
	//------------------------------------------------------------	
	shared_ptr<ComponentRepository> ComponentRepository()
	{
		return this->componentRepository;
	}

	//------------------------------------------------------------
	// Name: NamedId
	// Desc:
	//------------------------------------------------------------
	unsigned int NamedId(const string& namedId) 
	{
		if (this->namedIdMap.find(namedId) == this->namedIdMap.end()) {
			auto id = componentRepository->GenerateId(); 
			this->namedIdMap[namedId] = id; 
			return id; 
		}

		return this->namedIdMap[namedId];
	}

	//------------------------------------------------------------
	// Name: NamedEntityId
	// Desc: 
	//------------------------------------------------------------	
	unsigned int NamedEntityId(const string& entityName)
	{
		if (this->namedEntityMap.find(entityName) == this->namedEntityMap.end()) {
			auto id = this->componentRepository->GenerateId(); 
			this->namedEntityMap[entityName] = id;  
		}

		return this->namedEntityMap[entityName];
	}

	//------------------------------------------------------------
	// Name: ResourceIdFromPath
	// Desc:
	//------------------------------------------------------------
	unsigned int ResourceIdFromPath(const string& path) const
	{
		return this->graphics->LoadGraphicResource(path);
	}

	//------------------------------------------------------------
	// Name: ResourceIdFromName
	// Desc:
	//------------------------------------------------------------
	unsigned int ResourceIdFromName(const string& namedId, const SerialUtility::NamedValue& namedValue) const
	{
		// TODO: build graphics resource from namedValue
		return 0; 
	}

	//------------------------------------------------------------
	// Name: ParentId
	// Desc: 
	//------------------------------------------------------------
	unsigned int ParentId()
	{
		return this->parentId;
	}

	//------------------------------------------------------------
	// Name: ParentEntityId
	// Desc: 
	//------------------------------------------------------------
	unsigned int ParentEntityId()
	{
		return this->parentEntityId; 
	}

	//------------------------------------------------------------
	// Name: ParentEntityId
	// Desc: 
	//------------------------------------------------------------
	std::string ParentCollectionName()
	{
		return this->parentCollectionName;
	}

	//------------------------------------------------------------
	// Name: NewComponent
	// Desc: 
	//------------------------------------------------------------
	template<typename T> // todo: where T is child class of BaseComponent
	T* NewComponent()
	{
		std::string collectionName; 

		if (!this->componentRepository->ContainsCollection(this->ParentCollectionName())) {
			// else get the default top level collection
			collectionName = "";
		}

		auto baseComponent = this->componentRepository->NewComponent<T>(collectionName);

		return baseComponent; 
	}
};


#endif // PARSE_CONTEXT_H