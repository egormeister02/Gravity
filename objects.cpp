#include "objects.h"

// Объекты
float norm(const sf::Vector2f& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

bool isInsideWindow(const sf::CircleShape& object, const sf::RenderWindow& window)
{
    sf::Vector2f position = object.getPosition();
    sf::Vector2u windowSize = window.getSize() + sf::Vector2u({250,250});

    bool insideX = (position.x >= -250.f) && (position.x <= static_cast<float>(windowSize.x));
    bool insideY = (position.y >= -250.f) && (position.y <= static_cast<float>(windowSize.y));

    return insideX && insideY;
}


	Object::Object(sf::Vector2f pos, sf::Vector2f vel, float mass, float size):
	velocity(vel), mass(mass), sf::CircleShape(size)
	{
		this->setPosition(pos);
	}

	void Object::changeVelocity(sf::Vector2f dv)
	{
		velocity += dv;
	}

	float Object::getMass() const
	{
		return mass;
	}

	sf::Vector2f Object::getVelocity() const
	{
		return velocity;
	}


    System::System(const std::vector<Object>& obj)
        : objects(obj)
    {
        updateTotalMass();
		updateTotalInMoment();
	}

    void System::addObject(const Object& obj)
    {
        objects.push_back(obj);
        totalMass += obj.getMass();
		totalInMoment += obj.getMass()* obj.getPosition();
    }

	void System::eraseObject(Object& obj)
	{

		Object* start = objects.data();

		std::ptrdiff_t index = &obj - start;

		if (index >= 0 && index < objects.size()) 
			objects.erase(objects.begin() + index);
	}

    float System::getTotalMass() const
    {
        return totalMass;
    }

	sf::Vector2f System::getInMoment() const
    {
        return totalInMoment;
    }
	
	void System::updateVelocity()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			sf::Vector2f force = {0, 0};
			for (int j = 0; j < objects.size(); j ++)
			{
				if(i != j)
				{
					sf::Vector2f direction = (objects[j].getPosition()) - (objects[i].getPosition());
					if (norm(direction) > 5.f)
					force += float(G * objects[i].getMass() * objects[j].getMass() / pow(norm(direction), 3)) * direction;
				}
				
			}
			objects[i].changeVelocity(force / objects[i].getMass() * dt);
		}
	}

	void System::updatePosition()
	{
		for (Object& obj : objects)
		{
			obj.move(obj.getVelocity() * dt);
		}
		updateTotalInMoment();
	}

	void System::updateTotalInMoment()
	{
		totalInMoment = {0, 0};
		for(const Object& obj : objects)
		{
			totalInMoment += obj.getPosition() * obj.getMass();
		}
		totalPosition = totalInMoment / totalMass;
	}

	void System::draw(sf::RenderWindow& window) 
    {
        std::vector<Object*> toErase;
        for (Object& obj : objects)
        {
            if (!isInsideWindow(obj, window))
                toErase.push_back(&obj);
            else
                window.draw(obj);
        }
        for (Object* obj : toErase)
        {
            eraseObject(*obj);
        }
    }

	
    void System::updateTotalMass()
    {
        totalMass = 0;
        for(const Object& obj : objects)
        {
            totalMass += obj.getMass();
        }
    }

