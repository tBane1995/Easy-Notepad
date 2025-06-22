#ifndef text_wrapper_hpp
#define text_wrapper_hpp

std::vector < sf::Text* > wrap_text(int line_width, sf::Font& font, float characterSize, std::wstring text) {

    std::vector < sf::Text* > wrapped_text;

    std::wstring line = L"";
    std::wstring word = L"";

    for (auto& character : text) {
        if (sf::Text(word + character, font, characterSize).getGlobalBounds().width > line_width) {

            if (line != L"") {
                sf::Text* t = new sf::Text(line, font, characterSize);
                wrapped_text.push_back(t);
                line = L"";
            }

            // word longer than line
            std::wstring l = L"";
            word = word + character;
            for (wchar_t& c : word) {
                if (sf::Text(l + c, font, characterSize).getGlobalBounds().width > line_width) {
                    sf::Text* t = new sf::Text(l, font, characterSize);
                    wrapped_text.push_back(t);
                    l = c;
                }
                else
                    l = l + c;
            }

            sf::Text* t = new sf::Text(l, font, characterSize);
            wrapped_text.push_back(t);

            word = L"";
        }
        else if (sf::Text(line + word + character, font, characterSize).getGlobalBounds().width > line_width)
        {
            sf::Text* t = new sf::Text(line, font, characterSize);
            wrapped_text.push_back(t);

            line = L"";
            word = word + character;
        }
        else if (character == L'\n') {

            if (sf::Text(line + word, font, characterSize).getGlobalBounds().width > line_width) {
                sf::Text* t = new sf::Text(line, font, characterSize);
                wrapped_text.push_back(t);

                sf::Text* t2 = new sf::Text(word + L"\n", font, characterSize);
                wrapped_text.push_back(t2);

                line = L"";
                word = L"";
            }
            else {
                sf::Text* t = new sf::Text(line + word + L"\n", font, characterSize);
                wrapped_text.push_back(t);

                line = L"";
                word = L"";
            }

        }
        else if (character == L' ' || character == L'\t') {
            if (sf::Text(line + word, font, characterSize).getGlobalBounds().width > line_width) {
                sf::Text* t = new sf::Text(line + L"\n", font, characterSize);
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
        wrapped_text.push_back(t);
    }

    return wrapped_text;

}

#endif