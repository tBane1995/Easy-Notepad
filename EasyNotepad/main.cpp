#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <functional>
#include <cmath>
#include <string>

#include "window.hpp"
#include "SFML_intro.hpp"
#include "mouse.hpp"
#include "text_wrapper.hpp"
#include "elementGUI.hpp"
#include "scrollbar.hpp"


sf::Color text_color = sf::Color(188, 190, 196);
sf::Color background_color = sf::Color(30, 31, 34, 255);
sf::Color selection_color = sf::Color(33, 66, 131, 255);

sf::Font font;
short characterSize;

Scrollbar* scrollbar;

std::wstring text;
std::vector < sf::Text* > lines;

sf::RectangleShape cursor;
sf::Vector2i cursorPosition = sf::Vector2i(0, 0);

sf::Clock timeClock;
sf::Time currentTime;


sf::Vector2i getCursorPosition() {

    if (lines.empty())
        return sf::Vector2i(0, 0);

    if (worldMousePosition.y >= lines.back()->getGlobalBounds().top) {

        for (size_t i = 0; i < lines.back()->getString().toWideString().size(); ++i) {
            sf::Vector2f charPos = lines.back()->findCharacterPos(i);
            sf::Vector2f nextPos = lines.back()->findCharacterPos(i + 1);

            float charWidth = nextPos.x - charPos.x;

            if (worldMousePosition.x < charPos.x + charWidth / 2.f) {
                //std::cout << "cursor on position: " << i << "," << lines.size() - 1 << "\n";
                return sf::Vector2i(i, lines.size() - 1);
            }
        }

        //std::cout << "cursor on position: " << lines.back()->getString().toWideString().size() << "," << lines.size() - 1 << "\n";
        return sf::Vector2i(lines.back()->getString().toWideString().size(), lines.size() - 1);
    }
        
    

    for (int t = 0; t < lines.size(); t++) {

        

        for (size_t i = 0; i < lines[t]->getString().toWideString().size() - 1; ++i) {

            sf::Vector2f charPos = lines[t]->findCharacterPos(i);
            float nextX = lines[t]->findCharacterPos(i + 1).x;

            sf::FloatRect charRect(charPos.x, charPos.y, nextX - charPos.x, font.getLineSpacing(characterSize));

            if (charRect.contains(worldMousePosition)) {

                if (worldMousePosition.x < charRect.left + charRect.width / 2) {
                    //std::cout << "cursor on position: " << i << "," << t << "\n";
                    return sf::Vector2i(i, t);
                }
                else {
                    //std::cout << "cursor on position: " << i+1 << "," << t << "\n";
                    return sf::Vector2i(i + 1, t);
                }
                    

            }

        }

        sf::Vector2f charPos; 
        if (lines[t]->getString().toWideString().back() == L'\n') {

            charPos = lines[t]->findCharacterPos(lines[t]->getString().toWideString().size() - 1);
            if (worldMousePosition.y >= charPos.y &&
                worldMousePosition.y <= charPos.y + font.getLineSpacing(characterSize) &&
                worldMousePosition.x >= charPos.x) {
                //std::cout << "cursor on position: " << lines[t]->getString().toWideString().size()-1 << "," << t << "\n";
                return sf::Vector2i(lines[t]->getString().toWideString().size()-1, t);
            }
        }
        else {
            charPos = lines[t]->findCharacterPos(lines[t]->getString().toWideString().size());
            if (worldMousePosition.y >= charPos.y &&
                worldMousePosition.y <= charPos.y + font.getLineSpacing(characterSize) &&
                worldMousePosition.x >= charPos.x) {
                //std::cout << "cursor on position: " << lines[t]->getString().toWideString().size() << "," << t << "\n";
                return sf::Vector2i(lines[t]->getString().toWideString().size(), t);
            }
        }
            

        

    }

    return sf::Vector2i(lines.back()->getString().toWideString().size(), lines.size() - 1);
}

int getCursorIndex(sf::Vector2i position) {
    int index = 0;
    for (int i = 0; i < position.y && i < lines.size(); ++i) {
        index += lines[i]->getString().toWideString().size();
    }
    index += position.x;
    //std::cout << "index: " << index << "\n";
    return index;
}

