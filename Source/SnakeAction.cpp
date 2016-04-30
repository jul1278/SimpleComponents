#include "Game/IGameApp.h"
#include "Graphics/Graphics.h"
#include "Components/ComponentCollectionRepository.h"
#include "Events/SDLEventCollector.h"
#include "Events/ButtonEventArgs.h"
#include "Graphics/RectGraphicsResource.h"
#include "Graphics/TextGraphicsResource.h"
#include "Components/GraphicsComponent.h"
#include "Components/TransformComponent.h"
#include "Utility/MathUtility.h"
#include "SnakeGame/SnakeAction.h"
#include <string>
#include <chrono>
#include <ratio>
#include <iostream>

//---------------------------------------------------------------------------
// Name: SnakeAction 
// Desc:
//---------------------------------------------------------------------------
SnakeAction::SnakeAction(IGameApp* gameApp)
{
	this->snakeStartLength = 4;
	this->snakeScore = 0;
	this->snakeLength = this->snakeStartLength;

	this->currentSnakeDirection = SNAKE_LEFT;
	auto graphics = gameApp->GetGraphics();

	auto headStartPos = Vector2D(graphics->WindowWidth() / 2.0f, graphics->WindowHeight() / 2.0f);

	auto componentCollection = gameApp->GetComponentCollectionRepository();

	auto snakeGraphicId = graphics->AddGraphicsResource(new RectGraphicsResource(8.0f, 8.0f, 0xff, 0xff, 0xff, 0xff));
	auto foodGraphicId = graphics->AddGraphicsResource(new RectGraphicsResource(8.0f, 8.0f, 0xff, 0xff, 0xff, 0xff)); 

	this->textGraphicResource = new TextGraphicsResource("0", "", 20); 
	auto textGraphicResourceId = graphics->AddGraphicsResource(this->textGraphicResource); 

	componentCollection->NewCollection("Snake");
	componentCollection->NewCollection("Food");
	componentCollection->NewCollection("Score"); 

	auto scoreTextTransformComponent = componentCollection->NewComponent<TransformComponent>("Score");
	auto scoreGraphicsComponent = componentCollection->NewComponent<GraphicsComponent>("Score"); 

	scoreTextTransformComponent->position = Vector2D(10.0f, 10.0f); 
	scoreGraphicsComponent->transformComponent = scoreTextTransformComponent; 
	scoreGraphicsComponent->resourceId = textGraphicResourceId; 

	auto snakeHead = componentCollection->NewComponent<TransformComponent>("Snake");
	auto snakeHeadGraphics = componentCollection->NewComponent<GraphicsComponent>("Snake");

	snakeHead->position = headStartPos;
	snakeHeadGraphics->transformComponent = snakeHead;
	snakeHeadGraphics->resourceId = snakeGraphicId;

	auto lastPos = snakeHead->position;

	auto foodPos = componentCollection->NewComponent<TransformComponent>("Food");
	auto foodGraphic = componentCollection->NewComponent<GraphicsComponent>("Food");

	foodPos->position.x = MathUtility::RandomIntUniformDist() % graphics->WindowWidth();
	foodPos->position.y = MathUtility::RandomIntUniformDist() % graphics->WindowHeight();

	foodGraphic->resourceId = foodGraphicId;
	foodGraphic->transformComponent = foodPos;

	for (auto i = 0; i < (this->snakeStartLength - 1); i++) {

		auto snakePart = componentCollection->NewComponent<TransformComponent>("Snake");
		auto snakePartGraphics = componentCollection->NewComponent<GraphicsComponent>("Snake");

		lastPos += Vector2D(10.0f, 0.0);

		snakePart->position = lastPos;
		snakePartGraphics->transformComponent = snakePart;
		snakePartGraphics->resourceId = snakeGraphicId;
	}

	this->SpawnFood(gameApp);

	this->sdlEventCollector.RegisterListener<ButtonEventArgs>(bind(&SnakeAction::OnButtonEvent, this, placeholders::_1));
}
//---------------------------------------------------------------------------
// Name: SpawnFood 
// Desc:
//---------------------------------------------------------------------------
void SnakeAction::SpawnFood(IGameApp* gameApp)
{
	auto graphics = gameApp->GetGraphics();
	auto componentCollections = gameApp->GetComponentCollectionRepository(); 

	auto width = graphics->WindowWidth(); 
	auto height = graphics->WindowHeight(); 

	auto& foodTransform = componentCollections->SelectFromCollection<TransformComponent>("Food")->front(); 
	auto snakeTransforms = componentCollections->SelectFromCollection<TransformComponent>("Snake"); 
	
	auto notSpawnFood = true; 
	
	while (notSpawnFood) {

		int x = 10 * (MathUtility::RandomIntUniformDist() % (width / 10));
		int y = 10 * (MathUtility::RandomIntUniformDist() % (height / 10));

		foodTransform.position.x = x;
		foodTransform.position.y = y; 

		notSpawnFood = false;

		for (auto snakeTransform : *snakeTransforms) {
			if (snakeTransform.position == foodTransform.position) {
				notSpawnFood = true;
				break;
			}
		}
	}	
}
//---------------------------------------------------------------------------
// Name: Update
// Desc:
//---------------------------------------------------------------------------
void SnakeAction::Update(IGameApp* gameApp) 
{
	auto componentCollectionRepository = gameApp->GetComponentCollectionRepository(); 

	auto transformComponents = componentCollectionRepository->SelectFromCollection<TransformComponent>("Snake");
	auto graphicsComponents = componentCollectionRepository->SelectFromCollection<GraphicsComponent>("Snake");
	auto foodTransform = componentCollectionRepository->SelectFromCollection<TransformComponent>("Food")->front(); 

	this->sdlEventCollector.Update();

	if (!transformComponents->empty()) {
		auto& headTransform = transformComponents->front();

		Vector2D lastPos;
		auto nextPos = headTransform.position;

		switch (this->currentSnakeDirection) {
		case SNAKE_LEFT:
			headTransform.position += Vector2D(-10.0f, 0.0f);
			break;
		case SNAKE_RIGHT:
			headTransform.position += Vector2D(10.0f, 0.0f);
			break;
		case SNAKE_DOWN:
			headTransform.position += Vector2D(0.0f, 10.0f);
			break;
		case SNAKE_UP:
			headTransform.position += Vector2D(0.0f, -10.0f);
			break;
		}

		for (auto i = 1; i < transformComponents->size(); i++) {
			lastPos = (*transformComponents)[i].position;
			(*transformComponents)[i].position = nextPos;
			nextPos = lastPos;
		}

		std::chrono::steady_clock::time_point start; 
		start = std::chrono::steady_clock::now();
		
		// see if we ate anything
		if (headTransform.position == foodTransform.position) {

			this->snakeScore++;

			this->textGraphicResource->SetText(to_string(this->snakeScore)); 

			auto newSnakePart = componentCollectionRepository->NewComponent<TransformComponent>("Snake"); 
			auto newSnakeGraphic = componentCollectionRepository->NewComponent<GraphicsComponent>("Snake"); 

			newSnakePart->position = lastPos;
			newSnakeGraphic->transformComponent = newSnakePart; 
			newSnakeGraphic->resourceId = 0; 

			this->SpawnFood(gameApp); 

			auto diff = std::chrono::steady_clock::now() - start; 

			auto diffNs = chrono::duration<double, std::milli>(diff); 

			cout << "Eat food time: " << diffNs.count() << endl;
		}
	}
}
//---------------------------------------------------------------------------
// Name: OnButtonEvent
// Desc:
//---------------------------------------------------------------------------
void SnakeAction::OnButtonEvent(const ButtonEventArgs& buttonEventArgs)
{
	if (!buttonEventArgs.Released()) {
		return;
	}

	if (buttonEventArgs.Key() == LEFT_ARROW) {

		switch (this->currentSnakeDirection) {
		case SNAKE_LEFT:
			this->currentSnakeDirection = SNAKE_DOWN;
			break;
		case SNAKE_RIGHT:
			this->currentSnakeDirection = SNAKE_UP;
			break;
		case SNAKE_DOWN:
			this->currentSnakeDirection = SNAKE_LEFT;
			break;
		case SNAKE_UP:
			this->currentSnakeDirection = SNAKE_LEFT;
			break;
		}
	}
	else if (buttonEventArgs.Key() == RIGHT_ARROW) {

		switch (this->currentSnakeDirection) {
		case SNAKE_LEFT:
			this->currentSnakeDirection = SNAKE_UP;
			break;
		case SNAKE_RIGHT:
			this->currentSnakeDirection = SNAKE_DOWN;
			break;
		case SNAKE_DOWN:
			this->currentSnakeDirection = SNAKE_RIGHT;
			break;
		case SNAKE_UP:
			this->currentSnakeDirection = SNAKE_RIGHT;
			break;
		}
	}
}