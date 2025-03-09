#include "application.h"
#include "hyperflow.h"
#include <sstream>

namespace app
{
	void Application::Start()
	{
	
	}

	void Application::Update()
	{
		auto window = hf::Hyperflow::MainWindow();
		auto position = window->mouse.GetPosition();
		std::ostringstream oss;
		oss << "X: " << position.x << " Y: " << position.y;
		hf::Hyperflow::MainWindow()->SetTitle(oss.str().c_str());
	}

	void Application::Render()
	{

	}

	void Application::Quit()
	{
	
	}
}