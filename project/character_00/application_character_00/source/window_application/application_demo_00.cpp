#include "application_character_00_pch.h"
#include "window_application/application_demo_00.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

#include "scene_component/scene_component_background.h"
#include "scene_component/scene_component_camera_ray.h"
#include "scene_component/scene_component_disk.h"
#include "scene_component/scene_component_grid.h"
#include "scene_component/scene_component_screen_quad.h"
#include "scene_component/scene_component_sphere.h"

IWindowApplication* const ApplicationDemo00::Factory(
	const HWND in_hwnd,
	const WindowApplicationParam&in_application_param
	)
{
	return new ApplicationDemo00(
		in_hwnd,
		in_application_param
		);
}

ApplicationDemo00::ApplicationDemo00(
	const HWND in_hwnd,
	const WindowApplicationParam& in_application_param
	) 
	: IWindowApplication(
		in_hwnd,
		in_application_param
		)
    , _timer_accumulate_wrapped(0.0f)
{
	LOG_MESSAGE(
		"ApplicationDemo00 ctor %p",
		this
		);
	_draw_system = std::make_unique<DrawSystem> (in_hwnd);


    {
        _draw_resources = std::make_unique<DrawResources>();
        auto command_list = _draw_system->CreateCustomCommandList();

        _draw_resources->_screen_quad = SceneComponentScreenQuad::Factory(
            _draw_system.get(),
            command_list->GetCommandList()
            );

        _draw_resources->_camera_ray = SceneComponentCameraRay::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
			VectorFloat3(0.0f, 1.0f, 0.0)
            );

        _draw_resources->_background = SceneComponentBackground::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay()
            );

        _draw_resources->_grid = SceneComponentGrid::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay()
            );

        _draw_resources->_disk = SceneComponentDisk::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay(),
			VectorFloat4(1.0f, 0.0f, 0.0f, 0.5f),
			//VectorFloat3(0.0f, 0.0f, 1.0f) 
			VectorFloat3(3.0f, 2.0f, 1.0f)
			);

        _draw_resources->_sphere_0 = SceneComponentSphere::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay(),
			VectorFloat4(0.0f, 1.0f, 0.0f, 0.5f),
			VectorFloat4(3.0f, 0.0f, 2.0f, 0.5f)
			);
        _draw_resources->_sphere_1 = SceneComponentSphere::Factory(
            _draw_system.get(),
            command_list->GetCommandList(),
            SceneComponentScreenQuad::GetInputElementDescArray(),
            in_application_param._root_path,
            _draw_resources->_camera_ray->GetHeapWrapperCameraRay(),
			VectorFloat4(0.0f, 0.0f, 1.0f, 0.5f),
			VectorFloat4(0.0f, 0.0f, 0.0f, 1.0f)
			);

	}

    _timer = std::make_unique<Timer>();
}

ApplicationDemo00::~ApplicationDemo00()
{
	if (_draw_system)
	{
		_draw_system->WaitForGpu();
	}

    _draw_resources.reset();
	_draw_system.reset();
	LOG_MESSAGE(
		"ApplicationDemo00 dtor %p",
		this
		);
}

void ApplicationDemo00::Update()
{
	BaseType::Update();

    const float time_delta = _timer ? _timer->GetDeltaSeconds() : 0.0f;

	if (_draw_system && _draw_resources)
	{
		auto frame = _draw_system->CreateNewFrame();
        const auto& screen_quad = _draw_resources->_screen_quad ? _draw_resources->_screen_quad->GetGeometryRef() : nullptr;

        if (_draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_camera_ray->Update(
                time_delta,
                frame.get(),
                screen_quad
                );
        }

        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        if (_draw_resources->_background && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_background->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }

        if (_draw_resources->_grid && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_grid->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }
#if 1
        if (_draw_resources->_disk && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_disk->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }
#endif
        if (_draw_resources->_sphere_0 && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_sphere_0->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }

        if (_draw_resources->_sphere_1 && _draw_resources->_camera_ray && screen_quad)
        {
            _draw_resources->_sphere_1->Draw(
                _draw_system.get(),
                frame.get(),
                screen_quad,
                _draw_resources->_camera_ray->GetConstantBufferB0()
                );
        }

	}
}

void ApplicationDemo00::OnWindowSizeChanged(
	const int in_width,
	const int in_height
	)
{
	BaseType::OnWindowSizeChanged(
		in_width,
		in_height
		);

    if (_draw_resources && _draw_resources->_camera_ray)
    {
        _draw_resources->_camera_ray->OnWindowSizeChanged(
            in_width,
            in_height
            );
    }

	if (_draw_system)
	{
		_draw_system->OnResize();
	}

	return;
}

void ApplicationDemo00::OnKey(
    const int in_vk_code,
    const int in_scan_code,
    const bool in_repeat_flag,
    const int in_repeat_count,
    bool in_up_flag
    )
{
    in_scan_code;
    in_repeat_flag;
    in_repeat_count;

    if (_draw_resources && _draw_resources->_camera_ray)
    {
        _draw_resources->_camera_ray->OnKey(
            in_vk_code,
            in_up_flag
            );
    }

    return;
}

