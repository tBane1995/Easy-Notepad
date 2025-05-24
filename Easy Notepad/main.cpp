#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
sf::RenderWindow* window;

sf::Font font;
short characterSize;

std::wstring text;
std::vector < sf::Text* > texts;

sf::Vector2i mousePosition;
sf::Vector2f worldMousePosition;

sf::RectangleShape cursor;
sf::Vector2i cursorPosition = sf::Vector2i(0, 0);

sf::Clock timeClock;
sf::Time currentTime;


int selecting_start = 2;    // selecting start cursor
int selecting_end = 4;      // selecting end cursor

std::vector<sf::Text*> wrapText(int line_length = -1) {
    std::vector<sf::Text*> t;
    std::wstring line = L"";
    std::wstring word = L"";
    wchar_t white_char = '\0';

    for (auto& wchar : text) {
        if (wchar == '\n') {
            line = line + word + white_char;
            sf::Text* new_text = new sf::Text(line, font, characterSize);
            new_text->setFillColor(sf::Color::White);
            if (!t.empty())
                new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
            else
                new_text->setPosition(0, 0);
            t.push_back(new_text);
            line = L"";
            word = L"";
            continue;
        }



        if (wchar == L' ' || wchar == L'\t') {
            white_char = wchar;
            word = word + white_char;

            if (line_length > -1) {

                if (line == L"") {
                    sf::Text test_word(word, font, characterSize);

                    if (test_word.getGlobalBounds().width >= line_length) {

                        std::wstring part_of_word = L"";
                        wchar_t character = '\0';

                        for (wchar_t& ch : word) {
                            character = ch;
                            sf::Text w(part_of_word + character, font, characterSize);
                            if (w.getGlobalBounds().width >= line_length) {
                                sf::Text* new_text = new sf::Text(part_of_word, font, characterSize);
                                new_text->setFillColor(sf::Color::White);
                                if (!t.empty())
                                    new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
                                else
                                    new_text->setPosition(0, 0);
                                t.push_back(new_text);
                                part_of_word = character;
                            }
                            else
                                part_of_word = part_of_word + character;
                        }

                        if (part_of_word != L"") {
                            sf::Text* new_text = new sf::Text(part_of_word, font, characterSize);
                            new_text->setFillColor(sf::Color::White);
                            if (!t.empty())
                                new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
                            else
                                new_text->setPosition(0, 0);
                            t.push_back(new_text);
                            part_of_word = L"";
                        }

                        word = L"";
                        continue;
                    }
                }

                sf::Text test_text(line + white_char + word, font, characterSize);
                if (test_text.getGlobalBounds().width >= line_length) {
                    sf::Text* new_text = new sf::Text(line, font, characterSize);
                    new_text->setFillColor(sf::Color::White);
                    if (!t.empty())
                        new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
                    else
                        new_text->setPosition(0, 0);
                    t.push_back(new_text);
                    line = word;
                    word = L"";
                    continue;

                }
                else
                    line = line + word;

            }


            word = L"";
            white_char = '\0';
        }
        else if(wchar != '\0') {
            word += wchar;
        }
    }

    // dodaj ostatnią linię
    sf::Text test_word(word, font, characterSize);

    if (test_word.getGlobalBounds().width >= line_length) {

        std::wstring part_of_word = L"";
        wchar_t character = '\0';

        for (wchar_t& ch : word) {
            character = ch;
            sf::Text w(part_of_word + character, font, characterSize);
            if (w.getGlobalBounds().width >= line_length) {
                sf::Text* new_text = new sf::Text(part_of_word, font, characterSize);
                new_text->setFillColor(sf::Color::White);
                if (!t.empty())
                    new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
                else
                    new_text->setPosition(0, 0);
                t.push_back(new_text);
                part_of_word = character;
            }
            else
                part_of_word = part_of_word + character;
        }

        if (part_of_word != L"") {
            sf::Text* new_text = new sf::Text(part_of_word, font, characterSize);
            new_text->setFillColor(sf::Color::White);
            if (!t.empty())
                new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
            else
                new_text->setPosition(0, 0);
              t.push_back(new_text);
                part_of_word = L"";
            }

            word = L"";
        }

    if (!word.empty() || !line.empty()) {

        sf::Text test_text(line + word, font, characterSize);
        if (line_length > -1 && test_text.getGlobalBounds().width >= line_length && !line.empty()) {
            // dodaj najpierw obecną linię
            sf::Text* new_text = new sf::Text(line, font, characterSize);
            new_text->setFillColor(sf::Color::White);
            new_text->setPosition(0, t.empty() ? 0 : t.back()->getPosition().y + font.getLineSpacing(characterSize));
            t.push_back(new_text);

            // a potem słowo w nowej linii
            new_text = new sf::Text(word, font, characterSize);
            new_text->setFillColor(sf::Color::White);
            new_text->setPosition(0, t.back()->getPosition().y + font.getLineSpacing(characterSize));
            t.push_back(new_text);
        }
        else {
            // zmieściło się wszystko — dodaj jako jedna linia
            sf::Text* new_text = new sf::Text(line + word, font, characterSize);
            new_text->setFillColor(sf::Color::White);
            new_text->setPosition(0, t.empty() ? 0 : t.back()->getPosition().y + font.getLineSpacing(characterSize));
            t.push_back(new_text);
        }
    }

    return t;
}

