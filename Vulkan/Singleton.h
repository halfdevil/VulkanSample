#pragma once

template <class T> class Singleton
{
private:

	static T *msInstance;

public:

	Singleton()
	{
		msInstance = static_cast<T*>(this);
	}

	static T* GetPtr() 
	{
		return msInstance;
	}

	static T& Get()
	{
		return *msInstance;
	}
};

template <class T> T* Singleton<T>::msInstance = nullptr;