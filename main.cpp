#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Fields {
public:
  using field_t = std::string;
  using fields_t = std::vector<field_t>;

  Fields(fields_t fields) : fields_{std::move(fields)} {}

  Fields() = default;
  Fields(Fields &&) = default;
  Fields(const Fields &) = default;

  void replace_field(std::size_t index, field_t new_field) {
    if (index >= fields_.size()) {
      throw std::length_error("field index is too large");
    }
    fields_[index] = std::move(new_field);
  }

  void insert_field(std::size_t index, field_t new_field) {
    if (index >= fields_.size()) {
      throw std::length_error("field index is too large");
    }
    fields_.insert(fields_.begin() + index, std::move(new_field));
  }

  void add_field(field_t new_field) {
    fields_.emplace_back(std::move(new_field));
  }

  void add_fields(const Fields &new_fields) {
    fields_.reserve(fields_.size() + new_fields.size());
    for (auto &&field : new_fields.fields_) {
      fields_.push_back(field);
    }
  }

  void remove_field(std::size_t index) {
    if (index >= fields_.size()) {
      throw std::length_error("field index is too large");
    }
    fields_.erase(fields_.begin() + index);
  }

  void insert_fields(std::size_t index, const Fields &new_fields) {
    if (index >= fields_.size()) {
      throw std::length_error("new fields index is too large");
    }
    fields_.insert(fields_.begin() + index, new_fields.fields_.begin(),
                   new_fields.fields_.end());
  }

  std::size_t size() const { return fields_.size(); }

  const field_t &operator[](std::size_t i) const { return fields_[i]; }

  field_t &operator[](std::size_t i) { return fields_[i]; }

  field_t get_field(std::size_t i) const { return fields_[i]; }

private:
  fields_t fields_;
};

class FieldsIO {
public:
  FieldsIO(char delim) : delim_{delim} {};

  Fields read_fields(std::string line) const {
    std::vector<std::string> fields;
    std::istringstream in(line);
    while (in.good()) {
      std::string field;
      std::getline(in, field, delim_);
      if (field.size() > 0) {
        fields.emplace_back(std::move(field));
      }
    }
    return fields;
  }

  Fields read_fields(std::istream &in) const {
    std::string input_buffer;
    std::getline(in, input_buffer);
    return read_fields(input_buffer);
  }

  std::vector<Fields> read_fields_multiline(std::istream &in,
                                            size_t n_lines) const {
    std::vector<Fields> output;
    output.reserve(n_lines);
    for (size_t i = 0; i < n_lines && in.good(); ++i) {
      output.emplace_back(read_fields(in));
    }
    return output;
  }

  void dump_fields(std::ostream &os, const Fields &fields) {
    if (fields.size() == 0) {
      return;
    }
    for (auto i = 0; i < fields.size() - 1; ++i) {
      os << fields[i] << delim_;
    }
    os << fields[fields.size() - 1];
  }

  template <typename BeginIt, typename EndIt>
  void dump_fields_multiline(std::ostream &os, BeginIt begin, EndIt end) {
    auto before_end = --end;
    for (auto i = begin; i != before_end; ++i) {
      dump_fields(os, *i);
      os << '\n';
    }
    dump_fields(os, *before_end);
  }

private:
  const char delim_;
};

void test_read_and_dump() {
  std::cout << "\ntest_read_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  io.dump_fields(std::cout, fields);
  std::cout << "\ntest_read_and_dump ends\n";
}

void test_read_5_lines_and_dump() {
  std::cout << "\ntest_read_5_lines_and_dump\n";
  auto io = FieldsIO('\t');
  std::vector<Fields> lines;
  for (std::size_t i = 0; i < 5; ++i) {
    lines.push_back(io.read_fields(std::cin));
  }
  io.dump_fields_multiline(std::cout, lines.begin(), lines.end());

  std::cout << "\ntest_read_5_lines_and_dump ends\n";
}

void test_edit_first_field_in_place_and_dump() {
  std::cout << "\ntest_edit_first_field_in_place_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  std::string &first_field = fields[0];
  first_field[0] = 'a';
  io.dump_fields(std::cout, fields);
  std::cout << "\ntest_edit_first_field_in_place_and_dump ends\n";
}

void test_edit_first_field_with_copy_and_dump() {
  std::cout << "\ntest_edit_first_field_with_copy_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  std::string first_field_copy = fields.get_field(0);
  first_field_copy[0] = 'a';
  fields.replace_field(0, first_field_copy);
  io.dump_fields(std::cout, fields);
  std::cout << "\ntest_edit_first_field_with_copy_and_dump ends\n";
}

void test_remove_first_field_and_dump() {
  std::cout << "\ntest_remove_first_field_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  fields.remove_field(0);
  io.dump_fields(std::cout, fields);

  std::cout << "\ntest_remove_first_field_and_dump ends\n";
}

void test_insert_field_on_second_pos_and_dump() {
  std::cout << "\ntest_insert_field_on_second_pos_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  fields.insert_field(1, "new_field");
  io.dump_fields(std::cout, fields);
  std::cout << "\ntest_insert_field_on_second_pos_and_ends\n";
}

void test_add_new_field_and_dump() {
  std::cout << "\ntest_add_new_field_and_dump\n";
  auto io = FieldsIO('\t');
  auto fields = io.read_fields(std::cin);
  fields.add_field("new_field");
  io.dump_fields(std::cout, fields);

  std::cout << "\ntest_add_new_field_and_dump ends\n";
}

void test_join_two_lines_and_dump() {
  std::cout << "\ntest_join_two_lines_and_dump\n";
  auto io = FieldsIO('\t');
  auto first_line = io.read_fields(std::cin);
  auto second_line = io.read_fields(std::cin);
  first_line.add_fields(second_line);
  io.dump_fields(std::cout, first_line);
  std::cout << "\ntest_join_two_lines_and_dump ends\n";
}

int main() {
  test_read_and_dump();
  test_read_5_lines_and_dump();
  test_edit_first_field_in_place_and_dump();
  test_edit_first_field_with_copy_and_dump();
  test_remove_first_field_and_dump();
  test_insert_field_on_second_pos_and_dump();
  test_add_new_field_and_dump();
  test_join_two_lines_and_dump();
}
