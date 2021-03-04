#ifndef __ARRAY_HXX__
#define __ARRAY_HXX__

#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>

namespace SCRC
{
    template <typename T>
    class Array
    {
        public:
            Array(int ndims_p, std::vector<int> dims_p);

            T &operator()(int i, int j);
            T &operator()(int i);

            const T &operator()(int i, int j) const;
            const T &operator()(int i) const;

            std::vector<int> size() const;

        private:
            std::vector<T> data_;
            const std::vector<int> dims_;
            const int ndims_;
    };
};

#endif