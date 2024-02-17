#include "common/common_pch.h"
#include "common/bezier/bezier_border_helper.h"

void BezierBorderHelper::GenerateCurledUpSegment(
    std::vector<BezierCurve::BezierSegment>& ,//out_segment_data, 
    const std::vector<BezierCurve::BezierSegment>& //in_reference_segment_data
    )
{
}

/*
ref data: x==256 mirror, border left 96, top 96, right 96, bottom 96 (inner 64, 320)  (top, under 96y, middle to 160, bottom 256
m 256,256 c 10e-6,-39.99999 10e-6,-39.99999 8.00001,-55.99999 8,-16 8,-16.58086 -8,-24.00001
m 256,176 c -31.99999,-8 -31.99999,-8 -63.73575,0.14313
M 192.26425,176.14313 C 95.5175,192.82991 96.081338,192.18931 72.061003,160.48249
M 72.061003,160.48249 C 64.000001,128 63.86575,128.26851 80.000002,96.000003 96.000003,64.000002 96.000003,64.000002 160,80.000002 c 64.00001,23.999998 64.01303,24.009758 88.00001,-8 11.98983,-16 12.00339,-16 0,-32.000001 -14.3242,-19.093544 -16,-19.669774 -32,0
*/
namespace
{
    enum class NineSliceQuadrant
    {
        TTopLeft,
        TTopCenter,
        TTopRight,
        TMiddleLeft,
        TMiddleCenter,
        TMiddleRight,
        TBottomLeft,
        TBottomCenter,
        TBottomRight
    };
    struct TPoint
    {
    public:
        NineSliceQuadrant Quadrant;
        float X;
        float Y;
    };

    struct TSegment
    {
    public:
        TPoint Data[3];
    };


    static const TSegment s_border0[] =
    {
        {
            //256,256
            TPoint({NineSliceQuadrant::TBottomCenter, 0.5f, 1.0f}),
            //256,216
            TPoint({NineSliceQuadrant::TBottomCenter, 0.5f, 0.5833333333f}),
            //264,201
            TPoint({NineSliceQuadrant::TBottomCenter, 0.525f, 0.427f})
        },
        {
            //264,201
            TPoint({NineSliceQuadrant::TBottomCenter, 0.525f, 0.427f}),
            //271, 183
            TPoint({NineSliceQuadrant::TBottomCenter, 0.546875f, 0.23958f}),
            //256, 176
            TPoint({NineSliceQuadrant::TBottomCenter, 0.5f, 0.166f})
        },
        {
            //256, 176
            TPoint({NineSliceQuadrant::TBottomCenter, 0.5f, 0.166f}),
            //223, 168
            TPoint({NineSliceQuadrant::TBottomCenter, 0.396875f, 0.083333f}),
            //192, 176
            TPoint({NineSliceQuadrant::TBottomCenter, 0.3f, 0.166f})
        },
        {
            //192, 176
            TPoint({NineSliceQuadrant::TBottomCenter, 0.3f, 0.166f}),
            //95,192
            TPoint({NineSliceQuadrant::TBottomLeft, 0.98f, 0.333f}),
            //72,162
            TPoint({NineSliceQuadrant::TBottomLeft, 0.75f, 0.02083f})
        },
        {
            //72,162
            TPoint({NineSliceQuadrant::TBottomLeft, 0.75f, 0.02083f}),
            //64,128
            TPoint({NineSliceQuadrant::TMiddleLeft, 0.66f, 0.5f}),
            //80,96
            TPoint({NineSliceQuadrant::TMiddleLeft, 0.83333f, 0.0f})
        },
        {
            //80,96
            TPoint({NineSliceQuadrant::TMiddleLeft, 0.83333f, 0.0f}),
            //96,63
            TPoint({NineSliceQuadrant::TTopCenter, 0.0f, 0.65625f}),
            //160,80
            TPoint({NineSliceQuadrant::TTopCenter, 0.2f, 0.83333f})
        },
        {
            //160,80
            TPoint({NineSliceQuadrant::TTopCenter, 0.2f, 0.83333f}),
            //223,104
            TPoint({NineSliceQuadrant::TMiddleCenter, 0.396875f, 0.125f}),
            //248,72
            TPoint({NineSliceQuadrant::TTopCenter, 0.475f, 0.75f})
        },
        {
            //248,72
            TPoint({NineSliceQuadrant::TTopCenter, 0.475f, 0.75f}),
            //260,56
            TPoint({NineSliceQuadrant::TTopCenter, 0.5125f, 0.5833f}),
            //248,40
            TPoint({NineSliceQuadrant::TTopCenter, 0.475f, 0.41666f})
        },
        {
            //248,40
            TPoint({NineSliceQuadrant::TTopCenter, 0.475f, 0.41666f}),
            //235,21
            TPoint({NineSliceQuadrant::TTopCenter, 0.434375f, 0.21875f}),
            //216,40
            TPoint({NineSliceQuadrant::TTopCenter, 0.375f, 0.4166f})
        }
    };
    static const int s_border0_count = ARRAY_LITERAL_SIZE(s_border0);

