#pragma once

class RegisterService {
  public:
	static RegisterService &getInstance()
	{
		static RegisterService instance = RegisterService();
		return instance;
	}
	void registerAudio() const;
	void registerComponents() const;
};