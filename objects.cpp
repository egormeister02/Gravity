#include "objects.h"

// Объекты
float norm(const sf::Vector2f& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

float updateRightTextPosition(sf::Text &text, const float& y, const sf::RenderWindow &window) 
{
    float textWidth = text.getLocalBounds().width + text.getLocalBounds().left;
    text.setPosition({static_cast<float>(window.getSize().x) - textWidth - 10, y});
    return text.getPosition().x;
};

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

	void System::update(sf::RenderWindow& window)
	{
		updateVelocity();
		updatePosition();

		std::vector<Object*> toErase;

        for (Object& obj : objects)
            if (!isInsideWindow(obj, window))
                toErase.push_back(&obj);

        for (Object* obj : toErase)
            eraseObject(*obj);

		updateTotalMass();
		updateTotalInMoment();
	}

	void System::draw(sf::RenderWindow& window) 
    {
        for (Object& obj : objects)
            window.draw(obj);
    }

	
    void System::updateTotalMass()
    {
        totalMass = 0;
        for(const Object& obj : objects)
        {
            totalMass += obj.getMass();
        }
    }


	Info::Info(const System& sys, const sf::Font& font, unsigned int characterSize): system(sys)
	{
		for (int i = 0; i < info_names.size(); i++) 
		{
			entries.push_back({sf::Text(info_names[i], font, characterSize), sf::Text( "", font,characterSize)});
			entries[i].name.setFillColor(sf::Color::White);
			entries[i].value.setFillColor(sf::Color::White);
		}

		maxWidthName = getMaxWidthName();
		height = entries[0].name.getLocalBounds().height;
	}

	void Info::updateFPS(const int& fps)
	{
		entries[0].value.setString(std::to_string(fps));
	}

	void Info::updateValues()
	{
		entries[1].value.setString(std::to_string(system.getTotalMass()));
		entries[2].value.setString('{' + std::to_string(system.getInMoment().x) + ", " + std::to_string(system.getInMoment().y) + '}');
	}

	void Info::updatePosition(const sf::RenderWindow& window)
	{
		float min_x = static_cast<float>(window.getSize().x);
		float y = 10;
		for (Entry& entry : entries)
		{
			if (updateRightTextPosition(entry.value, y, window) < min_x)
				min_x = entry.value.getPosition().x;
			y += height + 10;
		}
		updateNamePosition(min_x);
	}

	void Info::update(const sf::RenderWindow& window)
	{
		updateValues();
		updatePosition(window);
	}

	void Info::draw(sf::RenderWindow& window)
	{
		for (const Entry& entry: entries)
		{
			window.draw(entry.value);
			window.draw(entry.name);
		}
	}

	float Info::getMaxWidthName() const
	{
		float maxWidth = 0;
		for (const Entry& entry : entries)
		{
			if (entry.name.getLocalBounds().width > maxWidth)
				maxWidth = entry.name.getLocalBounds().width;
		}
		return maxWidth;
	}

	void Info::updateNamePosition(const float& min_x)
	{
		for (Entry& entry : entries)
		{
			entry.name.setPosition({min_x - maxWidthName, static_cast<float>(entry.value.getPosition().y)});
		}
	}
