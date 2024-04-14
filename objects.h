#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <iostream>

// Объекты
float norm(const sf::Vector2f& v);

bool isInsideWindow(const sf::CircleShape& object, const sf::RenderWindow& window);


class Object: public sf::CircleShape
{
	public:

	Object(sf::Vector2f pos, sf::Vector2f vel, float mass, float size);

	void changeVelocity(sf::Vector2f dv);

	float getMass() const;

	sf::Vector2f getVelocity() const;

	private:
    sf::Vector2f 	velocity;
    float 			 	mass;

};

class System
{
public:

    System(const std::vector<Object>& obj);

    void addObject(const Object& obj);

	void eraseObject(Object& obj);

    float getTotalMass() const;

	sf::Vector2f getInMoment() const;
	
	void updateVelocity();

	void updatePosition();

	void updateTotalInMoment();

	void draw(sf::RenderWindow& window);

	

private:

	const float G  =      20000;
	const float dt =    1/200.f;

    float 				totalMass;
	sf::Vector2f    totalPosition;
	sf::Vector2f    totalInMoment;
	std::vector<Object>   objects;


    void updateTotalMass();
};