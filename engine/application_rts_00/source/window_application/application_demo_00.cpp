#include "application_rts_00_pch.h"

#include "window_application/application_demo_00.h"

#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

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
{
    LOG_MESSAGE(
        "ApplicationDemo00 ctor %p",
        this
        );
    _draw_system = std::make_unique<DrawSystem> (in_hwnd);
}

ApplicationDemo00::~ApplicationDemo00()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationDemo00 dtor %p",
        this
        );
}

void ApplicationDemo00::Update()
{
    BaseType::Update();
    if (_draw_system)
    {
        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());
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
    if (_draw_system)
    {
        _draw_system->OnResize();
    }
    return;
}

