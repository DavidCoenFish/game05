#pragma once

class VectorInt4
{
public:
    static const VectorInt4 s_zero;

    explicit VectorInt4(
        const int in_x = 0,
        const int in_y = 0,
        const int in_z = 0,
        const int in_w = 0
        );
    int& operator[](const int in_index);
    const int operator[](const int in_index) const;
    const int GetX() const;
    const int GetY() const;
    const int GetZ() const;
    const int GetW() const;

    const bool operator==(const VectorInt4& in_rhs) const;
    const bool operator!=(const VectorInt4& in_rhs) const;

private:
    enum Index
    {
        X = 0,
        Y,
        Z,
        W,
        Count
    };

    int _data[Index::Count];

};
