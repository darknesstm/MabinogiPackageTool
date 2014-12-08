#pragma once

#include "htmlayout.h"

namespace htmlayout
{
	class MainWindow : public event_handler,
		public notification_handler < MainWindow >
	{
	public:
		~MainWindow();

		HWND          hwnd;

		dom::element topbar;
		dom::element  body;
		dom::element  caption;
		dom::element  button_min;
		dom::element  button_max;
		dom::element  button_icon;
		dom::element  button_close;
		dom::element  corner;

		static  MainWindow* create(int x, int y, int width, int height, const wchar_t* caption = 0);
		static  MainWindow* self(HWND hWnd) { return (MainWindow*)::GetWindowLongPtr(hWnd, GWLP_USERDATA); }

		void            set_caption(const wchar_t* text);

		static  ATOM              register_class(HINSTANCE hInstance);
	protected:
		MainWindow() : event_handler(HANDLE_BEHAVIOR_EVENT) {}

		int       hit_test(int x, int y);
		HELEMENT  root();
		bool      is_minimized() const;
		bool      is_maximized() const;

		virtual BOOL  on_event(HELEMENT he, HELEMENT target, BEHAVIOR_EVENTS type, UINT_PTR reason);

		static  void              self(HWND hWnd, MainWindow* inst) { ::SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(inst)); }
		static  LRESULT CALLBACK  win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		static  HINSTANCE         hinstance;
	};

}