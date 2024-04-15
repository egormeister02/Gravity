#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <iostream>

const std::vector<std::string> info_names({"FPS: ", "Tatal mass: ", "Total moment: "});

float norm(const sf::Vector2f& v);

bool isInsideWindow(const sf::CircleShape& object, const sf::RenderWindow& window);

float updateRightTextPosition(sf::Text &fpsText,const float& y, const sf::RenderWindow &window);


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

	void update(sf::RenderWindow& window);

    void addObject(const Object& obj);

    float getTotalMass() const;

	sf::Vector2f getInMoment() const;

	void draw(sf::RenderWindow& window);	

private:

	const float G  =      20000;
	const float dt =    1/200.f;

    float 				totalMass;
	sf::Vector2f    totalPosition;
	sf::Vector2f    totalInMoment; 
	std::vector<Object>   objects;

	void eraseObject(Object& obj);

	void updateVelocity();

	void updatePosition();

	void updateTotalInMoment();

    void updateTotalMass();
};

class Info
{
	public:

	Info(const System& sys, const sf::Font& font, unsigned int characterSize);

	void updateFPS(const int& fps);

	void update(const sf::RenderWindow& window);

	void draw(sf::RenderWindow& window);

	private:

	const System&  		  system;

	float 			maxWidthName;
	float 			 	  height;

	struct Entry
	{
		sf::Text  name;
		sf::Text value;
	}; 

	std::vector<Entry> entries;

	void updateValues();

	void updatePosition(const sf::RenderWindow& window);

	float getMaxWidthName() const;

	void updateNamePosition(const float& min_x);

};