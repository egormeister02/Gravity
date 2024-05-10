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

const std::vector<std::string> info_names({"FPS: ", "Tatal mass: ", "Total moment: "});
const std::vector<std::string> inputNames({"next object mass: ", "Speed: "});

float norm(const sf::Vector2f& v);

bool isInsideWindow(const sf::CircleShape& object, const sf::RenderWindow& window);

float updateRightTextPosition(sf::Text &fpsText,const float& y, const sf::RenderWindow &window);


class Object: public sf::CircleShape
{
	public:

	Object(sf::Vector2f pos, sf::Vector2f vel, float mass, float size);

	void changeVelocity(sf::Vector2f dv);

	void setVelocity(sf::Vector2f vel)
	{
		velocity = vel;
	}

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

	void setWindowSize(sf::RenderWindow& window) {
        windowSize = static_cast<sf::Vector2f>(window.getSize());
    }

    void setMode(Mode newMode) {
        mode = newMode;
    }

	void update(sf::RenderWindow& window);

    void addObject(const Object& obj);

	void clearObjects() {
        objects.clear();
        // Также необходимо обновить общую массу и момент системы
        updateTotalMass();
        updateTotalInMoment();
    }

    float getTotalMass() const;

	sf::Vector2f getInMoment() const;

	float get_dt() const;

	void set_dt(float);

	void draw(sf::RenderWindow& window);	

private:

	sf::Vector2f windowSize; // Размеры окна
    Mode mode = WithWalls; // Режим работы системы

	const float G  =      20000;
	float dt	   =    1/200.f;

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

	sf::Vector2f getPosition() const
    {
        return {overallBounds.left, overallBounds.top};
    }

    // Метод для получения размера области ввода
    sf::Vector2f getSize() const
    {
        return {overallBounds.width, overallBounds.height};
    }

	void wheelChangeValue(int direction)
	{
		if (mode == InputMass)
    	{
            mass *=          pow(2, direction);
			mass  = (std::min(mass, 100000.f));
            mass  =      (std::max(mass, 0.f));
        }
        if (mode == InputSpeed)
        {
			speed *=       pow(1.2, direction);
			speed  = (std::min(speed, 1000.f));
            speed  =    (std::max(speed, 0.f));
		}
		updateText();
	}

	void draw(sf::RenderWindow& window) const
	{
		for (const Entry& entry: entries)
		{
			window.draw(entry.value);
			window.draw(entry.name);
		}
		window.draw(cursor);
	}

	void changeMode(sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Up)
			mode = static_cast<InputMode>((mode + nModes - 1) % nModes);
		else	
			mode = static_cast<InputMode>((mode + 1) % nModes);

		cursor.setPosition({5, entries[static_cast<int>(mode)].name.getPosition().y + height / 2});
	}

	void getMode() const;

	float getMass() const
	{
		return mass;
	}
	float getSpeed()
	{
		return speed;
	}
	
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

	void updateText()
	{
		entries[0].value.setString(std::to_string(mass));
		entries[1].value.setString(std::to_string(speed));
	}

	void updateValues();

	float getMaxWidthName() const;

	void updatePosition(const sf::RenderWindow& window);

};

class Button {
public:

	static inline sf::Font font;

    Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text)
    : action([](){}) // Инициализация пустой функцией
    {
        // Установка формы кнопки
        shape.setPosition(position);
        shape.setSize(size);
        shape.setFillColor(sf::Color::White);

        // Установка текста кнопки
        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::Black);

        centerText();
    }

    void setAction(const std::function<void()>& newAction) {
        action = newAction;
    }

    bool handleEvent(const sf::Event::MouseButtonEvent& event, const sf::RenderWindow& window) 
	{
        if (shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Vector2i(event.x, event.y)))) 
		{
            action(); // Вызов связанного действия
			return 1;
		}
		return 0;
    }

	sf::Vector2f  getPosition() const
	{
		return shape.getPosition();
	}

	sf::Vector2f getSize() const
	{
		return shape.getSize();
	}

    void draw(sf::RenderWindow& window) const 
	{
        window.draw(shape);
        window.draw(buttonText);
    }

	protected:
    sf::RectangleShape shape;
    sf::Text buttonText;
    std::function<void()> action; // Функция, которая будет вызвана при нажатии

	void centerText() {
        sf::FloatRect textRect = buttonText.getLocalBounds();
        buttonText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        sf::Vector2f buttonPos = shape.getPosition();
        sf::Vector2f buttonSize = shape.getSize();
        buttonText.setPosition(buttonPos.x + buttonSize.x/2.0f, buttonPos.y + buttonSize.y/2.0f);
    }
};

class ClearButton : public Button
{
	public:

	ClearButton(const sf::Vector2f& size, const std::string& text)
    : Button({0, 0}, size, text) {}

	void updatePosition(const sf::RenderWindow& window) {
        sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
        sf::Vector2f buttonSize = shape.getSize();
        float padding = 10.0f; // Отступ от краев окна

        // Установка новой позиции кнопки
        shape.setPosition(windowSize.x - buttonSize.x - padding, windowSize.y - buttonSize.y - padding);
        centerText();
    }
};