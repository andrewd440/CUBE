#include "..\..\Include\Input\ButtonEvent.h"
#include "..\..\Include\Misc\Assertions.h"

std::bitset<sf::Keyboard::KeyCount> SButtonEvent::mKeyUps;
std::bitset<sf::Keyboard::KeyCount> SButtonEvent::mKeyDowns;

std::bitset<sf::Mouse::ButtonCount> SButtonEvent::mMouseUps;
std::bitset<sf::Mouse::ButtonCount> SButtonEvent::mMouseDowns;

void SButtonEvent::ResetButtonEvents()
{
	mKeyDowns.reset();
	mKeyUps.reset();
	mMouseDowns.reset();
	mMouseUps.reset();
}

bool SButtonEvent::AddButtonEvent(const sf::Event& ButtonEvent)
{
	// Ensure that only button events are passed to this function.
	ASSERT(IsButtonEvent(ButtonEvent));

	if (ButtonEvent.key.code != sf::Keyboard::Unknown)
	{
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
	}

	return false;
}

bool SButtonEvent::GetKeyUp(const sf::Keyboard::Key Key)
{
	return mKeyUps[Key];
}

std::bitset<sf::Keyboard::KeyCount> SButtonEvent::GetAllKeyUps()
{
	return mKeyUps;
}

bool SButtonEvent::GetKeyDown(const sf::Keyboard::Key Key)
{
	return mKeyDowns[Key];
}

std::bitset<sf::Keyboard::KeyCount> SButtonEvent::GetAllKeyDowns()
{
	return mKeyDowns;
}

bool SButtonEvent::GetMouseUp(const sf::Mouse::Button Button)
{
	return mMouseUps[Button];
}

bool SButtonEvent::GetMouseDown(const sf::Mouse::Button Button)
{
	return mMouseDowns[Button];
}

bool SButtonEvent::IsButtonEvent(const sf::Event& Event)
{
	return Event.type == sf::Event::KeyPressed || Event.type == sf::Event::KeyReleased
		|| Event.type == sf::Event::MouseButtonPressed || Event.type == sf::Event::MouseButtonReleased;
}