    const float CalculateNineSliceX(
        const NineSliceQuadrant in_quadrant, 
        const float in_value,
        const float in_stop_1,
        const float in_stop_2,
        const float in_stop_3
        )
    {
        float low = 0.0f;
        float high = 0.0f;
        switch(in_quadrant)
        {
        case NineSliceQuadrant::TBottomLeft:
        case NineSliceQuadrant::TMiddleLeft:
        case NineSliceQuadrant::TTopLeft:
            high = in_stop_1;
            break;
        case NineSliceQuadrant::TBottomCenter:
        case NineSliceQuadrant::TMiddleCenter:
        case NineSliceQuadrant::TTopCenter:
            low = in_stop_1;
            high = in_stop_2;
            break;
        case NineSliceQuadrant::TBottomRight:
        case NineSliceQuadrant::TMiddleRight:
        case NineSliceQuadrant::TTopRight:
            low = in_stop_2;
            high = in_stop_3;
            break;
        default:
            break;
        }

        const float result = (low * (1.0f - in_value)) + (high * in_value);
        return result;
    }
    const float CalculateNineSliceXMirror(
        const NineSliceQuadrant in_quadrant, const float in_value,
        const float in_stop_1,
        const float in_stop_2,
        const float in_stop_3
        )
    {
        float low = 0.0f;
        float high = 0.0f;
        switch(in_quadrant)
        {
        case NineSliceQuadrant::TBottomRight:
        case NineSliceQuadrant::TMiddleRight:
        case NineSliceQuadrant::TTopRight:
            high = in_stop_1;
            break;
        case NineSliceQuadrant::TBottomCenter:
        case NineSliceQuadrant::TMiddleCenter:
        case NineSliceQuadrant::TTopCenter:
            low = in_stop_1;
            high = in_stop_2;
            break;
        case NineSliceQuadrant::TBottomLeft:
        case NineSliceQuadrant::TMiddleLeft:
        case NineSliceQuadrant::TTopLeft:
            low = in_stop_2;
            high = in_stop_3;
            break;
        default:
            break;
        }

        const float result = (low * in_value) + (high * (1.0f - in_value));
        return result;
    }

    const float CalculateNineSliceY(
        const NineSliceQuadrant in_quadrant, const float in_value,
        const float in_stop_1,
        const float in_stop_2,
        const float in_stop_3
        )
    {
        float low = 0.0f;
        float high = 0.0f;
        switch(in_quadrant)
        {
        case NineSliceQuadrant::TTopLeft:
        case NineSliceQuadrant::TTopCenter:
        case NineSliceQuadrant::TTopRight:
            high = in_stop_1;
            break;
        case NineSliceQuadrant::TMiddleLeft:
        case NineSliceQuadrant::TMiddleCenter:
        case NineSliceQuadrant::TMiddleRight:
            low = in_stop_1;
            high = in_stop_2;
            break;

        case NineSliceQuadrant::TBottomLeft:
        case NineSliceQuadrant::TBottomCenter:
        case NineSliceQuadrant::TBottomRight:
            low = in_stop_2;
            high = in_stop_3;
            break;
        default:
            break;
        }

        const float result = (low * (1.0f - in_value)) + (high * in_value);
        return result;
    }

