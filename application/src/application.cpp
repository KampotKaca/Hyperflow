#include "application.h"
#include "hyperflow.h"

namespace app
{
	static void Log(void* params);
	static void UpLog(void* params);

	hf::KeySubscriptionData LOG_SUBSCRIPTION =
	{
		.callback = Log,
		.state = hf::KeyState::Down,
		.keyCode = hf::KEY_BUTTON_LEFT,
	};

	hf::KeySubscriptionData UP_SUBSCRIPTION =
	{
		.callback = UpLog,
		.state = hf::KeyState::Up,
		.keyCode = hf::KEY_BUTTON_LEFT,
	};

	void Application::Start()
	{
		hf::SubscribeOnKey(&LOG_SUBSCRIPTION);
		hf::SubscribeOnKey(&UP_SUBSCRIPTION);
	}

	void Application::Update()
	{

	}

	void Application::Render()
	{

	}

	void Application::Quit()
	{
		hf::UnsubscribeOnKey(&LOG_SUBSCRIPTION);
		hf::UnsubscribeOnKey(&UP_SUBSCRIPTION);
	}

	static void Log(void* params)
	{
		log_error("LOGGGGGG");
	}

	static void UpLog(void* params)
	{
		log_error("LOGGGGGG UPPPPPPP");
	}
}