sf::Vector2i getCursorFromIndex(int index) {
    if (lines.empty())
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

void linesPositioning() {

    sf::Vector2f position(0, float(-scrollbar->scroll_value) * font.getLineSpacing(characterSize));

    for (int i = 0; i < lines.size();i+=1) {
        sf::Vector2f p = sf::Vector2f(0, float(i) * font.getLineSpacing(characterSize));
        p = p + sf::Vector2f(position);
        lines[i]->setPosition(p);
    }
}

class Selection {
public:

    int start_index = 0;
    int end_index = 0;

    Selection() {
        start_index = 0;
        end_index = 0;
    }

    ~Selection() {}

    void draw() {

        sf::Vector2i start_position;
        sf::Vector2i end_position;

        if (start_index < end_index) {
            start_position = getCursorFromIndex(start_index);
            end_position = getCursorFromIndex(end_index);
        }
        else {
            start_position = getCursorFromIndex(end_index);
            end_position = getCursorFromIndex(start_index);
        }

        int start_x = start_position.x;
        int end_x = end_position.x;

        int start_y = start_position.y;
        int end_y = end_position.y;

        float rect_start_x = (start_x == 0) ? 0 : lines[start_y]->findCharacterPos(start_x).x;
        float rect_end_x = lines[end_y]->findCharacterPos(end_x).x;

        float rect_start_y = (start_x == 0) ? 0 : lines[start_y]->findCharacterPos(start_x).y;
        float rect_end_y = lines[end_y]->findCharacterPos(end_x).y;


        std::vector < sf::RectangleShape > rects;


        if (end_y == start_y && start_x != end_x) {
            sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(rect_end_x - rect_start_x + 1, font.getLineSpacing(characterSize)));
            rect.setPosition(rect_start_x, rect_start_y);
            rect.setFillColor(selection_color);
            rects.push_back(rect);
        }
        else if (end_y - start_y >= 1) {

            sf::RectangleShape top_rect = sf::RectangleShape(sf::RectangleShape(sf::Vector2f(window->getSize().x - rect_start_x, font.getLineSpacing(characterSize))));
            top_rect.setPosition(rect_start_x, rect_start_y);
            top_rect.setFillColor(selection_color);
            rects.push_back(top_rect);

            sf::RectangleShape center_rect = sf::RectangleShape(sf::RectangleShape(sf::Vector2f(window->getSize().x, (end_y - start_y - 1) * font.getLineSpacing(characterSize))));
            center_rect.setPosition(0, rect_start_y + font.getLineSpacing(characterSize));
            center_rect.setFillColor(selection_color);
            rects.push_back(center_rect);

            sf::RectangleShape bottom_rect = sf::RectangleShape(sf::RectangleShape(sf::Vector2f(rect_end_x, font.getLineSpacing(characterSize))));
            bottom_rect.setPosition(0, rect_end_y);
            bottom_rect.setFillColor(selection_color);
            rects.push_back(bottom_rect);
        }


        for (auto& rect : rects)
            window->draw(rect);
    }
};

Selection* selection = nullptr;

void setCursorPosition(sf::Vector2i cursor_position) {
    cursorPosition = cursor_position;

    if (cursor_position == sf::Vector2i(0, 0)) {
        cursor.setPosition(sf::Vector2f(0, 0));
        return;
    }

    if (lines.empty()) {
        cursor.setPosition(sf::Vector2f(0, 0));
        return;
    }


    std::wstring line = (cursorPosition.y < lines.size()) ? lines[cursorPosition.y]->getString().toWideString() : L"";
    
    if (line.size() == 0) {
        sf::Vector2f charPos(0, (cursorPosition.y - scrollbar->scroll_value) * font.getLineSpacing(characterSize));
        cursor.setPosition(charPos);
        return;
    }
    
    if (cursor_position.x <= line.size()) {
        sf::Vector2f charPos = lines[cursorPosition.y]->findCharacterPos(cursor_position.x);
        //std::cout << charPos.x << ", " << charPos.y << "\n";
        cursor.setPosition(charPos);
        return;
    }

    sf::Vector2f endPos = lines[cursorPosition.y]->findCharacterPos(line.size());
    cursor.setPosition(endPos);

}

void setCursorLeft() {
    //std::cout << "Left" << "\n";
    if (cursorPosition.x > 0) {
        cursorPosition.x -= 1;
    }
    else {
        if (!lines.empty() && cursorPosition.y > 0) {
            cursorPosition.y -= 1;
            int last_character_index = (lines[cursorPosition.y]->getString().toWideString().back() == '\n') ? lines[cursorPosition.y]->getString().toWideString().size() - 1 : lines[cursorPosition.y]->getString().toWideString().size();
            cursorPosition.x = last_character_index;

        }
    }

    setCursorPosition(cursorPosition);
}

