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

	void Object::setVelocity(sf::Vector2f vel)
	{
		velocity = vel;
	}

	sf::Vector2f Object::getVelocity() const
	{
		return velocity;
	}


    System::System(const std::vector<Object>& obj)
		: objects(obj)
    {
		// Устанавливаем толщину границы и цвет
    	frame.setOutlineThickness(-3); // Отрицательное значение для внутренней границы
    	frame.setOutlineColor(sf::Color::White); // Цвет границы - белый
    	frame.setFillColor(sf::Color::Transparent); // Внутренняя часть прозрачная

        updateTotalMass();
		updateTotalInMoment();
		updateTotaAngular();
	}

	void System::setWindowSize(sf::RenderWindow& window) {
        windowSize = static_cast<sf::Vector2f>(window.getSize());
		frame.setSize(windowSize);
    }

    void System::setMode(Mode newMode) {
        mode = newMode;
    }

	void System::move(sf::Vector2f shift)
	{
		for (Object& obj : objects)
			obj.setPosition(obj.getPosition() + shift);

		updateTotalMass();
		updateTotalInMoment();
		updateTotaAngular();
	}
	
    void System::addObject(const Object& obj)
    {
        objects.push_back(obj);
        totalMass += obj.getMass();
		totalInMoment += obj.getMass()* obj.getPosition();
    }

	void System::clearObjects() {
        objects.clear();
        // Также необходимо обновить общую массу и момент системы
        updateTotalMass();
        updateTotalInMoment();
		updateTotaAngular();
    }

	void System::stayObjects()
	{
		sf::Vector2f d_vel = -totalAngular / totalMass;
		for (Object& obj : objects)
			obj.changeVelocity(d_vel);

		updateTotalMass();
        updateTotalInMoment();
	}

	sf::Vector2f System::getTotalAngular() const
	{
		return totalAngular;
	}

	System::Mode System::getMode() const
	{
		return mode;
	}

	sf::Vector2f System::getCenterMass() const
	{
		return centerMass;
	};


	float System::get_dt() const
	{
		return dt * 10000;
	}

	void System::set_dt(float d_t)
	{
		dt = d_t / 10000;
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

			if (mode == WithWalls) 
			{
                sf::Vector2f pos = objects[i].getPosition();
                sf::Vector2f vel = objects[i].getVelocity();

                if (pos.x < 0 || pos.x > windowSize.x) {
                    vel.x *= -1; // Отражение по горизонтали
                }
                if (pos.y < 0 || pos.y > windowSize.y) {
                    vel.y *= -1; // Отражение по вертикали
                }

                objects[i].setVelocity(vel);
            }
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
		centerMass = totalInMoment / totalMass;
	}

	void System::updateTotaAngular()
	{
		totalAngular = {0, 0};
		for(const Object& obj : objects)
		{
			totalAngular += static_cast<sf::Vector2f>(obj.getMass() * obj.getVelocity());
		}
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
		updateTotaAngular();
	}

	void System::draw(sf::RenderWindow& window) 
    {
        for (Object& obj : objects)
            window.draw(obj);

		if (mode == WithWalls)
			window.draw(frame);
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
		entries[3].value.setString('{' + std::to_string(system.getTotalAngular().x) + ", " + std::to_string(system.getTotalAngular().y) + '}');
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

	void Info::draw(sf::RenderWindow& window) const
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


	Input::Input(const sf::Font& font, unsigned int characterSize)
	{
		for (int i = 0; i < inputNames.size(); i++) 
		{
			entries.push_back({sf::Text(inputNames[i], font, characterSize), sf::Text("", font, characterSize)});
			entries[i].name.setFillColor(sf::Color::White);
			entries[i].value.setFillColor(sf::Color::White);
		}

		maxWidthName = getMaxWidthName();
		height = entries[0].name.getLocalBounds().height;

		float minY = std::numeric_limits<float>::max();
		float maxY = 0.f;
		float minX = std::numeric_limits<float>::max();
		float maxX = 0.f;

		int i = 0;
		for (Entry& entry : entries)
		{   
			entry.name.setPosition({20, 20 + (height + 5) * i});
			entry.value.setPosition({entry.name.getPosition().x + maxWidthName, 20 + (height + 5) * i});

			// Обновляем minY и maxY для overallBounds
			minY = std::min(minY, entry.name.getPosition().y);
			maxY = std::max(maxY, entry.name.getPosition().y + entry.name.getLocalBounds().height);
			
			// Обновляем minX и maxX для overallBounds
			minX = std::min(minX, entry.name.getPosition().x);
			maxX = std::max(maxX, entry.value.getPosition().x + entry.value.getLocalBounds().width);

			++i;
		}

		updateText();

		cursor = sf::CircleShape(5);
		cursor.setPosition({5, entries[static_cast<int>(mode)].name.getPosition().y + height / 2});

		// Обновляем minX и maxX для overallBounds с учетом курсора
		minX = std::min(minX, cursor.getPosition().x);
		maxX = std::max(maxX, cursor.getPosition().x + cursor.getRadius() * 2);

		// Устанавливаем overallBounds
		overallBounds.left = minX;
		overallBounds.top = minY;
		overallBounds.width = maxX - minX;
		overallBounds.height = maxY - minY;
	}

	float Input::getMaxWidthName() const
	{
		float maxWidth = 0;
		for (const Entry& entry : entries)
		{
			if (entry.name.getLocalBounds().width > maxWidth)
				maxWidth = entry.name.getLocalBounds().width;
		}
		return maxWidth;
	}

	sf::Vector2f Input::getPosition() const
    {
        return {overallBounds.left, overallBounds.top};
    }

    // Метод для получения размера области ввода
    sf::Vector2f Input::getSize() const
    {
        return {overallBounds.width, overallBounds.height};
    }

	void Input::wheelChangeValue(int direction)
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

	void Input::draw(sf::RenderWindow& window) const
	{
		for (const Entry& entry: entries)
		{
			window.draw(entry.value);
			window.draw(entry.name);
		}
		window.draw(cursor);
	}

	void Input::changeMode(sf::Keyboard::Key key)
	{
		if (key == sf::Keyboard::Up)
			mode = static_cast<InputMode>((mode + nModes - 1) % nModes);
		else	
			mode = static_cast<InputMode>((mode + 1) % nModes);

		cursor.setPosition({5, entries[static_cast<int>(mode)].name.getPosition().y + height / 2});
	}

	InputMode Input::getMode() const
	{
		return mode;
	}

	float Input::getMass() const
	{
		return mass;
	}
	float Input::getSpeed() const
	{
		return speed;
	}

	void Input::setSpeed(float newSpeed)
	{
		speed = newSpeed;
	}

	void Input::updateText()
	{
		entries[0].value.setString(std::to_string(mass));
		entries[1].value.setString(std::to_string(speed));
	}


    Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, const std::string& text)
    : action([](){}) // Инициализация пустой функцией
    {
        // Установка формы кнопки
        shape.setPosition(position);
        shape.setSize(size);
        shape.setFillColor(sf::Color(255, 255, 255, 128));

        // Установка текста кнопки
        buttonText.setFont(font);
        buttonText.setString(text);
        buttonText.setCharacterSize(24);
        buttonText.setFillColor(sf::Color::Black);

        centerText();
    }

    void Button::setAction(const std::function<void()>& newAction) {
        action = newAction;
    }

    bool Button::handleEvent(const sf::Event::MouseButtonEvent& event, const sf::RenderWindow& window) 
	{
        if (shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Vector2i(event.x, event.y)))) 
		{
            action(); // Вызов связанного действия
			return 1;
		}
		return 0;
    }

	sf::Vector2f  Button::getPosition() const
	{
		return shape.getPosition();
	}

	sf::Vector2f Button::getSize() const
	{
		return shape.getSize();
	}

    void Button::draw(sf::RenderWindow& window) const 
	{
        window.draw(shape);
        window.draw(buttonText);
    }


	void Button::centerText() 
	{
        sf::FloatRect textRect = buttonText.getLocalBounds();
        buttonText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top  + textRect.height/2.0f);
        sf::Vector2f buttonPos = shape.getPosition();
        sf::Vector2f buttonSize = shape.getSize();
        buttonText.setPosition(buttonPos.x + buttonSize.x/2.0f, buttonPos.y + buttonSize.y/2.0f);
    }

	void ClearButton::updatePosition(const sf::RenderWindow& window) {
        sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
        sf::Vector2f buttonSize = shape.getSize();
        float padding = 10.0f; // Отступ от краев окна

        // Установка новой позиции кнопки
        shape.setPosition(windowSize.x - buttonSize.x - padding, windowSize.y - buttonSize.y - padding);
        centerText();
    }
