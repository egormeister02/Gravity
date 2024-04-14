#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <iostream>

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

class Object: public sf::CircleShape
{
	public:

	Object(sf::Vector2f pos, sf::Vector2f vel, float mass, float size):
	velocity(vel), mass(mass), sf::CircleShape(size)
	{
		this->setPosition(pos);
	}

	void changeVelocity(sf::Vector2f dv)
	{
		velocity += dv;
	}

	float getMass() const
	{
		return mass;
	}

	sf::Vector2f getVelocity() const
	{
		return velocity;
	}

	private:
    sf::Vector2f 	velocity;
    float 			 	mass;

};

class System
{
public:

    System(const std::vector<Object>& obj)
        : objects(obj)
    {
        updateTotalMass();
		updateTotalInMoment();
	}

    void addObject(const Object& obj)
    {
        objects.push_back(obj);
        totalMass += obj.getMass();
		totalInMoment += obj.getMass()* obj.getPosition();
    }

	void eraseObject(Object& obj)
	{

		Object* start = objects.data();

		std::ptrdiff_t index = &obj - start;

		if (index >= 0 && index < objects.size()) 
			objects.erase(objects.begin() + index);
	}

    float getTotalMass() const
    {
        return totalMass;
    }

	sf::Vector2f getInMoment() const
    {
        return totalInMoment;
    }
	
	void updateVelocity()
	{
		for (int i = 0; i < objects.size(); i++)
		{
			sf::Vector2f force = {0, 0};
			for (int j = 0; j < objects.size(); j ++)
			{
				if(i != j)
				{
					sf::Vector2f direction = (objects[j].getPosition()) - (objects[i].getPosition());
					if (norm(direction) > 10.f)
					force += float(G * objects[i].getMass() * objects[j].getMass() / pow(norm(direction), 3)) * direction;
				}
				
			}
			objects[i].changeVelocity(force / objects[i].getMass() * dt);
		}
	}

	void updatePosition()
	{
		for (Object& obj : objects)
		{
			obj.move(obj.getVelocity() * dt);
		}
		updateTotalInMoment();
	}

	void updateTotalInMoment()
	{
		totalInMoment = {0, 0};
		for(const Object& obj : objects)
		{
			totalInMoment += obj.getPosition() * obj.getMass();
		}
		totalPosition = totalInMoment / totalMass;
	}

	void draw(sf::RenderWindow& window) 
{
    window.clear();
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

	

private:

	const float G  =      20000;
	const float dt = 1/200.f;

    float 				totalMass;
	sf::Vector2f    totalPosition;
	sf::Vector2f    totalInMoment;
	std::vector<Object>   objects;


    void updateTotalMass()
    {
        totalMass = 0;
        for(const Object& obj : objects)
        {
            totalMass += obj.getMass();
        }
    }
};




int main()
{
    // Создаём объекты
    Object a = {{600, 200}, {5, -20}, 1.5, 5};
    Object b = {{590, 300}, {-5, 20}, 1.5, 5};
	Object c = {{750, 200}, {0, 0}, 4.f, 5};
	Object d = {{750, 250}, {-30, 3}, 3.f, 5};
	System system({});
    // Окно SFML 
    sf::RenderWindow window(sf::VideoMode({1400u, 1000u}), "Gravity Simulation");

	bool isDragging = false;
	sf::Vector2f startPosition;
    while (window.isOpen())
    {
        sf::Event event;
		
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Запоминаем начальную позицию
                startPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				isDragging = true;
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                if (isDragging)
                {
                    // Вычисляем вектор скорости
                    sf::Vector2f endPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    sf::Vector2f velocity = startPosition - endPosition; // Вектор направлен от конца к началу

                    // Создаем новый объект с вычисленной скоростью
                    Object newObject(startPosition, velocity, 5.f, 5.f);
                    system.addObject(newObject);

                    isDragging = false;
                }
            }
        }

        system.updateVelocity();
		system.updatePosition(); // Обновляем симуляцию
        system.draw(window);
        window.display();


		printf("{%f, %f}\n", system.getInMoment().x, system.getInMoment().y);
    }
}