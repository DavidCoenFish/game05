#pragma once
class VectorFloat2
{
public:
    explicit VectorFloat2(
        const float in_x = 0,
        const float in_y = 0
        );
    static const VectorFloat2 Cross(const VectorFloat2& in_subject);
    float&operator[](const int in_index);
    const float operator[](const int in_index) const;
    const float GetX() const;
    const float GetY() const;
    void NormaliseSelf();
    const bool operator==(const VectorFloat2& in_rhs) const;

private:
    enum Index
    {
        X = 0,
        Y,
        Count};

private:
    float _data[Index::Count];
};
