echo "Downloading Test Files for Unit Tests..."
wget https://data.scrc.uk/data_product/SCRC:geography/scotland/lookup_table@0.1.0 -O test/test_table.h5
wget https://data.scrc.uk/data_product/LSHTM:distributions/R0@0.1.0 -O test/test_dis.toml
wget https://data.scrc.uk/data_product/EERA:fixed-parameters/T_rec@0.1.0 -O test/test_pe.toml
wget https://data.scrc.uk/data_product/LSHTM:contact_matrices/national@0.20200811.0 -O test/test_array.h5