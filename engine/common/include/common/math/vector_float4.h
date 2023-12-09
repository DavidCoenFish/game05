#pragma once

class VectorFloat2;

class VectorFloat4
{
public:
    static const VectorFloat4 s_zero;
    static const VectorFloat4 s_white;

    explicit VectorFloat4(
        const float in_x = 0,
        const float in_y = 0,
        const float in_z = 0,
        const float in_w = 0
        );
    float& operator[](const int in_index);
    const float operator[](const int in_index) const;
    const float GetX() const;
    const float GetY() const;
    const float GetZ() const;
    const float GetW() const;
    const bool operator==(const VectorFloat4& in_rhs) const;
    const bool operator!=(const VectorFloat4& in_rhs) const;
    VectorFloat4& operator=(const VectorFloat4& in_rhs);

    // Move to DscMath?
    const bool Inside(
        const VectorFloat2& in_value
        ) const;

private:
    enum Index
    {
        X = 0,
        Y,
        Z,
        W,
        Count};

private:
    float _data[Index::Count];
};
