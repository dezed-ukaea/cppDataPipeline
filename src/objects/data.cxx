#include "scrc/objects/data.hxx"

namespace SCRC {
    void DataTable::delete_column(const std::string& col_name) {
        if(int_cols_.find(col_name) != int_cols_.end())
        {
            int_cols_.erase(col_name);
        }
        else if(float_cols_.find(col_name) != float_cols_.end())
        {
            float_cols_.erase(col_name);
        }
        else if(str_cols_.find(col_name) != str_cols_.end())
        {
            str_cols_.erase(col_name);
        }
        else
        {
            APILogger->error("Failed to delete column '{0}', column does not exist", col_name);
            throw std::invalid_argument("Invalid data table column name");
        }
        ncols_ -= 1;
    }

    std::string DataTable::print() const {
        std::stringstream out_str_;
        out_str_ << " | ";
        for(auto& col : int_cols_) {
            out_str_ << std::setw(10);
            out_str_ << col.second->name();
            out_str_ << " | ";
        }
        for(auto& col : float_cols_) {
            out_str_ << std::setw(10);
            out_str_ << col.second->name();
            out_str_ << " | ";
        }
        for(auto& col : str_cols_) {
            out_str_ << std::setw(10);
            out_str_ << col.second->name();
            out_str_ << " | ";
        }
        out_str_ << std::endl;
        out_str_ << " " << std::string(ncols_*13, '-');
        out_str_ << std::endl;
        for(int i{0}; i < row_names_.size(); ++i) {
            out_str_ << " | ";
            for(auto& col : int_cols_) {
                out_str_ << std::setw(10);
                out_str_ << col.second->operator[](i);
                out_str_ << " | ";
            }
            for(auto& col : float_cols_) {
                out_str_ << std::setw(10);
                out_str_ << col.second->operator[](i);
                out_str_ << " | ";
            }
            for(auto& col : str_cols_) {
                out_str_ << std::setw(10);
                out_str_ << col.second->operator[](i);
                out_str_ << " | ";
            }

            out_str_ << std::endl;
        }
        out_str_ << std::endl;
        return out_str_.str();
    }
};