#pragma once

#include <vector>
#include <utility>

template <typename Arg>
class TEvent
{
public:
	using ArgumentType = Arg;
	using EventType = void(*)(Arg);

private:
	using ListenerPair = std::pair<void*, void(*)(void*, Arg)>;

	template <typename T, void (T::*Function)(Arg)>
	static void ListenerStub(void* Instance, Arg Argument)
	{
		(static_cast<T*>(Instance)->*Function)(Argument);
	}

public:
	
	template <typename T, void (T::*Function)(Arg)>
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

	void Invoke(Arg Argument)
	{
		for (auto& Listener : mListeners)
			Listener.second(Listener.first, Argument);
	}

private:
	std::vector<ListenerPair> mListeners;
};