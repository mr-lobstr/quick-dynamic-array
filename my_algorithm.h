#ifndef MY_ALGORITHM_GUARD_H
#define MY_ALGORITHM_GUARD_H

template <typename T, typename Action>
void transform(T *beg, T *end, T *out, Action action)
{
    auto step = beg < end ? 1 : -1;

    while (beg != end)
    {
        action(out, *beg);
        out += step;
        beg += step;
    }
}

template <typename T, typename Action>
void for_each(T *beg, T *end, Action action)
{
    auto step = beg < end ? 1 : -1;

    while (beg != end)
    {
        action(beg);
        beg += step;
    }
}

#endif