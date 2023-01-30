#pragma once

class Sound sealed
{
public:
	Sound();
	~Sound();

	static Sound& Get();

private:
	static Sound* s_singleton;
};

