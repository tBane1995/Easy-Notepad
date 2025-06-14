#ifndef elementGUI_hpp
#define elementGUI_hpp

class ElementGUI {
public:

	ElementGUI() { }
	~ElementGUI() { }

	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

ElementGUI* elementGUIClicked = nullptr;

#endif