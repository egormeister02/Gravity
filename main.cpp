#include "objects.h"

void updateTextPosition(sf::Text &fpsText, sf::RenderWindow &window) {
    float textWidth = fpsText.getLocalBounds().width + fpsText.getLocalBounds().left;
    fpsText.setPosition(static_cast<float>(window.getSize().x) - textWidth - 10, 10.f);
};


int main()
{
	System system({});
    float mass = 5.f;
    const float scaleFactor = 5.f;
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
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setString("FPS: ");
    updateTextPosition(fpsText, window);


    sf::Text InputMassText;
    InputMassText.setFont(font);
    InputMassText.setCharacterSize(24);
    InputMassText.setFillColor(sf::Color::White);
    InputMassText.setPosition(20, 10);

    sf::Text defaultMassText;
    defaultMassText.setFont(font);
    defaultMassText.setCharacterSize(24);
    defaultMassText.setFillColor(sf::Color(255, 255, 255, 128));
    defaultMassText.setString("default mass");
    defaultMassText.setPosition(240, 10);
    
    sf::CircleShape cursor(5);
    int y_cursor = 20;
    cursor.setPosition(5, y_cursor);
    std::string inputString = "next object mass = 5";
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
                    if (event.key.code == sf::Keyboard::Return)
                    {
                        // Enter key was pressed, stop typing
                        typing = false;
                        // Here you can convert the inputString to a float and create the object with the mass
                        // float mass = std::stof(inputString.substr(12)); // Extract the number part
                        // Create object with mass...
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                    {
                        // Escape key was pressed, clear the current input
                        inputString = "next object mass = 5";
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
                        }

                        inputString = "next object mass = " + std::to_string(mass);
                    }
                    break;

                case sf::Event::Resized:
                    {
                        updateTextPosition(fpsText, window);
                        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                        window.setView(sf::View(visibleArea));
                    }
                    break;

                default:
                    break;
            }
        }

        window.clear();

        frameTime += clock.restart().asSeconds();
        frameCount++;
        if (frameTime >= 1.f) // Каждую секунду
        {
            fpsText.setString("FPS: " + std::to_string(frameCount));
            updateTextPosition(fpsText, window);
            frameTime -= 1.f;
            frameCount = 0;
        }
        

        std::string numericPart = inputString.substr(19);
        try 
        {
            mass = std::stof(numericPart);
        }
        catch (const std::invalid_argument& e) 
        {
            mass = 5;
            window.draw(defaultMassText);
        }


        InputMassText.setString(inputString);
        system.updateVelocity();
		system.updatePosition(); // Обновляем симуляцию
        system.draw(window);
        window.draw(InputMassText); 
        window.draw(fpsText); 
        
        if (frameTime >= 0.5)
            window.draw(cursor);
        window.display();
    }
}