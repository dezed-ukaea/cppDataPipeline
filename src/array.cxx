#include "array.hxx"

namespace SCRC
{
    template<typename T>
    Array<T>::Array(int ndims_p, std::vector<int> dims_p) : dims_(dims_p), ndims_(ndims_p)
    {
        int nvals = 1;

        assert(ndims_p > 0);

        for (int d = 0; d < ndims_; d++)
        {
            nvals *= dims_p.at(d);
        }

        data_.resize(nvals);
    }

    template<typename T>
    const T &Array<T>::operator()(int i) const
    {
        if (ndims_ == 1)
        {
            return data_.at(i);
        }
        else
        {
            throw std::logic_error("Array dimension mismatch");
        }
    }

    template<typename T>
    T &Array<T>::operator()(int i)
    {
        return const_cast<T&>(const_cast<const Array<T>*>(this)->operator()(i));
    }

    template<typename T>
    const T &Array<T>::operator()(int i, int j) const
    {
    if (ndims_ == 2) {
        if ((i < 0 || i >= dims_.at(0)) ||
            (j < 0 || j >= dims_.at(1))) {
        throw std::domain_error("Attempt to access element ("+std::to_string(i)+","+std::to_string(j)+
                            " of an array of dimensions ("+std::to_string(dims_.at(0))+","+std::to_string(dims_.at(1))+")");
        }

        // This is column-major (Fortran) ordering, so that looping over x is fastest
        return data_.at(j*dims_.at(0)+i); // TODO: decide on row-major or column-major
    } else {
        throw std::logic_error("Array dimension mismatch");
    }
    }

    template<typename T>
    T &Array<T>::operator()(int i, int j)
    {
        return const_cast<T&>(const_cast<const Array<T>*>(this)->operator()(i,j));
    }

    template<typename T>
    std::vector<int> Array<T>::size() const
    {
        return dims_;
    }

    template class Array<double>;
    template class Array<int>;
    template class Array<float>;
};