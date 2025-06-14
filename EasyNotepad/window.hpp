#ifndef window_hpp
#define window_hpp

sf::View view(sf::FloatRect(0, 0, 480, 640));
sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(view.getSize().x, view.getSize().y), "Easy Notepad!", sf::Style::Titlebar | sf::Style::Close);

#endif
