#include "object.h"
#include "component.h"
#include "../Components/image.h"
#include "Utils.h"
#include <typeinfo>
#include "SceneManager.h"

Object::Object(SceneManager *scene, SDL_Renderer *renderer_) : currentScene(scene), renderer(renderer_)
{
	deltatime = 0;
	layer = 0;
	active = true;
	angle = 0;
	fixedCamera=false; 
	this->renderer = renderer_;
}

bool Object::Crossing(Object *obj, const float &x_range, const float &y_range)
{
	Vector2 temp_pos = obj->GetPosition();
	Vector2 temp_size = obj->GetSize();

	return ((temp_pos.y + temp_size.y * y_range) >= (position.y)) &&
		   (temp_pos.y) <= (position.y + size.y * y_range) &&
		   (temp_pos.x + temp_size.x * x_range >= position.x) && temp_pos.x <= position.x + size.x * x_range;
}

bool Object::Crossing(Object *obj)
{
	return Crossing(obj, 1.0f, 1.0f);
}

Vector2 Object::GetPosition() const
{
	return position;
}

void Object::SetPosition(const Vector2 &vec2)
{
	position = vec2;
}

void Object::SetAngle(float _angle)
{
	angle = _angle; 
}
void Object::SetPositionOnPlatform(const Vector2 &vec2)
{
	SetPosition(Vector2(vec2.x, vec2.y - size.y));
}

void Object::MoveY(const float &pos_y)
{
	position.y += pos_y * deltatime * 80;
}

void Object::MoveX(const float &pos_x)
{
	position.x += pos_x * deltatime * 80;
}

Vector2 Object::GetSize() const
{
	return size;
}

float Object::GetAngle() const{
	return angle; 
}
void Object::InitSize(Image *img)
{
	if (img != nullptr)
		size = img->GetSize();
}
void Object::InitSize()
{
	Image *img = GetComponent<Image>();
	if (img != nullptr)
		size = img->GetSize();
}

void Object::SetLayer(int newLayer)
{
	layer = newLayer;
	currentScene->updateLayer();
}

int Object::GetLayer() const
{
	return layer;
}


void Object::AddComponent(Component *component)
{

	if (this)
	{
		for (auto it = components.begin(); it != components.end();)
		{
			if (typeid(*component) == typeid(**it))
			{
				delete *it;
				it = components.erase(it);
			}
			else
			{
				++it;
			}
		}
		component->object = this;
		component->Init();
		components.push_back(component);
	}
	else
		std::cerr << "Object didn't create " << std::endl;
}

Component *Object::GetComponent(const std::type_info &ti) const
{
	for (auto &component : components)
	{
		if (typeid(*component) == ti)
		{
			return component;
		}
	}
	return nullptr;
}

template <typename T>
void Object::RemoveComponent()
{
	Component *comp = this->GetComponent(typeid(T));
	if (comp != nullptr)
	{
		delete comp;

		components.erase(std::remove(components.begin(), components.end(), comp), components.end());
	}
}

void Object::update(float deltaTime)
{
	for (auto &component : components)
	{
		component->Update();
		component->update(deltaTime);
	}
	this->deltatime = deltaTime;
}

SceneManager *Object::GetScene() const
{
	return currentScene;
}

SDL_Renderer *Object::GetRenderer() const
{
	return renderer;
}

void Object::UpdateEvents(SDL_Event &event)
{
	for (auto &component : components)
	{
		if (event.type == SDL_MOUSEBUTTONDOWN)
			component->OnMouseButtonDown(Vector2((float)event.button.x, (float)event.button.y));
		else if (event.type == SDL_MOUSEBUTTONUP)
			component->OnMouseButtonUp(Vector2((float)event.button.x, (float)event.button.y));
		else if (event.type == SDL_MOUSEMOTION)
			component->OnMouseButtonMotion(Vector2((float)event.motion.x, (float)event.motion.y));
		else if (event.type == SDL_KEYDOWN)
			component->onKeyPressed(event.key.keysym.sym);
		else if (event.type == SDL_KEYUP)
			component->onKeyReleased(event.key.keysym.sym);
	}
}

void Object::SetActive(bool status)
{

	active = status;
}

Object::~Object()
{
	for (auto *component : components)
	{
		delete component;
	}
	components.clear();
}
