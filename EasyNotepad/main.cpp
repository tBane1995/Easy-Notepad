#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "SFML_intro.hpp"

sf::RenderWindow* window;

sf::Font font;
short characterSize;

std::wstring text;
std::vector < sf::Text* > lines;

sf::Vector2i mousePosition;
sf::Vector2f worldMousePosition;

sf::RectangleShape cursor;
sf::Vector2i cursorPosition = sf::Vector2i(0, 0);

sf::Clock timeClock;
sf::Time currentTime;


int selecting_start = 2; // selecting start cursor
int selecting_end = 4; // selecting end cursor

std::vector < sf::Text* > wrap_text(int line_width, std::wstring text) {
    
    std::vector < sf::Text* > wrapped_text;

    std::wstring line = L"";
    std::wstring word = L"";

    sf::Color textColor = sf::Color::White;

    for (auto& character : text) {

        if (sf::Text(line + word + character, font, characterSize).getGlobalBounds().width > line_width)
        {
            sf::Text* t = new sf::Text(line + L"\n", font, characterSize);
            (wrapped_text.empty()) ? t->setPosition(0, 0) : t->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
            t->setFillColor(textColor);
            wrapped_text.push_back(t);

            line = L"";
        }
        else if (character == L'\n') {

            if (sf::Text(line + word + L"\n", font, characterSize).getGlobalBounds().width > line_width) {
                sf::Text* t = new sf::Text(line, font, characterSize);
                (wrapped_text.empty()) ? t->setPosition(0, 0) : t->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
                t->setFillColor(textColor);
                wrapped_text.push_back(t);

                sf::Text* t2 = new sf::Text(word + L"\n", font, characterSize);
                (wrapped_text.empty()) ? t2->setPosition(0, 0) : t2->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
                t2->setFillColor(textColor);
                wrapped_text.push_back(t2);

                line = L"";
                word = L"";
            }
            else {
                sf::Text* t = new sf::Text(line + word + L"\n", font, characterSize);
                (wrapped_text.empty()) ? t->setPosition(0, 0) : t->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
                t->setFillColor(textColor);
                wrapped_text.push_back(t);

                line = L"";
                word = L"";
            }

        }
        else if (character == L' ' || character == L'\t') {
            if (sf::Text(line + word, font, characterSize).getGlobalBounds().width > line_width) {
                sf::Text* t = new sf::Text(line + L"\n", font, characterSize);
                (wrapped_text.empty()) ? t->setPosition(0, 0) : t->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
                t->setFillColor(textColor);
                wrapped_text.push_back(t);
                line = L"";
                
            }
            else {
                line = line + word + character;
            }

            word = L"";
        }
        else {
            word = word + character;
        }
    }

    if (line != L"" || word != L"") {
        sf::Text* t = new sf::Text(line + word, font, characterSize);
        (wrapped_text.empty()) ? t->setPosition(0, 0) : t->setPosition(0, wrapped_text.back()->getPosition().y + font.getLineSpacing(characterSize));
        t->setFillColor(textColor);
        wrapped_text.push_back(t);
    }

    return wrapped_text;

}


sf::Vector2i getCursorPosition() {

    sf::Vector2i cur_pos = sf::Vector2i(0, 0);

    for (int t = 0; t < lines.size(); t++) {

        for (size_t i = 0; i < lines[t]->getString().getSize(); ++i) {

            sf::Vector2f charPos = lines[t]->findCharacterPos(i);
            float nextX = lines[t]->findCharacterPos(i + 1).x;

            sf::FloatRect charRect(charPos.x, charPos.y, nextX - charPos.x, font.getLineSpacing(characterSize));

            if (charRect.contains(worldMousePosition)) {

                if (worldMousePosition.x < charRect.left + charRect.width / 2)
                    return sf::Vector2i(i, t);
                else
                    return sf::Vector2i(i + 1, t);

            }

            bool isLastChar = (i == lines[t]->getString().getSize() - 1);
            if (isLastChar &&
                worldMousePosition.x > charRect.left &&
                worldMousePosition.y >= charRect.top &&
                worldMousePosition.y <= charRect.top + charRect.height)
            {
                return sf::Vector2i(i + 1, t);
            }
        }
    }

    return cur_pos;
}

