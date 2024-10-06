#pragma once

#include "common/math/vector_float2.h"

class DrawSystem;
class DrawSystemFrame;
class Shader;
class BezierCurve;
class BezierManagerImplementation;

/// these are helper classes to draw bezier curves with the draw system (like text). It could be under the draw system?
/// this is not a generic bezier curve class, this is data to draw bezier with the draw system
class BezierManager
{
public:
	static const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElementDesc();

	BezierManager(
		DrawSystem* const in_draw_system,
		ID3D12GraphicsCommandList* const in_command_list,
		const std::filesystem::path& in_root_path
		);
	~BezierManager();

	void DrawBezierCurve(
		DrawSystem* const in_draw_system,
		DrawSystemFrame* const in_draw_system_frame,
		BezierCurve* const in_bezier_curve
		);

private:
	std::unique_ptr<BezierManagerImplementation> _implementation;

};
