#include "..\..\Include\Input\TextEntered.h"

std::wstring STextEntered::mTextInput;

void STextEntered::AddTextEvent(const sf::Event& Event)
{
	mTextInput.push_back(static_cast<wchar_t>(Event.text.unicode));
}

const std::wstring& STextEntered::GetTextInput()
{
	return mTextInput;
}

void STextEntered::Reset()
{
	mTextInput.clear();
}