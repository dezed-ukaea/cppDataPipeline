#ifndef __SCRC_HDF5__
#define __SCRC_HDF5__

#include "H5Cpp.h"
#include <stdexcept>
#include <string>

namespace SCRC
{
    namespace HDF5
    {
        template<typename T>
        const H5::DataType* get_hdf5_type()
        {
            if(std::is_same<T, float>::value)
            {
                return &H5::PredType::NATIVE_FLOAT;
            }

            else if(std::is_same<T, double>::value)
            {
                return &H5::PredType::NATIVE_DOUBLE;
            }

            else if(std::is_same<T, long double>::value)
            {
                return &H5::PredType::NATIVE_LDOUBLE;
            }

            else if(std::is_same<T, char>::value || std::is_same<T, std::string>::value)
            {
                return &H5::PredType::NATIVE_CHAR;
            }

            else if(std::is_same<T, unsigned char>::value)
            {
                return &H5::PredType::NATIVE_UCHAR;
            }

            else if(std::is_same<T, short>::value)
            {
                return &H5::PredType::NATIVE_SHORT;
            }

            else if(std::is_same<T, unsigned short>::value)
            {
                return &H5::PredType::NATIVE_USHORT;
            }

            else if(std::is_same<T, int>::value)
            {
                return &H5::PredType::NATIVE_INT;
            }

            else if(std::is_same<T, unsigned int>::value)
            {
                return &H5::PredType::NATIVE_UINT;
            }

            else if(std::is_same<T, long>::value)
            {
                return &H5::PredType::NATIVE_LONG;
            }

            else if(std::is_same<T, unsigned long>::value)
            {
                return &H5::PredType::NATIVE_ULONG;
            }

            else if(std::is_same<T, long long>::value)
            {
                return &H5::PredType::NATIVE_LLONG;
            }

            else if(std::is_same<T, unsigned long long>::value)
            {
                return &H5::PredType::NATIVE_ULLONG;
            }
            
            throw std::runtime_error("Unsupported C++ type, could not convert to HDF5 type");
        }
    };
};

#endif