void setCursorRight() {
    //std::cout << "Right" << "\n";

    if (lines.empty()) {
        return;
    }

    int last_character_index = (lines[cursorPosition.y]->getString().toWideString().back() == '\n') ? lines[cursorPosition.y]->getString().toWideString().size() - 1 : lines[cursorPosition.y]->getString().toWideString().size();

    if (cursorPosition.x < last_character_index) { //////// +1
        cursorPosition.x += 1;
    }
    else {
        if (cursorPosition.y < lines.size() - 1) {
            cursorPosition.x = 0;
            cursorPosition.y += 1;
        }
    }

    setCursorPosition(cursorPosition);
}

void setCursorUp() {
    //std::cout << "Up" << "\n";
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
    //std::cout << "Down << "\n";
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

void setCursorLeftWithShift() {

    //std::cout << "shift + Left" << "\n";

    if (lines.empty()) {
        return;
    }
        
    if (cursorPosition.x > 0 || cursorPosition.y > 0) {
        int index = getCursorIndex(cursorPosition) - 1;

        if (selection == nullptr) {
            selection = new Selection();
            selection->start_index = index + 1;
            selection->end_index = index;

        }
        else {
            selection->end_index = index;

            if (selection->end_index == selection->start_index) {
                delete selection;
                selection = nullptr;
            }
        }

        setCursorPosition(getCursorFromIndex(index));
    }
    
   
}

void setCursorRightWithShift() {
    //std::cout << "shift + Right" << "\n";

    if (lines.empty()) {
        return;
    }

    int index = getCursorIndex(cursorPosition) + 1;

    if (selection == nullptr) {
        selection = new Selection();

        selection->start_index = index - 1;
        selection->end_index = index;

    }
    else {
        selection->end_index = index;

        if (selection->end_index == selection->start_index) {
            delete selection;
            selection = nullptr;
        }
    }

    setCursorPosition(getCursorFromIndex(index));
}

void setCursorUpWithShift() {
    //std::cout << "shift + Up" << "\n";
    if (lines.empty()) {
        return;
    }

    if (selection == nullptr) {

        int prev_index = getCursorIndex(cursorPosition);
        setCursorUp();
        int index = getCursorIndex(cursorPosition);

        selection = new Selection();

        selection->start_index = prev_index;
        selection->end_index = index;

        setCursorPosition(getCursorFromIndex(index));

    }
    else {
        setCursorUp();
        int index = getCursorIndex(cursorPosition);
        selection->end_index = index;

        if (selection->end_index == selection->start_index) {
            delete selection;
            selection = nullptr;
        }

        setCursorPosition(getCursorFromIndex(index));
    }
}

void setCursorDownWithShift() {
    //std::cout << "shift + Down" << "\n";
    if (lines.empty()) {
        return;
    }

    if (selection == nullptr) {

        int prev_index = getCursorIndex(cursorPosition);
        setCursorDown();
        int index = getCursorIndex(cursorPosition);

        selection = new Selection();

        selection->start_index = prev_index;
        selection->end_index = index;

        setCursorPosition(getCursorFromIndex(index));

    }
    else {
        setCursorDown();
        int index = getCursorIndex(cursorPosition);
        selection->end_index = index;

        if (selection->end_index == selection->start_index) {
            delete selection;
            selection = nullptr;
        }

        setCursorPosition(getCursorFromIndex(index));
    }
}

void generateScrollbar() {

    float scrollbar_value;

    //std::cout << "cur: " << cursorPosition.y << "\n";

    if (scrollbar != nullptr) {

        float min_val_on_screen = scrollbar->scroll_value;
        int lines_on_screen = int(window->getSize().y / font.getLineSpacing(characterSize));
        float max_val_on_screen = min_val_on_screen + lines_on_screen;

        if (cursorPosition.y < min_val_on_screen)
            scrollbar_value = cursorPosition.y;
        else if (cursorPosition.y >= max_val_on_screen - 1)
            scrollbar_value = cursorPosition.y - int(window->getSize().y / font.getLineSpacing(characterSize) - 1);
        else
            scrollbar_value = scrollbar->scroll_value;
    }
    else
        scrollbar_value = 0;

    sf::Vector2f scrollbar_size(16, window->getSize().y);
    sf::Vector2f scrollbar_position(window->getSize().x-16, 0);

    float scrollbar_len = window->getSize().y/font.getLineSpacing(characterSize);
    float max_value = (lines.empty())? 0 : lines.size();

    scrollbar = new Scrollbar(scrollbar_size, scrollbar_position, 0, max_value, scrollbar_value, scrollbar_len);
    scrollbar->onclick_func = []() {

        linesPositioning();
        setCursorPosition(cursorPosition);
    };
}

int main()
{
    
    

    font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    characterSize = 17;
    
    text = L"";

    

    lines = wrap_text(window->getSize().x-16, font, characterSize, text);
    generateScrollbar();

    cursor = sf::RectangleShape(sf::Vector2f(2, characterSize));
    cursor.setFillColor(sf::Color::Red);


    sf::Clock clock;

    SFML_intro* intro = new SFML_intro(window);
    delete intro;

    selection = nullptr;

    while (window->isOpen())
    {

        mousePosition = sf::Mouse::getPosition(*window); // get the mouse position about window
        worldMousePosition = window->mapPixelToCoords(mousePosition); // get global mouse position

        currentTime = timeClock.getElapsedTime();

        scrollbar->update();

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
            
            scrollbar->handleEvent(event);
            
            if (elementGUIClicked != nullptr) {
                mousePress = false;

                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    elementGUIClicked = nullptr;
                }
            }
            else if(elementGUIClicked == nullptr) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                    mousePress = true;

                    sf::Vector2i cur_pos = getCursorPosition();

                    if (!lines.empty() && cur_pos.y < lines.size()) {
                        if (selection != nullptr)
                            delete selection;

                        selection = new Selection();
                        selection->start_index = getCursorIndex(cur_pos);
                        selection->end_index = selection->start_index;
                        setCursorPosition(cur_pos);
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i cur_pos = getCursorPosition();
                    setCursorPosition(cur_pos);
                    mousePress = false;
                }
                else if (event.type == sf::Event::MouseMoved) {
                    if (mousePress) {
                        sf::Vector2i cur_pos = getCursorPosition();

                        if (selection != nullptr)
                            selection->end_index = getCursorIndex(cur_pos);

                        setCursorPosition(cur_pos);
                    }
                }
            }
            
            if (event.type == sf::Event::KeyPressed && event.key.control & event.key.code == sf::Keyboard::A) {
                if (selection != nullptr)
                    delete selection;

                selection = new Selection();
                selection->start_index = 0;
                selection->end_index = text.size();

                cursorPosition = getCursorFromIndex(selection->end_index);

                generateScrollbar();
                linesPositioning();
                setCursorPosition(cursorPosition);

            }
            else if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::V) {
                // Ctrl + V 

                int index = getCursorIndex(cursorPosition);

                if (selection != nullptr && selection->start_index != selection->end_index) {
                    text.erase(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                    int index = std::min(selection->start_index, selection->end_index);
                    delete selection;
                    selection = nullptr;
                }

                
                
                std::wstring clip_text = sf::Clipboard::getString().toWideString();
                while (clip_text.find(L"\r\n") != std::wstring::npos) {
                    clip_text.erase(clip_text.find(L"\r\n"), 1);
                };

                text.insert(index, clip_text);
                index += clip_text.size();

                for (auto& line : lines)
                    delete line;

                lines = wrap_text(window->getSize().x - 16, font, characterSize, text);
                cursorPosition = getCursorFromIndex(index);

                generateScrollbar();
                linesPositioning();
                setCursorPosition(cursorPosition);
                
            }
            else if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::C) {
                // Ctrl + C
                if (selection && selection->start_index != selection->end_index) {
                    
                    std::wstring clip_text = text.substr(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                    sf::Clipboard::setString(clip_text);
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.control && event.key.code == sf::Keyboard::X) {
                // Ctrl + X
                if (selection!=nullptr && selection->start_index != selection->end_index) {

                    std::wstring clip_text = text.substr(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                    sf::Clipboard::setString(clip_text);

                    text.erase(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                    int index = std::min(selection->start_index, selection->end_index);
                    delete selection;
                    selection = nullptr;
                    
                    generateScrollbar();
                    linesPositioning();
                    setCursorPosition(getCursorFromIndex(index));

                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.shift && event.key.code == sf::Keyboard::Left) {
                generateScrollbar();
                linesPositioning();
                setCursorLeftWithShift();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.shift && event.key.code == sf::Keyboard::Right) {

                generateScrollbar();
                linesPositioning();
                setCursorRightWithShift();

            }
            else if (event.type == sf::Event::KeyPressed && event.key.shift && event.key.code == sf::Keyboard::Up) {
                
                generateScrollbar();
                linesPositioning();
                setCursorUpWithShift();


                
            }
            else if (event.type == sf::Event::KeyPressed && event.key.shift && event.key.code == sf::Keyboard::Down) {
                generateScrollbar();
                linesPositioning();
                setCursorDownWithShift();

            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                if (!text.empty()) {

                    if (selection == nullptr || (selection != nullptr && selection->start_index == selection->end_index)) {
                        int index = getCursorIndex(cursorPosition);
                        text.erase(index, 1);
                    }
                    else {
                        text.erase(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                        int index = std::min(selection->start_index, selection->end_index);
                        delete selection;
                        selection = nullptr;
                        setCursorPosition(getCursorFromIndex(index));
                    }

                    for (auto& line : lines)
                        delete line;

                    lines = wrap_text(window->getSize().x - 16, font, characterSize, text);
                    generateScrollbar();
                    linesPositioning();

                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left) {
                
                setCursorLeft();
                generateScrollbar();
                linesPositioning();
                
                if (selection != nullptr) {
                    delete selection;
                    selection = nullptr;
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right) {
                
                setCursorRight();
                generateScrollbar();
                linesPositioning();
                
                if (selection != nullptr) {
                    delete selection;
                    selection = nullptr;
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up) {
                
                setCursorUp();
                generateScrollbar();
                linesPositioning();
                
                if (selection != nullptr) {
                    delete selection;
                    selection = nullptr;
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down) {
                
                setCursorDown();
                generateScrollbar();
                linesPositioning();
                
                if (selection != nullptr) {
                    delete selection;
                    selection = nullptr;
                }
            }
            else if (event.type == sf::Event::TextEntered) {
                
                int index = getCursorIndex(cursorPosition);

                if (event.text.unicode == 8) {
                    // backspace
                    if (index > 0 && !text.empty()) {
                        if (selection == nullptr || selection->start_index == selection->end_index){
                            text.erase(index - 1, 1);
                            index -= 1;
                        }
                        else {
                            text.erase(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                            index = std::min(selection->start_index, selection->end_index);
                        }

                        if (selection != nullptr) {
                            delete selection;
                            selection = nullptr;
                        }
                    }

                    if (!lines.empty()) {
                        for (auto& t : lines)
                            delete t;
                    }
                    lines.clear();

                    lines = wrap_text(window->getSize().x - 16, font, characterSize, text);

                    // Po każdej zmianie kursor wstawiamy po index
                    cursorPosition = getCursorFromIndex(index);

                    generateScrollbar();
                    linesPositioning();
                    setCursorPosition(cursorPosition);
                }
                else if (event.text.unicode == 13) {
                    // enter
                    text.insert(index, 1, L'\n');
                    cursorPosition.x = 0;
                    cursorPosition.y += 1;

                    if (!lines.empty()) {
                        for (auto& t : lines)
                            delete t;
                    }
                    lines.clear();

                    lines = wrap_text(window->getSize().x - 16, font, characterSize, text);

                    generateScrollbar();
                    linesPositioning();
                    setCursorPosition(cursorPosition);

                }
                else if (event.text.unicode == 9) {
                    // tab
                    text.insert(index, 1, L'\t');
                    index += 1;

                    if (!lines.empty()) {
                        for (auto& t : lines)
                            delete t;
                    }
                    lines.clear();

                    lines = wrap_text(window->getSize().x - 16, font, characterSize, text);

                    // Po każdej zmianie kursor wstawiamy po index
                    cursorPosition = getCursorFromIndex(index);

                    generateScrollbar();
                    linesPositioning();
                    setCursorPosition(cursorPosition);

                }
                else {
                    // other character
                    wchar_t character = static_cast<wchar_t>(event.text.unicode);
                    if (character >= 32 && character != 127) {

                        if (selection != nullptr && selection->start_index != selection->end_index) {

                            text.erase(std::min(selection->start_index, selection->end_index), std::abs(selection->end_index - selection->start_index));
                            index = std::min(selection->start_index, selection->end_index);
                            delete selection;
                            selection = nullptr;
                        }
                        

                        text.insert(index, 1, character);
                        index += 1;

                        if (!lines.empty()) {
                            for (auto& t : lines)
                                delete t;
                        }
                        lines.clear();

                        lines = wrap_text(window->getSize().x - 16, font, characterSize, text);

                        // Po każdej zmianie kursor wstawiamy po index
                        cursorPosition = getCursorFromIndex(index);

                        generateScrollbar();
                        linesPositioning();
                        setCursorPosition(cursorPosition);
                    }
                }

                


            }


        }

        window->clear(background_color);

        if (selection != nullptr) {
            selection->draw();
        }

        for (auto& line : lines)
            window->draw(*line);

        if (std::fmod(currentTime.asSeconds(), 0.6f) < 0.3f)
            window->draw(cursor);

        scrollbar->draw();

        window->display();
    }
}