    void AppendSegment(
        std::vector<BezierCurve::BezierSegment>& out_segment_data,
        const TSegment& in_segment,
        const float in_thick_0,
        const float in_thick_1,
        const float in_x_stop_1,
        const float in_x_stop_2,
        const float in_x_stop_3,
        const float in_y_stop_1,
        const float in_y_stop_2,
        const float in_y_stop_3,
        const VectorFloat2& in_offset
        )
    {
        const float x_left_0 = CalculateNineSliceX(
            in_segment.Data[0].Quadrant,
            in_segment.Data[0].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float x_right_0 = CalculateNineSliceXMirror(
            in_segment.Data[0].Quadrant,
            in_segment.Data[0].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float y_0 = CalculateNineSliceY(
            in_segment.Data[0].Quadrant,
            in_segment.Data[0].Y,
            in_y_stop_1,
            in_y_stop_2,
            in_y_stop_3
            ) + in_offset.GetY();

        const float x_left_1 = CalculateNineSliceX(
            in_segment.Data[1].Quadrant,
            in_segment.Data[1].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float x_right_1 = CalculateNineSliceXMirror(
            in_segment.Data[1].Quadrant,
            in_segment.Data[1].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float y_1 = CalculateNineSliceY(
            in_segment.Data[1].Quadrant,
            in_segment.Data[1].Y,
            in_y_stop_1,
            in_y_stop_2,
            in_y_stop_3
            ) + in_offset.GetY();

        const float x_left_2 = CalculateNineSliceX(
            in_segment.Data[2].Quadrant,
            in_segment.Data[2].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float x_right_2 = CalculateNineSliceXMirror(
            in_segment.Data[2].Quadrant,
            in_segment.Data[2].X,
            in_x_stop_1,
            in_x_stop_2,
            in_x_stop_3
            ) + in_offset.GetX();
        const float y_2 = CalculateNineSliceY(
            in_segment.Data[2].Quadrant,
            in_segment.Data[2].Y,
            in_y_stop_1,
            in_y_stop_2,
            in_y_stop_3
            ) + in_offset.GetY();

        out_segment_data.push_back(BezierCurve::BezierSegment({
            VectorFloat2(x_left_0, y_0),
            VectorFloat2(x_left_1, y_1),
            VectorFloat2(x_left_2, y_2),
            in_thick_0,
            in_thick_1
        }));

        out_segment_data.push_back(BezierCurve::BezierSegment({
            VectorFloat2(x_right_0, y_0),
            VectorFloat2(x_right_1, y_1),
            VectorFloat2(x_right_2, y_2),
            in_thick_0,
            in_thick_1
        }));
    }

}

void BezierBorderHelper::GenerateSegmentBorder0(
    std::vector<BezierCurve::BezierSegment>& out_segment_data, 
    const VectorFloat2& in_inner_size,
    const float in_border_left,
    const float in_border_top,
    const float in_border_right,
    const float in_border_bottom,
    const float in_thickness_min,
    const float in_thickness_max,
    const VectorFloat2& in_offset
    )
{
    out_segment_data.clear();

    const float x_stop_1 = in_border_left;
    const float x_stop_2 = x_stop_1 + in_inner_size.GetX();
    const float x_stop_3 = x_stop_2 + in_border_right;

    const float y_stop_1 = in_border_top;
    const float y_stop_2 = y_stop_1 + in_inner_size.GetY();
    const float y_stop_3 = y_stop_2 + in_border_bottom;

    for (int index = 0; index < s_border0_count; ++index)
    {
        float thick_0 = in_thickness_min;
        float thick_1 = in_thickness_max;

        // after the first segment (the stem) the rest of the branch slowly taper off back down to min
        if (0 < index)
        {
            const float ratio_0 = (index - 1.0f) / static_cast<float>(s_border0_count - 1.0f);
            const float ratio_1 = (index) / static_cast<float>(s_border0_count - 1.0f);

            thick_0 = (in_thickness_min * ratio_0) + (in_thickness_max * (1.0f - ratio_0));
            thick_1 = (in_thickness_min * ratio_1) + (in_thickness_max * (1.0f - ratio_1));
        }

        AppendSegment(
            out_segment_data,
            s_border0[index],
            thick_0,
            thick_1,
            x_stop_1,
            x_stop_2,
            x_stop_3,
            y_stop_1,
            y_stop_2,
            y_stop_3,
            in_offset
            );
    }
}
