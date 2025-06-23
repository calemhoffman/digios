#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <memory>
#include <vector>
#include <string>

using namespace arrow;

int main() {
    // Create schema
    auto field1 = field("id", int32());
    auto field2 = field("name", utf8());
    auto schema = std::make_shared<Schema>(std::vector<std::shared_ptr<Field>>{field1, field2});

    // Create data for id column
    Int32Builder id_builder;
    id_builder.AppendValues({1, 2, 3, 4, 5});

    // Create data for name column
    StringBuilder name_builder;
    name_builder.AppendValues({"Alice", "Bob", "Charlie", "David", "Eve"});

    // Build arrays
    std::shared_ptr<Array> id_array;
    id_builder.Finish(&id_array);
    std::shared_ptr<Array> name_array;
    name_builder.Finish(&name_array);

    // Create table
    auto table = Table::Make(schema, {id_array, name_array});

    // Create output file
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open("output.parquet"));

    // Write to Parquet
    parquet::WriterProperties::Builder builder;
    std::shared_ptr<parquet::WriterProperties> props = builder.build();
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile, 1024, props));

    return 0;
}