int getCursorIndex(sf::Vector2i position) {
    int index = 0;
    for (int i = 0; i < position.y && i < lines.size(); ++i) {
        index += lines[i]->getString().getSize();
    }
    index += position.x;
    return index;
}

sf::Vector2i getCursorFromIndex(int index) {
    if (index <= 0 || lines.empty())
        return sf::Vector2i(0, 0);

    int current = 0;
    for (int y = 0; y < lines.size(); ++y) {
        int lineSize = lines[y]->getString().getSize();

        if (index <= current + lineSize) {
            return sf::Vector2i(index - current, y);
        }

        current += lineSize;
    }

    // Jeśli index wykracza poza długość tekstu, ustaw na koniec ostatniej linii
    return sf::Vector2i(lines.back()->getString().toWideString().size(), lines.size() - 1);

    
}

void setCursorUp() {

    if(lines.empty())
        return;

    if (cursorPosition.y > 0) {

        float targetX = cursor.getGlobalBounds().left;

        cursorPosition.y -= 1;
        sf::Text* line = lines[cursorPosition.y];
        size_t lineLength = line->getString().toWideString().size();

        size_t closestIndex = 0;
        float closestDistance = std::abs(line->findCharacterPos(0).x - targetX);

        for (size_t i = 1; i <= lineLength; ++i) {
            sf::Vector2f pos = line->findCharacterPos(i);
            float distance = std::abs(pos.x - targetX);

            if (distance < closestDistance) {
                closestIndex = i;
                closestDistance = distance;
            }
        }

        cursorPosition.x = closestIndex;
        cursor.setPosition(line->findCharacterPos(closestIndex));
    }
}
void setCursorDown() {

    if (lines.empty())
        return;

    if (cursorPosition.y < lines.size() - 1) {

        float targetX = cursor.getGlobalBounds().left;

        cursorPosition.y += 1;
        sf::Text* line = lines[cursorPosition.y];
        size_t lineLength = line->getString().toWideString().size();

        size_t closestIndex = 0;
        float closestDistance = std::abs(line->findCharacterPos(0).x - targetX);

        for (size_t i = 1; i <= lineLength; ++i) {
            sf::Vector2f pos = line->findCharacterPos(i);
            float distance = std::abs(pos.x - targetX);

            if (distance < closestDistance) {
                closestIndex = i;
                closestDistance = distance;
            }
        }

        cursorPosition.x = closestIndex;
        cursor.setPosition(line->findCharacterPos(closestIndex));
    }

}

void setCursorPosition(sf::Vector2i cursor_position) {
    cursorPosition = cursor_position;

    if (cursor_position == sf::Vector2i(0, 0)) {
        cursor.setPosition(sf::Vector2f(0, 0));
        return;
    }
        

    for (int t = 0; t < lines.size(); t++) {
        if (t == cursor_position.y) {
            
            std::wstring line = lines[t]->getString().toWideString();

            if (line.size() == 0) {
                cursor.setPosition(lines[t]->getPosition());    // poprawka
                return;
            }

            if (cursor_position.x < line.size()) {
                sf::Vector2f charPos = lines[t]->findCharacterPos(cursor_position.x);
                cursor.setPosition(charPos.x, charPos.y);
                return;
            }
            

            // Kursor na końcu linii
            sf::Vector2f endPos = lines[t]->findCharacterPos(line.size());
            cursor.setPosition(endPos.x, endPos.y);
            return;
        }
    }
}

