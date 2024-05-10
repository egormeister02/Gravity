#include "objects.h"

int main()
{
	
    float mass = 2.f;
    const float scaleFactor = 2.f;
	bool isDragging = false;
	sf::Vector2f startPosition;  
      // Окно SFML 
    sf::RenderWindow window(sf::VideoMode({1400u, 1000u}), "Gravity Simulation");

    std::cout << "OK\n";
    // Загрузка шрифта
    sf::Font font;
    if (!font.loadFromFile("../font.ttf")) 
    {
        std::cerr << "Could not load font\n";
        return -1;
    }

    System system({});
    system.setWindowSize(window);

    Button::font = font;

    Info infoSystem(system, font, 20);
    Input inputSystem(font, 20);


    ClearButton clearButton({150, 50}, "clear");
    clearButton.updatePosition(window);
    clearButton.setAction([&system](){
        system.clearObjects(); // Очистка всех объектов в системе
    });

    Button pauseButton({10, inputSystem.getPosition().y + inputSystem.getSize().y + 40}, {150, 50}, "pause");
    pauseButton.setAction([&inputSystem]()
    {
        static float last_dt;
        if (inputSystem.getSpeed())
        {
            last_dt = inputSystem.getSpeed();
            inputSystem.setSpeed(0);
        }
        else
            inputSystem.setSpeed(last_dt);
    });

    Button centerButton({10, pauseButton.getPosition().y + pauseButton.getSize().y + 10}, {150, 50}, "to center");
    centerButton.setAction([&system, &window]()
    {
        sf::Vector2f shift = (static_cast<sf::Vector2f>(window.getSize()) / 2.f) - system.getCenterMass();
        system.move(shift);
    });

    Button stayButton({10, centerButton.getPosition().y + centerButton.getSize().y + 10}, {150, 50}, "stay");
    stayButton.setAction([&system]()
    {
        system.stayObjects();
    });

    // Таймер для измерения времени между кадрами
    sf::Clock clock;
    float frameTime = 0.f;
    int frameCount = 0;

    // Главный цикл
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
                        if (!(clearButton.handleEvent(event.mouseButton, window) || 
                              pauseButton.handleEvent(event.mouseButton, window) ||
                              centerButton.handleEvent(event.mouseButton, window) ||
                              stayButton.handleEvent(event.mouseButton, window)))
                        {
                            startPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                            isDragging = true;
                        }
                    }
                    break;

                case sf::Event::MouseButtonReleased:
                    if (isDragging)
                    {
                        // Вычисляем вектор скорости
                        sf::Vector2f endPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                        sf::Vector2f velocity = startPosition - endPosition; // Вектор направлен от конца к началу

                        // Создаем новый объект с вычисленной скоростью
                        Object newObject(startPosition, velocity, inputSystem.getMass(), 5.f);
                        system.addObject(newObject);

                        isDragging = false;
                    }
                    break;
                
                /*case sf::Event::TextEntered:
                    if (event.text.unicode < 128)
                        {
                            // If the key is backspace, remove the last character
                            if (event.text.unicode == '\b' && (inputMassString != "next object mass = "))
                            {
                                inputMassString.erase(inputMassString.size() - 1, 1);
                            }
                            else if ((event.text.unicode >= '0' && event.text.unicode <= '9') || event.text.unicode == '.')
                            {
                                // Append the new character to the string
                                inputMassString += static_cast<char>(event.text.unicode);
                            }
                        }
                        break;*/

                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        // Escape key was pressed, clear the current input
                        //inputMassString = "next object mass = 2";
                    }
                    else if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
                    {
                        inputSystem.changeMode(event.key.code);
                    }
                    
                    break;

                case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    inputSystem.wheelChangeValue(static_cast<int>(event.mouseWheelScroll.delta));
                }
                break;

                case sf::Event::Resized:
                    {
                        system.setWindowSize(window);
                        clearButton.updatePosition(window);
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


        system.set_dt(inputSystem.getSpeed());
        system.update(window);
        system.draw(window);

        infoSystem.update(window);
        infoSystem.draw(window);

        //inputSystem.updateText();
        inputSystem.draw(window);
        clearButton.draw(window);
        pauseButton.draw(window);
        centerButton.draw(window);
        stayButton.draw(window);
        
        window.display();
    }
}