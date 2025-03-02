#pragma once
#include "common/window/i_window_application.h"

class DrawSystem;
class SceneComponentScreenQuad;
class SceneComponentCameraRay;
class SceneComponentBackground;
class SceneComponentGrid;
class Timer;

class SceneComponentLineSpline;

class ApplicationDemo00 : public IWindowApplication
{
public:
	ApplicationDemo00(
		const HWND in_hwnd,
		const WindowApplicationParam& in_application_param
		);
	virtual ~ApplicationDemo00();
	static IWindowApplication* const Factory(
		const HWND in_hwnd,
		const WindowApplicationParam& in_application_param
		);

private:
	virtual void Update() override;
	virtual void OnWindowSizeChanged(
		const int in_width,
		const int in_height
		) override;
    virtual void OnKey(
        const int in_vk_code, 
        const int in_scan_code, 
        const bool in_repeat_flag, 
        const int in_repeat_count, 
        bool in_up_flag
        ) override;

private:
	typedef IWindowApplication BaseType;
	std::unique_ptr<DrawSystem> _draw_system;

    struct DrawResources
    {
        std::shared_ptr<SceneComponentScreenQuad> _screen_quad;
        std::shared_ptr<SceneComponentCameraRay> _camera_ray;
        std::shared_ptr<SceneComponentBackground> _background;
		std::shared_ptr<SceneComponentGrid> _grid;

		std::shared_ptr<SceneComponentLineSpline> _line_spline;

		
	};
    std::unique_ptr<DrawResources> _draw_resources;

    std::unique_ptr<Timer> _timer;
    float _timer_accumulate_wrapped;

};
