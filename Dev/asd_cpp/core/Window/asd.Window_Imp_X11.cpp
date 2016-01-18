﻿
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "asd.Window_Imp_X11.h"
#include "../Log/asd.Log_Imp.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace asd {

void Window_Imp_X11::CallbackOnFocus(GLFWwindow* window, int b)
{
	auto w = (Window_Imp_X11*) glfwGetWindowUserPointer(window);
	if (b == GL_TRUE)
	{
		auto onFocused = w->OnFocused;

		if (onFocused)
		{
			onFocused();
		}
	}
	else
	{

	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Window_Imp_X11::Unregist()
{
	if (!m_closed)
	{
		glfwDestroyWindow(m_window);
		m_window = nullptr;
		m_closed = true;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Window_Imp* Window_Imp_X11::Create(int32_t width, int32_t height, const achar* title, Log* logger, bool isFullScreen)
{
	if (!glfwInit())
	{
		if (logger != nullptr) logger->WriteLine("ウインドウシステムの初期化に失敗");
		return nullptr;
	}

#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	auto titleUTF8 = ToUtf8String(title);

	GLFWmonitor* monitor = nullptr;
	if (isFullScreen)
	{
		monitor = glfwGetPrimaryMonitor();
	}

	auto window = glfwCreateWindow(width, height, titleUTF8.c_str(), monitor, NULL);
	if (window == nullptr)
	{
		if (logger != nullptr) logger->WriteLine("ウインドウの作成に失敗");
		glfwTerminate();
		return nullptr;
	}

	glfwSwapInterval(1);

	return new Window_Imp_X11(window, width, height, logger, isFullScreen);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Window_Imp_X11::Window_Imp_X11(GLFWwindow* window, int32_t width, int32_t height, Log* logger, bool isFullScreen)
	: Window_Imp(logger)
	, m_closed(false)
{
	m_window = window;
	m_size.X = width;
	m_size.Y = height;
	glfwSetWindowUserPointer(window, this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Window_Imp_X11::~Window_Imp_X11()
{
	Unregist();
	glfwTerminate();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool Window_Imp_X11::DoEvent()
{
	if (m_closed) return false;

	if (glfwWindowShouldClose(m_window))
	{
		Unregist();
		return false;
	}

	glfwPollEvents();
	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Window_Imp_X11::SetTitle( const achar* title )
{
	auto titleUTF8 = ToUtf8String(title);
	glfwSetWindowTitle(m_window, titleUTF8.c_str());
}

Vector2DI Window_Imp_X11::GetPrimaryMonitorPosition()
{
	auto monitor = glfwGetPrimaryMonitor();
	if (monitor == nullptr) return Vector2DI();

	int32_t x = 0;
	int32_t y = 0;

	glfwGetMonitorPos(monitor, &x, &y);

	return Vector2DI(x, y);
}

Vector2DI Window_Imp_X11::GetPrimaryMonitorSize()
{
	auto monitor = glfwGetPrimaryMonitor();
	if (monitor == nullptr) return Vector2DI();

	auto videomode = glfwGetVideoMode(monitor);
	if (videomode == nullptr) return Vector2DI();

	return Vector2DI(videomode->width, videomode->height);
}

Vector2DI Window_Imp_X11::GetWindowPosition()
{
	int32_t x = 0;
	int32_t y = 0;

	glfwGetWindowPos(m_window, &x, &y);

	return Vector2DI(x, y);
}

void Window_Imp_X11::SetWindowPosition(Vector2DI position)
{
	glfwSetWindowPos(m_window, position.X, position.Y);
}

void Window_Imp_X11::ShowWindow()
{
	glfwShowWindow(m_window);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Window_Imp_X11::Close()
{
	glfwSetWindowShouldClose(m_window, 1);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* Window_Imp_X11::GetWindowHandle() const
{
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
