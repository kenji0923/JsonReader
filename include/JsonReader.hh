#ifndef JsonReader_h
#define JsonReader_h 1

#include "json11.hpp"

#include "TString.h"

#include <string>
#include <memory>

class JsonReader {
  std::shared_ptr<const json11::Json> Data;
  const json11::Json* CurrentData = nullptr;
  void ValueCheck(const std::string& key, const json11::Json &Content, json11::Json::Type type) const;

public:
  JsonReader() {}
  JsonReader(std::string ReadFileName);
  JsonReader(TString ReadFileName);
  JsonReader(const JsonReader& rh);
  JsonReader(JsonReader&& rh);
  ~JsonReader() {};

  void operator= (const JsonReader& rh);
  JsonReader operator[] (const std::string &key) const;
  JsonReader operator[] (size_t i) const;

  bool is_null() const;
  bool is_array() const;
  bool is_object() const;
  std::vector<JsonReader> array_items() const;
  std::map<std::string, JsonReader> object_items() const;
  double number_value() const;
  int int_value() const;
  bool bool_value() const;
  const std::string& string_value() const;

  // bool CheckHasKey(const std::string &key) const;
  // 
  // int Int(const std::string &key) const;
  // int Int(const std::string &key, int i) const;
  // 
  double Double(const std::string &key) const;
  // double Double(const std::string &key, int i) const;
  // 
  std::vector<double> Array1(const std::string &key, int nsize=-1) const;
  std::vector<std::vector<double>> Array2(const std::string &key) const;
  // std::vector<std::vector<std::vector<double>>> Array3(const std::string &key) const;

  std::vector<std::string> StringArray1(const std::string &key) const;
  std::string String(const std::string &key) const;
  
  const json11::Json& GetJson() const { return *CurrentData; }
};

#endif
