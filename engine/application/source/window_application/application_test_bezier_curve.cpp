#include "application_pch.h"

#include "window_application/application_test_bezier_curve.h"

#include "common/bezier/bezier_border_helper.h"
#include "common/bezier/bezier_curve.h"
#include "common/bezier/bezier_file_helper.h"
#include "common/bezier/bezier_manager.h"
#include "common/bezier/bezier_simulate.h"
#include "common/draw_system/custom_command_list.h"
#include "common/draw_system/draw_system.h"
#include "common/draw_system/draw_system_frame.h"
#include "common/draw_system/geometry/geometry_generic.h"
#include "common/draw_system/render_target/i_render_target.h"
#include "common/draw_system/shader/shader.h"
#include "common/draw_system/shader/shader_constant_buffer.h"
#include "common/draw_system/shader/shader_pipeline_state_data.h"
#include "common/file_system/file_system.h"
#include "common/log/log.h"
#include "common/math/angle.h"
#include "common/math/matrix_float33.h"
#include "common/math/quaternion_float.h"
#include "common/math/vector_int2.h"
#include "common/math/vector_float2.h"
#include "common/math/vector_float3.h"
#include "common/util/timer.h"
#include "common/util/vector_helper.h"
#include "common/window/window_application_param.h"

IWindowApplication* const ApplicationTestBezierCurve::Factory(
    const HWND in_hwnd,
    const WindowApplicationParam&in_application_param
    )
{
    return new ApplicationTestBezierCurve(
        in_hwnd,
        in_application_param
        );
}

ApplicationTestBezierCurve::ApplicationTestBezierCurve(
    const HWND in_hwnd,
    const WindowApplicationParam& in_application_param
    ) 
    : IWindowApplication(
        in_hwnd,
        in_application_param
        )
    , _time_accumulate(0.0f)
    , _counter(0)
{
    LOG_MESSAGE(
        "ApplicationTestBezierCurve  ctor %p",
        this
        );
    RenderTargetFormatData render_target_format_data(
        DXGI_FORMAT_B8G8R8A8_UNORM,
        true,
        VectorFloat4(0.5f, 0.5f, 0.5f, 1.0f)
        );
    _draw_system = std::make_unique < DrawSystem > (
        in_hwnd, 
        2, //in_back_buffer_count
        D3D_FEATURE_LEVEL_11_0, 
        0, //in_options
        render_target_format_data
        );

    auto command_list = _draw_system->CreateCustomCommandList();

    _bezier_manager = std::make_unique<BezierManager>(
        _draw_system.get(),
        command_list->GetCommandList(),
        in_application_param._root_path
        );
#if 0
     std::vector<BezierCurve::BezierSegment> segment_data({
        { VectorFloat2(-100.0f, 0.0f), VectorFloat2(-100.0f, 100.0f), VectorFloat2(0.0f, 100.0f), 1.0f, 1.0f},
        { VectorFloat2(0.0f, 100.0f), VectorFloat2(100.0f, 100.0f), VectorFloat2(100.0f, 0.0f), 1.0f, 10.0f},
        { VectorFloat2(100.0f, 0.0f), VectorFloat2(100.0f, -100.0f), VectorFloat2(0.0f, -100.0f), 10.0f, 1.0f},
        { VectorFloat2(0.0f, -100.0f), VectorFloat2(-100.0f, -100.0f), VectorFloat2(-100.0f, 0.0f), 1.0f, 1.0f}
     });
#elif 0
    std::vector<BezierCurve::BezierSegment> segment_data;
    BezierFileHelper::SegmentDataFromSvg(segment_data, in_application_param._data_path / "leaf_00_e.svg");
#elif 0
    std::vector<BezierCurve::BezierSegment> segment_data;
    BezierBorderHelper::GenerateSegmentBorder0(
        segment_data,
        VectorFloat2(200.0f, 50.0f),
        64.0f,
        32.0f,
        64.0f,
        32.0f,
        1.0f,
        2.0f,
        VectorFloat2(128.0f, 64.0f)
        );
#else
    std::vector<BezierCurve::BezierSegment> segment_data;
    BezierBorderHelper::GenerateSegmentBorder0(
        segment_data,
        VectorFloat2(200.0f, 50.0f),
        64.0f,
        32.0f,
        64.0f,
        32.0f,
        1.0f,
        2.0f,
        VectorFloat2(128.0f, 64.0f)
        );
    _bezier_simulate = std::make_unique<BezierSimulate>(
        segment_data
        );

    _bezier_simulate->GatherData(segment_data);
    _timer = std::make_unique<Timer>();

#endif
    _bezier_curve = std::make_unique<BezierCurve>(
        segment_data
        );
}

ApplicationTestBezierCurve::~ApplicationTestBezierCurve()
{
    if (_draw_system)
    {
        _draw_system->WaitForGpu();
    }
    _timer.reset();
    _bezier_simulate.reset();
    _bezier_curve.reset();
    _bezier_manager.reset();
    _draw_system.reset();
    LOG_MESSAGE(
        "ApplicationTestBezierCurve  dtor %p",
        this
        );
}

void ApplicationTestBezierCurve::Update()
{
    BaseType::Update();

    if (_draw_system)
    {
        const float delta_seconds = _timer ? _timer->GetDeltaSeconds() : 0.0f;

        _time_accumulate += delta_seconds;

        auto simulate = _bezier_simulate.get();
        if (nullptr != simulate)
        {
            if (4.0f < _time_accumulate)
            {
                _time_accumulate -= 4.0f;
                if (_counter == 0)
                {
                    _counter = 1;
                    std::vector<BezierCurve::BezierSegment> segment_data;
                    BezierBorderHelper::GenerateSegmentBorder0(segment_data, VectorFloat2(100.0f, 100.0f), 64.0f, 32.0f, 64.0f, 32.0f, 1.5f, 3.0f, VectorFloat2(178.0f, 32.0f));
                    simulate->SetTargetSoft(segment_data);
                }
                else
                {
                    _counter = 0;
                    std::vector<BezierCurve::BezierSegment> segment_data;
                    BezierBorderHelper::GenerateSegmentBorder0(segment_data, VectorFloat2(200.0f, 100.0f), 64.0f, 32.0f, 64.0f, 32.0f, 1.5f, 3.0f, VectorFloat2(128.0f, 64.0f));
                    simulate->SetTargetSoft(segment_data);
                }
            }

            simulate->Update(delta_seconds);

            {
                std::vector<BezierCurve::BezierSegment> segment_data;
                simulate->GatherData(segment_data);
                if (nullptr != _bezier_curve)
                {
                    _bezier_curve->SetData(segment_data);
                }
            }
        }

        auto frame = _draw_system->CreateNewFrame();
        frame->SetRenderTarget(_draw_system->GetRenderTargetBackBuffer());

        if ((nullptr != _bezier_curve) && (nullptr != _bezier_manager))
        {
            _bezier_curve->SetContainerSize(
                _draw_system->GetRenderTargetBackBuffer()->GetSize(),
                VectorInt2(0, 0)
                );
            
            // Draw
            _bezier_manager->DrawBezierCurve(
                _draw_system.get(),
                frame.get(),
                _bezier_curve.get()
                );
        }
    }
}

void ApplicationTestBezierCurve::OnWindowSizeChanged(
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
