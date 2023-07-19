#pragma once
class QuaternionFloat;
class VectorFloat3;
class MatrixFloat33;

class MatrixFloat33
{
public:
    static const MatrixFloat33 FactoryQuaternionFloat(const QuaternionFloat&in_rotation);
    explicit MatrixFloat33(
        const float in_a00 = 0,
        const float in_a10 = 0,
        const float in_a20 = 0,
        const float in_a01 = 0,
        const float in_a11 = 0,
        const float in_a21 = 0,
        const float in_a02 = 0,
        const float in_a12 = 0,
        const float in_a22 = 0
        );
    float& operator[](const int in_index);
    const float operator[](const int in_index) const;
    const bool operator==(const MatrixFloat33& in_rhs) const;

    friend const VectorFloat3 operator* (const MatrixFloat33& in_lhs, const VectorFloat3& in_rhs);

private:
    enum Index
    {
        M00 = 0,
        M10,
        M20,
        M01,
        M11,
        M21,
        M02,
        M12,
        M22,
        Count
    };

private:
    float _data[Index::Count];

};

const VectorFloat3 operator* (const MatrixFloat33& in_lhs, const VectorFloat3& in_rhs);
