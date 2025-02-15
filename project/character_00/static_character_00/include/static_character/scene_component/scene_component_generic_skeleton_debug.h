#pragma once

class DrawSystem;
class DrawSystemFrame;
class GeometryGeneric;

struct CameraConstantBufferB0;


class SceneComponentGenericSkeletonDebug
{
public:
    SceneComponentGenericSkeletonDebug(
        );
    ~SceneComponentGenericSkeletonDebug();

    void Draw(
        DrawSystem* const in_draw_system,
        DrawSystemFrame* const in_draw_system_frame,
        const std::shared_ptr<GeometryGeneric>& in_screen_quad,
        const CameraConstantBufferB0& in_camera_constant_buffer
        );

	void SetData(
		);

private:

};