sf::Vector2i getCursorPosition() {

    sf::Vector2i cur_pos = sf::Vector2i(0, 0);

    for (int t = 0; t < texts.size();t++) {

        for (size_t i = 0; i < texts[t]->getString().getSize(); ++i) {

            sf::Vector2f charPos = texts[t]->findCharacterPos(i);
            float nextX = texts[t]->findCharacterPos(i + 1).x;

            sf::FloatRect charRect(charPos.x, charPos.y, nextX - charPos.x, font.getLineSpacing(characterSize));

            if (charRect.contains(worldMousePosition)) {

                if (worldMousePosition.x < charRect.left + charRect.width / 2)
                    return sf::Vector2i(i, t);
                else
                    return sf::Vector2i(i+1, t);
            }

            bool isLastChar = (i == texts[t]->getString().getSize() - 1);
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

void setCursorPosition(sf::Vector2i cursor_position) {
    cursorPosition = cursor_position;

    // Przechodzimy przez wszystkie linie tekstu
    for (int t = 0; t < texts.size(); t++) {

        if (t == cursor_position.y) {
            std::wstring line = texts[t]->getString().toWideString();

            if (cursor_position.x <= line.size()) {
                sf::Vector2f charPos = texts[t]->findCharacterPos(cursor_position.x);

                cursor.setPosition(charPos.x, charPos.y);
                return;
            }
        }
    }
}

int getCursorIndex(sf::Vector2i position) {
    int index = 0;
    for (int i = 0; i < position.y && i < texts.size(); ++i) {
        index += texts[i]->getString().getSize();
    }
    index += position.x;
    return index;
}

sf::Vector2i getCursorFromIndex(int index) {
    int current = 0;
    for (int y = 0; y < texts.size(); ++y) {
        int lineSize = texts[y]->getString().getSize();
        if (index <= current + lineSize) {
            return sf::Vector2i(index - current, y);
        }
        current += lineSize;
    }
    if (!texts.empty())
        return sf::Vector2i(texts.back()->getString().getSize(), texts.size() - 1);
    return sf::Vector2i(0, 0);
}

int main()
{

    sf::View view(sf::FloatRect(0, 0, 640, 640));
    window = new sf::RenderWindow(sf::VideoMode(view.getSize().x, view.getSize().y), "Easy Notepad!", sf::Style::Titlebar | sf::Style::Close);

    font.loadFromFile("arial.ttf");
    characterSize = 17;
    text =
        L"Gracz najpierw zagaduje handlarza gdyż ten jest najbliżej. Handlarz oferuje skórzane ubranie w zamian za dostarczenie kilku skór od myśliwego, " 
        L"którego gracz mijał wcześniej. Zielarka da graczowi trochę złota w zamian za przyniesienie kilku roślin leczniczych. "
        L"U kowala gracz może zakupić oręż - zwyczajny prosty miecz gdyż jest to niewprawiony kowal w miecznictwie. "
        L"Zaś do wieży mędrca nie da się dostać.Gracz rusza spowrotem do myśliwego po skóry, lecz ten jest nieufny, "
        L"ale ostatecznie zgadza się i daje graczowi skóry.";


    texts = wrapText(window->getSize().x);

    cursor = sf::RectangleShape(sf::Vector2f(2, characterSize));
    cursor.setFillColor(sf::Color::Red);

    sf::Clock clock;

    while (window->isOpen())
    {

        mousePosition = sf::Mouse::getPosition(*window);	            // get the mouse position about window
        worldMousePosition = window->mapPixelToCoords(mousePosition);   // get global mouse position

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
            }
            else if (event.type == sf::Event::TextEntered) {

                if (event.text.unicode < 128) {

                    int index = 0;
                    for (int i = 0; i < cursorPosition.y; i++) {
                        index = index + texts[i]->getString().toWideString().size();
                    }
                    index = index + cursorPosition.x;

                    if (event.text.unicode == '\b') {
                        
                        if (index > 0 && !text.empty()) {
                            text.erase(index -1, 1);
                            index -= 1;
                        }
                    }
                    else if (event.text.unicode == 32) {
                        // space
                        text.insert(index, 1, char(' '));
                        index += 1;
                    }
                    else if (event.text.unicode == 13) {
                        // enter
                        text.insert(index, 1, char('\n'));
                        index += 1;
                    }
                    else {
                        // is character
                        text.insert(index, 1, wchar_t(event.text.unicode));
                        index += 1;
                    }

                    for (auto& t : texts)
                        delete t;
                    texts = wrapText(window->getSize().x);

                    cursorPosition = getCursorFromIndex(index);
                    setCursorPosition(cursorPosition);
                }

 
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    if (cursorPosition.x > 0) {
                        cursorPosition.x -= 1;
                    }
                    else {
                        if (cursorPosition.y > 0) {
                            cursorPosition.y -= 1;
                            cursorPosition.x = texts[cursorPosition.y]->getString().toWideString().size();
                        }
                    }

                }
                else if (event.key.code == sf::Keyboard::Right) {
                    if (cursorPosition.x < texts[cursorPosition.y]->getString().toWideString().size()) {
                        cursorPosition.x += 1;
                    }
                    else {
                        if (cursorPosition.y < texts.size() - 1) {
                            cursorPosition.x = 0;
                            cursorPosition.y += 1;
                        }
                            

                        
                    }
                        

                }
                else if (event.key.code == sf::Keyboard::Up) {
                    if(cursorPosition.y > 0)
                        cursorPosition.y -= 1;
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    if(cursorPosition.y < texts.size()-1)
                        cursorPosition.y += 1;
                }


                setCursorPosition(cursorPosition);

            }


        }

        window->clear(sf::Color(48, 48, 48, 255));
        for (auto& text : texts)
            window->draw(*text);
        if (std::fmod(currentTime.asSeconds(), 0.6f) < 0.3f)
            window->draw(cursor);
        window->display();
    }
}