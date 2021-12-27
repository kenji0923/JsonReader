#include "JsonReader.hh"

#include "json11.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

/**
 * Constructor for parsing json file.
 */
JsonReader::JsonReader(std::string ReadFileName)
{
  std::string AllJson = "";
  std::string strBuf;
  std::string err;

  try {
    std::ifstream ifs(ReadFileName.c_str());

    if (!ifs) {
      err = "File " + ReadFileName;
      err += " was not found.";
      throw err;
    }

    while (ifs>> strBuf) {
      AllJson += strBuf;
    }

    Data.reset(new json11::Json(json11::Json::parse(AllJson, err)));
    CurrentData = Data.get();
    if (err != "") throw err;
  } catch (std::string err) {
    std::cerr << err << std::endl;
    exit(-1);
  }
}

/**
 * Copy constructor.
 */
JsonReader::JsonReader(const JsonReader& rh)
{ 
  Data = rh.Data;
  CurrentData = rh.CurrentData;
}

/**
 * Move constructor
 */
JsonReader::JsonReader(JsonReader&& rh)
{
  Data = std::move(rh.Data);
  CurrentData = rh.CurrentData;
}

/**
 * Insert operator.
 */
void JsonReader::operator= (const JsonReader& rh)
{
  Data = rh.Data;
  CurrentData = rh.CurrentData;
}


/**
 * Get reader for a item.
 */
JsonReader JsonReader::operator[] (const std::string &key) const
{
  JsonReader jsr(*this);
  jsr.CurrentData = &((*CurrentData)[key]);
  return jsr;
}

/**
 * Get reader for a item.
 */
JsonReader JsonReader::operator[] (size_t i) const
{
  JsonReader jsr(*this);
  jsr.CurrentData = &((*CurrentData)[i]);
  return jsr;
}

/**
 * Check value to have specified type.
 */
void JsonReader::ValueCheck(const std::string& key, const json11::Json &Content, json11::Json::Type type) const {
  if (Content.type() == type) {
  } else {
    std::stringstream code;
    if (Content.is_null()){
      code << "Unknown key \"" << key << "\" was called.";
    } else {
      code << "Unknown type \"" << Content.type() << "\" was found. Key \"" << key << "\" was assumed to be \"" << type << "\".";
    }
    throw code.str();
  }
}

bool JsonReader::is_null() const
{
  return CurrentData->is_null();
}

bool JsonReader::is_array() const
{
  return CurrentData->is_array();
}

bool JsonReader::is_object() const
{
  return CurrentData->is_object();
}

std::vector<JsonReader> JsonReader::array_items() const
{
  std::vector<JsonReader> arr;
  for (auto& data : CurrentData->array_items()) {
    JsonReader jsr(*this);
    jsr.CurrentData = &data;
    arr.push_back(jsr);
  }
  return arr;
}

std::map<std::string, JsonReader> JsonReader::object_items() const
{
  std::map<std::string, JsonReader> obj;
  for (auto& data : CurrentData->object_items()) {
    JsonReader jsr(*this);
    jsr.CurrentData = &data.second;
    obj.insert(std::make_pair(data.first, jsr));
  }
  return obj;
}

double JsonReader::number_value() const
{
  return CurrentData->number_value();
}

int JsonReader::int_value() const
{
  return CurrentData->int_value();
}

bool JsonReader::bool_value() const
{
  return CurrentData->bool_value();
}

const std::string& JsonReader::string_value() const
{
  return CurrentData->string_value();
}

/**
 * Check and get array of numbers. Check size of the array if nsize >= 0.
 */
std::vector<double> JsonReader::Array1(const std::string &key, int nsize) const
{
  std::vector<double> arr;

  try {
    auto Content = (*CurrentData)[key]["Value"];
    if (Content.is_null()) {
      Content = (*CurrentData)[key];
    }

    ValueCheck(key, Content, json11::Json::ARRAY);
    int size = Content.array_items().size();
    if (nsize >= 0) {
      if (nsize != size) {
        // throw std::string(Form("Key %s is an array with size of %d, but %d is required.", key.c_str(), size, nsize));
      }
    }
    arr.resize(size);

    for (int k = 0; k < size; ++k) {
      ValueCheck(key, Content[k], json11::Json::NUMBER);
      arr[k] = Content[k].number_value();
    }
  } catch (std::string err) {
    std::cerr << err << std::endl;
    exit(-1);
  }

  return arr;
}

double JsonReader::Double(const std::string &key) const
{
  auto Content = (*CurrentData)[key]["Value"];
  if (Content.is_null()) {
    Content = (*CurrentData)[key];
  }
  ValueCheck(key, Content, json11::Json::NUMBER);
  return Content.number_value();
}

std::vector<std::vector<double>> JsonReader::Array2(const std::string &key) const
{
  auto Content = (*CurrentData)[key]["Value"];
  if (Content.is_null()) {
    Content = (*CurrentData)[key];
  }

  std::vector<std::vector<double>> arr;

  ValueCheck(key, Content, json11::Json::ARRAY);
  int size = Content.array_items().size();
  arr.resize(size);

  for (int k = 0; k < size; ++k) {
    ValueCheck(key, Content[k], json11::Json::ARRAY);
    int size = Content[k].array_items().size();
    arr[k].resize(size);

    for (int l = 0; l < size; ++l) {
      ValueCheck(key, Content[k][l], json11::Json::NUMBER);
      arr[k][l] = Content[k][l].number_value();
    }
  }

  return arr;
}

std::vector<std::string> JsonReader::StringArray1(const std::string &key) const {
  auto Content = (*CurrentData)[key]["Value"];
  if (Content.is_null()) {
    Content = (*CurrentData)[key];
  }

  std::vector<std::string> arr;

  ValueCheck(key, Content, json11::Json::ARRAY);
  int size = Content.array_items().size();
  arr.resize(size);

  for (int k = 0; k < size; ++k) {
    ValueCheck(key, Content[k], json11::Json::STRING);
    arr[k] = Content[k].string_value();
  }

  return arr;
}

std::string JsonReader::String(const std::string &key) const
{
  auto Content = (*CurrentData)[key]["Value"];
  if (Content.is_null()) {
    Content = (*CurrentData)[key];
  }

  ValueCheck(key, Content, json11::Json::STRING);

  return Content.string_value();
}
