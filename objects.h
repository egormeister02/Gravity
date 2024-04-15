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

    float 				totalMass = 0;
	sf::Vector2f    totalPosition = {0, 0};
	sf::Vector2f    totalInMoment = {0, 0};
	std::vector<Object>   objects;


    void updateTotalMass();
};

class Info
{
	public:

	Info(const System& sys, const sf::Font& font, unsigned int characterSize): system(sys)
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

	void updateValues(const int& fps)
	{
		entries[0].value.setString(std::to_string(fps));
		entries[1].value.setString(std::to_string(system.getTotalMass()));
		entries[2].value.setString('{' + std::to_string(system.getInMoment().x) + ", " + std::to_string(system.getInMoment().y) + '}');
	}

	void updatePosition(const sf::RenderWindow& window)
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

	void draw(sf::RenderWindow& window)
	{
		for (const Entry& entry: entries)
		{
			window.draw(entry.value);
			window.draw(entry.name);
		}
	}

	private:

	const System& system;
	float maxWidthName;
	float height;

	struct Entry
	{
		sf::Text  name;
		sf::Text value;
	}; 

	std::vector<Entry> entries;

	float getMaxWidthName() const
	{
		float maxWidth = 0;
		for (const Entry& entry : entries)
		{
			if (entry.name.getLocalBounds().width > maxWidth)
				maxWidth = entry.name.getLocalBounds().width;
		}
		return maxWidth;
	}

	void updateNamePosition(const float& min_x)
	{
		for (Entry& entry : entries)
		{
			entry.name.setPosition({min_x - maxWidthName, static_cast<float>(entry.value.getPosition().y)});
		}
	}

};