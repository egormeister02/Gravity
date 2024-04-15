#include "objects.h"

int main()
{
	System system({});
    float mass = 2.f;
    const float scaleFactor = 2.f;
	bool isDragging = false;
	sf::Vector2f startPosition;  
      // Окно SFML 
    sf::RenderWindow window(sf::VideoMode({1400u, 1000u}), "Gravity Simulation");

    std::cout << "OK\n";
    // Загрузка шрифта
    sf::Font font;
    if (!font.loadFromFile("../font.ttf")) // Укажите путь к файлу шрифта
    {
        std::cerr << "Could not load font\n";
        return -1;
    }

    // Настройка текста для отображения FPS
    Info infoSystem(system, font, 20);


    sf::Text InputMassText;
    InputMassText.setFont(font);
    InputMassText.setCharacterSize(24);
    InputMassText.setFillColor(sf::Color::White);
    InputMassText.setPosition(20, 10);

    sf::Text defaultMassText;
    defaultMassText.setFont(font);
    defaultMassText.setCharacterSize(24);
    defaultMassText.setFillColor(sf::Color(255, 255, 255, 128));
    defaultMassText.setString("default mass = 2");
    defaultMassText.setPosition(240, 10);
    
    sf::CircleShape cursor(5);
    int y_cursor = 20;
    cursor.setPosition(5, y_cursor);
    std::string inputString = "next object mass = 2";
    bool typing = true;
    // Таймер для измерения времени между кадрами
    sf::Clock clock;
    float frameTime = 0.f;
    int frameCount = 0;

    while (window.isOpen())
    {
        sf::Event event;
		
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        // Запоминаем начальную позицию
                        startPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        isDragging = true;
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (isDragging)
                    {
                        // Вычисляем вектор скорости
                        sf::Vector2f endPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        sf::Vector2f velocity = startPosition - endPosition; // Вектор направлен от конца к началу

                        // Создаем новый объект с вычисленной скоростью
                        Object newObject(startPosition, velocity, mass, 5.f);
                        system.addObject(newObject);

                        isDragging = false;
                    }
                    break;
                
                case sf::Event::TextEntered:
                    if (event.text.unicode < 128)
                        {
                            // If the key is backspace, remove the last character
                            if (event.text.unicode == '\b' && (inputString != "next object mass = "))
                            {
                                inputString.erase(inputString.size() - 1, 1);
                            }
                            else if ((event.text.unicode >= '0' && event.text.unicode <= '9') || event.text.unicode == '.')
                            {
                                // Append the new character to the string
                                inputString += static_cast<char>(event.text.unicode);
                            }
                        }
                        break;

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        // Escape key was pressed, clear the current input
                        inputString = "next object mass = 2";
                    }
                    else if (event.key.code == sf::Keyboard::Down)
                    {
                        // Escape key was pressed, clear the current input
                        y_cursor = y_cursor + 20;
                        cursor.setPosition(5, y_cursor);
                    }
                    break;

                    case sf::Event::MouseWheelScrolled:
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                    {
                        //std::cout << event.mouseWheelScroll.delta;
                        if (event.mouseWheelScroll.delta > 0)
                        {
                            // Прокрутка вверх, увеличиваем массу
                            mass *= scaleFactor;
                        }
                        else if (event.mouseWheelScroll.delta < 0)
                        {
                            // Прокрутка вниз, уменьшаем массу
                            mass /= scaleFactor;
                            if (mass < 0.000001)
                                mass = 0.000001;
                        }

                        inputString = "next object mass = " + std::to_string(mass);
                    }
                    break;

                case sf::Event::Resized:
                    {
                        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                        window.setView(sf::View(visibleArea));
                    }
                    break;

                default:
                    break;
            }
        }

        frameTime += clock.restart().asSeconds();
        frameCount++;
        if (frameTime >= 1.f) // Каждую секунду
        {
            infoSystem.updateFPS(frameCount);
            frameTime -= 1.f;
            frameCount = 0;
        }

        window.clear();

        std::string numericPart = inputString.substr(19);
        try 
        {
            mass = std::stof(numericPart);
        }
        catch (const std::invalid_argument& e) 
        {
            mass = 2;
            window.draw(defaultMassText);
        }

        InputMassText.setString(inputString);

        system.update(window);
        system.draw(window);

        infoSystem.update(window);
        infoSystem.draw(window);
        
        window.draw(InputMassText);     
        window.draw(cursor);
        
        window.display();
    }
}