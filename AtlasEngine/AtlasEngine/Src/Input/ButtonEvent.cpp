#include "..\..\Include\Input\ButtonEvent.h"
#include "..\..\Include\Misc\Assertions.h"

std::bitset<sf::Keyboard::KeyCount> FButtonEvent::mKeyUps;
std::bitset<sf::Keyboard::KeyCount> FButtonEvent::mKeyDowns;

std::bitset<sf::Mouse::ButtonCount> FButtonEvent::mMouseUps;
std::bitset<sf::Mouse::ButtonCount> FButtonEvent::mMouseDowns;

void FButtonEvent::ResetButtonEvents()
{
	mKeyDowns.reset();
	mKeyUps.reset();
	mMouseDowns.reset();
	mMouseUps.reset();
}

bool FButtonEvent::AddButtonEvent(const sf::Event& ButtonEvent)
{
	// Ensure that only button events are passed to this function.
	ASSERT(IsButtonEvent(ButtonEvent));

	switch (ButtonEvent.type)
	{
	case sf::Event::KeyPressed:
		mKeyDowns[ButtonEvent.key.code] = true;
		return true;
		break;
	case sf::Event::KeyReleased:
		mKeyUps[ButtonEvent.key.code] = true;
		return true;
		break;
	case sf::Event::MouseButtonPressed:
		mMouseDowns[ButtonEvent.key.code] = true;
		return true;
		break;
	case sf::Event::MouseButtonReleased:
		mMouseUps[ButtonEvent.key.code] = true;
		return true;
		break;
	}

	return false;
}

bool FButtonEvent::GetKeyUp(const sf::Keyboard::Key Key)
{
	return mKeyUps[Key];
}

bool FButtonEvent::GetKeyDown(const sf::Keyboard::Key Key)
{
	return mKeyDowns[Key];
}

bool FButtonEvent::GetMouseUp(const sf::Mouse::Button Button)
{
	return mMouseUps[Button];
}

bool FButtonEvent::GetMouseDown(const sf::Mouse::Button Button)
{
	return mMouseDowns[Button];
}

bool FButtonEvent::IsButtonEvent(const sf::Event& Event)
{
	return Event.type == sf::Event::KeyPressed || Event.type == sf::Event::KeyReleased
		|| Event.type == sf::Event::MouseButtonPressed || Event.type == sf::Event::MouseButtonReleased;
}