#include <SFML/Graphics.hpp>
#include <math.h>
#include <vector>
#include <iostream>
#include <functional>


enum InputMode
{
	InputMass = 0,
	InputSpeed = 1
};

const std::vector<std::string> info_names({"FPS: ", "Tatal mass: ", "Total moment: ", "Total angular:"});
const std::vector<std::string> inputNames({"next object mass: ", "Speed: "});

float norm(const sf::Vector2f& v);

bool isInsideWindow(const sf::CircleShape& object, const sf::RenderWindow& window);

float updateRightTextPosition(sf::Text &fpsText,const float& y, const sf::RenderWindow &window);


class Object: public sf::CircleShape
{
	public:

	Object(sf::Vector2f pos, sf::Vector2f vel, float mass, float size);

	void changeVelocity(sf::Vector2f dv);

	void setVelocity(sf::Vector2f vel);

	float getMass() const;

	sf::Vector2f getVelocity() const;

	private:
    sf::Vector2f 	velocity;
    float 			 	mass;

};



class System
{
public:

	enum Mode {
		Normal,
		WithWalls
	};

    System(const std::vector<Object>& obj);

	void setWindowSize(sf::RenderWindow& window);

    void setMode(Mode newMode);

	Mode getMode() const;

	void move(sf::Vector2f shift);

	void update(sf::RenderWindow& window);

    void addObject(const Object& obj);

	void clearObjects();

	void stayObjects();

    float getTotalMass() const;

	void updateTotaAngular();

	sf::Vector2f getTotalAngular() const;

	sf::Vector2f getInMoment() const;

	sf::Vector2f getCenterMass() const;

	float get_dt() const;

	void set_dt(float);

	void draw(sf::RenderWindow& window);	

private:

	sf::Vector2f windowSize; // Размеры окна
    Mode mode = WithWalls; // Режим работы системы

	const float G  =      20000;
	float dt	   =    1/200.f;

	sf::RectangleShape frame;

    float 				totalMass;
	sf::Vector2f       centerMass;
	sf::Vector2f    totalInMoment; 
	sf::Vector2f     totalAngular;
	std::vector<Object>   objects;

	void eraseObject(Object& obj);

	void updateVelocity();

	void updatePosition();

	void updateTotalInMoment();

    void updateTotalMass();
};

struct Entry
	{
		sf::Text  name;
		sf::Text value;
	}; 

class Info
{
	public:

	Info(const System& sys, const sf::Font& font, unsigned int characterSize);

	void updateFPS(const int& fps);

	void update(const sf::RenderWindow& window);



	void draw(sf::RenderWindow& window) const;

	private:

	const System&  		  system;

	float 			maxWidthName;
	float 			 	  height;

	std::vector<Entry> entries;

	void updateValues();

	void updatePosition(const sf::RenderWindow& window);

	float getMaxWidthName() const;

	void updateNamePosition(const float& min_x);

};

class Input
{
	public:

	Input(const sf::Font& font, unsigned int characterSize);

	sf::Vector2f getPosition() const;

    sf::Vector2f getSize() const;

	void wheelChangeValue(int direction);

	void draw(sf::RenderWindow& window) const;

	void changeMode(sf::Keyboard::Key key);

	InputMode getMode() const;

	float getMass() const;

	float getSpeed() const;

	void setSpeed(float newSpeed);
	
	private:

	InputMode          mode   = InputMode::InputMass;
	const unsigned int nModes = 				   2;

	float mass  =  2;
	float speed = 50;

	float 			 	  height;
	float 			maxWidthName;

	std::vector<Entry> entries;
	
    sf::CircleShape cursor;

	sf::FloatRect overallBounds;

	void updateText();

	void updateValues();

	float getMaxWidthName() const;

	void updatePosition(const sf::RenderWindow& window);

};

class Button {
public:

	static inline sf::Font font;

    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text);

    void setAction(const std::function<void()>& newAction);

    bool handleEvent(const sf::Event::MouseButtonEvent& event, const sf::RenderWindow& window);

	sf::Vector2f  getPosition() const;

	sf::Vector2f getSize() const;

    void draw(sf::RenderWindow& window) const;

	protected:
    sf::RectangleShape shape;
    sf::Text buttonText;
    std::function<void()> action; // Функция, которая будет вызвана при нажатии

	void centerText();
};

class ClearButton : public Button
{
	public:

	ClearButton(const sf::Vector2f& size, const std::string& text)
    : Button({0, 0}, size, text) {}

	void updatePosition(const sf::RenderWindow& window);
};