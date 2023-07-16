#pragma once
class VectorFloat3
{
public:
    explicit VectorFloat3(
        const float in_x = 0,
        const float in_y = 0,
        const float in_z = 0
        );
    static const VectorFloat3 Cross(
        const VectorFloat3&in_lhs,
        const VectorFloat3&in_rhs
        );
    float& operator[](const int in_index);
    const float operator[](const int in_index) const;
    const float GetX() const;
    const float GetY() const;
    const float GetZ() const;
    void NormaliseSelf();
    const bool operator==(const VectorFloat3& in_rhs) const;

private:
    enum Index
    {
        X = 0,
        Y,
        Z,
        Count};

private:
    float _data[Index::Count];
};
