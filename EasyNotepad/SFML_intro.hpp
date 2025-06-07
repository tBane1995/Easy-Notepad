#ifndef SFML_intro_hpp
#define SFML_intro_hpp

enum class intro_states { start, logo_entry_anim, logo_hidding, end };

class SFML_intro {

public:
	intro_states state;

	sf::Texture logo_texture;
	sf::Sprite logo_sprite;
	
	sf::Font font;
	sf::Text SFML;

	sf::Time start_time;

	SFML_intro(sf::RenderWindow* window) {

		logo_texture = sf::Texture();
		logo_texture.loadFromFile("sfml-icon-big.png");

		logo_sprite = sf::Sprite();
		logo_sprite.setTexture(logo_texture);
		logo_sprite.setOrigin(logo_texture.getSize().x / 2, logo_texture.getSize().y / 2);

		sf::Vector2f screen_size = sf::Vector2f(window->getSize().x, window->getSize().y);
		float logo_scale;
		if (window->getSize().x < window->getSize().y) {
			// Skaluj względem szerokości okna
			logo_scale = (window->getSize().x * 0.4f) / float(logo_texture.getSize().x);
		}
		else {
			// Skaluj względem wysokości okna
			logo_scale = (window->getSize().y * 0.4f) / float(logo_texture.getSize().y);
		}
		logo_sprite.setScale(logo_scale, logo_scale);
		
		font = sf::Font();
		font.loadFromFile("arial.ttf");
		SFML = sf::Text(L"SFML", font, 30);
		SFML.setFillColor(sf::Color::White);
		play(window);
	}

	~SFML_intro() { }

	void play(sf::RenderWindow* window) {
		state = intro_states::start;
		sf::Clock clock;
		start_time = clock.getElapsedTime();

		float hgh = logo_sprite.getGlobalBounds().height + SFML.getGlobalBounds().height;
		logo_sprite.setPosition(sf::Vector2f(window->getSize().x / 2 , window->getSize().y / 2 - hgh/2.0f));
		SFML.setPosition(sf::Vector2f(window->getSize().x / 2, window->getSize().y/2 + hgh / 2.0f));
		window->clear(sf::Color::Black);
		window->display();

		state = intro_states::logo_entry_anim;

		while (state != intro_states::end) {

			if (state == intro_states::logo_entry_anim) {
				float col_alpha = (clock.getElapsedTime() - start_time).asSeconds() * 50000.0f / 255.0f;
				logo_sprite.setColor(sf::Color(255.0f, 255.0f, 255.0f, col_alpha));
				window->clear(sf::Color::Black);
				window->draw(logo_sprite);
				window->draw(SFML);
				window->display();

				if (col_alpha > 255) {
					state = intro_states::logo_hidding;
					start_time = clock.getElapsedTime();
					
				}

			}
			else if (state == intro_states::logo_hidding) {
				float col_alpha = 255 - (clock.getElapsedTime() - start_time).asSeconds() * 100000.0f / 255.0f;
				logo_sprite.setColor(sf::Color(255.0f, 255.0f, 255.0f, col_alpha));
				window->clear(sf::Color::Black);
				window->draw(logo_sprite);
				window->draw(SFML);
				window->display();

				if (col_alpha <= 0) {
					state = intro_states::end;
				}
			}
		}
	}

};
#endif // !SFML_intro_hpp