int main()
{

    sf::View view(sf::FloatRect(0, 0, 480, 640));
    window = new sf::RenderWindow(sf::VideoMode(view.getSize().x, view.getSize().y), "Easy Notepad!", sf::Style::Titlebar | sf::Style::Close);

    font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    characterSize = 17;
    
    text = L"";

    lines = wrap_text(window->getSize().x, text);

    cursor = sf::RectangleShape(sf::Vector2f(2, characterSize));
    cursor.setFillColor(sf::Color::Red);

    sf::Clock clock;

    SFML_intro* intro = new SFML_intro(window);
    delete intro;

    while (window->isOpen())
    {

        mousePosition = sf::Mouse::getPosition(*window); // get the mouse position about window
        worldMousePosition = window->mapPixelToCoords(mousePosition); // get global mouse position

        currentTime = timeClock.getElapsedTime();

        sf::Event event;
        while (window->pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window->close();

            if (event.type == sf::Event::Resized)
            {
                
                // Aktualizacja widoku bez skalowania
                sf::View view;
                view.setSize(static_cast <float>(event.size.width), static_cast <float>(event.size.height));
                view.setCenter(view.getSize() / 2.f);
                window->setView(view);
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i cur_pos = getCursorPosition();
                setCursorPosition(cur_pos);
            }else if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::V) {
                // Ctrl + V 
                int index = getCursorIndex(cursorPosition);
                
                std::wstring clip_text = sf::Clipboard::getString().toWideString();
                while (clip_text.find(L"\r\n") != std::wstring::npos) {
                    clip_text.erase(clip_text.find(L"\r\n"), 1);
                };

                text.insert(index, clip_text);
                index += clip_text.size();

                lines = wrap_text(window->getSize().x, text);
                cursorPosition = getCursorFromIndex(index);
                setCursorPosition(cursorPosition);
                
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                int index = getCursorIndex(cursorPosition);
                if (!text.empty()) {
                    text.erase(index, 1);
                    lines = wrap_text(window->getSize().x, text);
                }
            }
            else if (event.type == sf::Event::TextEntered) {
                
                int index = getCursorIndex(cursorPosition);

                if (event.text.unicode == 8) {
                        // backspace
                    if (index > 0 && !text.empty()) {
                        text.erase(index - 1, 1);
                        index -= 1;
                    }
                }
                else if (event.text.unicode == 13) {
                    // enter
                    text.insert(index, 1, L'\n');
                    index += 1;
                        
                }
                else if (event.text.unicode == 9) {
                    // tab
                    text.insert(index, 1, L'\t');
                    index += 1;

                }
                else {
                    // other character
                    wchar_t character = static_cast<wchar_t>(event.text.unicode);
                    if (character >= 32 && character != 127) {
                        text.insert(index, 1, character);
                        index += 1;
                    }
                }

                for (auto& t : lines)
                    delete t;
                //lines = wrapText(window->getSize().x);
                lines = wrap_text(window->getSize().x, text);

                // Po każdej zmianie kursor wstawiamy po index
                cursorPosition = getCursorFromIndex(index);
                setCursorPosition(cursorPosition);
               


            }
            else if (event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Left) {
                    if (cursorPosition.x > 0) {
                        cursorPosition.x -= 1;
                    }
                    else {
                        if (!lines.empty() && cursorPosition.y > 0) {
                            cursorPosition.y -= 1;
                            cursorPosition.x = lines[cursorPosition.y]->getString().toWideString().size() - 1; //////// -1
                        }
                    }

                }
                else if (event.key.code == sf::Keyboard::Right) {
                    if (!lines.empty() && cursorPosition.x < lines[cursorPosition.y]->getString().toWideString().size()) { //////// +1
                        cursorPosition.x += 1;
                    }
                    else {
                        if (cursorPosition.y < lines.size() - 1) {
                            cursorPosition.x = 0;
                            cursorPosition.y += 1;
                        }
                    }
                }
                else if (event.key.code == sf::Keyboard::Up) {
                    setCursorUp();

                }
                else if (event.key.code == sf::Keyboard::Down) {
                    setCursorDown();

                }


                setCursorPosition(cursorPosition);

            }


        }

        window->clear(sf::Color(48, 48, 48, 255));
        for (auto& line : lines)
            window->draw(*line);

        if (std::fmod(currentTime.asSeconds(), 0.6f) < 0.3f)
            window->draw(cursor);

        window->display();
    }
}