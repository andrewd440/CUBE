#pragma once

#include <vector>
#include <utility>

template <typename... Args>
class TEvent
{
public:
	using EventType = void(*)(Args...);

private:
	using ListenerPair = std::pair<void*, void(*)(void*, Args...)>;

	template <typename T, void (T::*Function)(Args...)>
	static void ListenerStub(void* Instance, Args... Argument)
	{
		(static_cast<T*>(Instance)->*Function)(Argument...);
	}

public:
	
	template <typename T, void (T::*Function)(Args...)>
	void AddListener(T* Instance)
	{
		ListenerPair Pair{ Instance, &ListenerStub<T, Function> };
		mListeners.push_back(Pair);
	}

	template <typename T>
	void RemoveListener(T* Instance)
	{
		auto Found = std::find_if(mListeners.begin(), mListeners.end(), [&Instance](ListenerPair& Pair){ return Pair.first == Instance; });
		if (Found != mListeners.end())
		{
			mListeners.erase(Found);
		}
	}

	void Invoke(Args... Argument)
	{
		for (auto& Listener : mListeners)
			Listener.second(Listener.first, Argument...);
	}

private:
	std::vector<ListenerPair> mListeners;
};