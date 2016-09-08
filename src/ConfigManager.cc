/**
  @author Thebluefish
  @license The MIT License (MIT)
  @copyright
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ConfigManager.h"
#include "ConfigFile.h"

#include <Urho3D/Urho3D.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>

//namespace blu
//{
ConfigManager::ConfigManager(Urho3D::Context* context, const Urho3D::String& defaultFileName, bool caseSensitive, bool saveDefaultParameters)
  : Urho3D::Object(context)
  , _defaultFileName(defaultFileName)
  , _caseSensitive(caseSensitive)
  , _saveDefaultParameters(saveDefaultParameters) {
  Load(); }

ConfigManager::~ConfigManager() {

}

void ConfigManager::RegisterObject(Urho3D::Context* context) {
  context->RegisterFactory<ConfigManager>(); }

// Check if value exists
bool ConfigManager::Has(const Urho3D::String& section, const Urho3D::String& parameter) {
  return Get(section, parameter) != Urho3D::Variant::EMPTY; }

// Set value
void ConfigManager::Set(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Variant& value) {
  SettingsMap* sectionMap = GetSection(section, true);

  sectionMap->operator[](parameter) = value; }

// Get value
const Urho3D::Variant ConfigManager::Get(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Variant& defaultValue) {
  SettingsMap* sectionMap = GetSection(section);

  // Section Doesn't exist
  if (!sectionMap) {
    return defaultValue; }

  // Section exists, parmeter doesn't exist
  if (sectionMap->Find(parameter) == sectionMap->End()) {
    return defaultValue; }

  // Section exists, parameter exists
  return (*sectionMap)[parameter]; }

const Urho3D::String ConfigManager::GetString(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::String& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_STRING) {
    return value.GetString(); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const int ConfigManager::GetInt(const Urho3D::String& section, const Urho3D::String& parameter, const int defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_INT
      && value.GetInt() != defaultValue) {
    return value.GetInt(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToInt(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const int ConfigManager::GetUInt(const Urho3D::String& section, const Urho3D::String& parameter, const unsigned defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_INT
      && value.GetUInt() != defaultValue) {
    return value.GetUInt(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToUInt(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const bool ConfigManager::GetBool(const Urho3D::String& section, const Urho3D::String& parameter, const bool defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_BOOL) {
    return value.GetBool(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToBool(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const float ConfigManager::GetFloat(const Urho3D::String& section, const Urho3D::String& parameter, const float defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_FLOAT) {
    return value.GetFloat(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToFloat(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Vector2 ConfigManager::GetVector2(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Vector2& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_VECTOR2) {
    return value.GetVector2(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToVector2(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Vector3 ConfigManager::GetVector3(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Vector3& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_VECTOR3) {
    return value.GetVector3(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToVector3(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Vector4 ConfigManager::GetVector4(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Vector4& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_VECTOR4) {
    return value.GetVector4(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToVector4(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Quaternion ConfigManager::GetQuaternion(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Quaternion& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_QUATERNION) {
    return value.GetQuaternion(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToQuaternion(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Color ConfigManager::GetColor(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Color& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_COLOR) {
    return value.GetColor(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToColor(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::IntRect ConfigManager::GetIntRect(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::IntRect& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_INTRECT) {
    return value.GetIntRect(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToIntRect(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::IntVector2 ConfigManager::GetIntVector2(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::IntVector2& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_INTVECTOR2) {
    return value.GetIntVector2(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToIntVector2(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Matrix3 ConfigManager::GetMatrix3(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Matrix3& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_MATRIX3) {
    return value.GetMatrix3(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToMatrix3(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Matrix3x4 ConfigManager::GetMatrix3x4(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Matrix3x4& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_MATRIX3X4) {
    return value.GetMatrix3x4(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToMatrix3x4(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

const Urho3D::Matrix4 ConfigManager::GetMatrix4(const Urho3D::String& section, const Urho3D::String& parameter, const Urho3D::Matrix4& defaultValue) {
  const Urho3D::Variant value = Get(section, parameter);

  if (value.GetType() == Urho3D::VAR_MATRIX4) {
    return value.GetMatrix4(); }

  if (value.GetType() == Urho3D::VAR_STRING) {
    return Urho3D::ToMatrix4(value.GetString()); }

  // Parameter doesn't exist, or is a different type
  if (_saveDefaultParameters) {
    // Set back to default
    Set(section, parameter, defaultValue); }

  return defaultValue; }

// Clears all settings
void ConfigManager::Clear() {
  _map.Clear(); }

// Load settings from file
bool ConfigManager::Load(const Urho3D::String& fileName, bool overwriteExisting) {
  const Urho3D::FileSystem* fileSystem = context_->GetSubsystem<Urho3D::FileSystem>();

  ConfigFile configFile(context_);

  // Check if file exists
  if (!fileSystem->FileExists(fileName)) {
    return false; }

  Urho3D::File file(context_, fileName, Urho3D::FILE_READ);
  configFile.BeginLoad(file);

  return Load(configFile, overwriteExisting); }

bool ConfigManager::Load(ConfigFile& configFile, bool overwriteExisting) {
  const ConfigMap* map = configFile.GetMap();

  SettingsMap* section = 0;
  for (Urho3D::Vector<ConfigSection>::ConstIterator itr = map->Begin(); itr != map->End(); itr++) {
    if (itr->Begin() == itr->End()) {
      continue; }

    Urho3D::String header = Urho3D::String::EMPTY;

    if (itr != map->Begin()) {
      header = *(itr->Begin());
      header = ConfigFile::ParseHeader(header); }

    const SettingsMap* section = GetSection(header, true);

    for (Urho3D::Vector<Urho3D::String>::ConstIterator section_itr = ++itr->Begin(); section_itr != itr->End(); section_itr++) {
      const Urho3D::String& line = *section_itr;

      Urho3D::String parameter;
      Urho3D::String value;

      ConfigFile::ParseProperty(line, parameter, value);

      if (parameter != Urho3D::String::EMPTY && value != Urho3D::String::EMPTY) {
        Set(header, parameter, value); } } }

  return true; }

// Save settings to file
bool ConfigManager::Save(const Urho3D::String& fileName, bool smartSave) {
  const Urho3D::FileSystem* fileSystem = GetSubsystem<Urho3D::FileSystem>();

  Urho3D::SharedPtr<ConfigFile> configFile(new ConfigFile(context_));

  if (smartSave) {
    Urho3D::SharedPtr<Urho3D::File> file(new Urho3D::File(context_, fileName, Urho3D::FILE_READ));

    // Ensure file is open
    if (file->IsOpen()) {
      configFile->BeginLoad(*file); } }

  // Attempt to save the file
  if (Save(*configFile)) {
    Urho3D::SharedPtr<Urho3D::File> file(new Urho3D::File(context_, fileName, Urho3D::FILE_WRITE));

    // Ensure file is open
    if (!file->IsOpen()) {
      return false; }

    configFile->Save(*file, smartSave); }

  return true; }


bool ConfigManager::Save(ConfigFile& configFile) {
  SaveSettingsMap("", _map, configFile);

  return true; }

void ConfigManager::SaveSettingsMap(Urho3D::String section, SettingsMap& map, ConfigFile& configFile) {
  // Save out parameters
  for (SettingsMap::Iterator itr = map.Begin(); itr != map.End(); itr++) {
    // Skip over sub-sections
    if (itr->second_.GetType() == Urho3D::VAR_VOIDPTR) {
      continue; }

    Urho3D::String value = itr->first_;

    // Set parameter
    if (itr->second_.GetType() == Urho3D::VAR_STRING) {
      configFile.Set(section, value, itr->second_.GetString()); }

    if (itr->second_.GetType() == Urho3D::VAR_INT) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetInt())); }

    if (itr->second_.GetType() == Urho3D::VAR_BOOL) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetBool())); }

    if (itr->second_.GetType() == Urho3D::VAR_FLOAT) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetFloat())); }

    if (itr->second_.GetType() == Urho3D::VAR_VECTOR2) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetVector2())); }

    if (itr->second_.GetType() == Urho3D::VAR_VECTOR3) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetVector3())); }

    if (itr->second_.GetType() == Urho3D::VAR_VECTOR4) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetVector4())); }

    if (itr->second_.GetType() == Urho3D::VAR_QUATERNION) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetQuaternion())); }

    if (itr->second_.GetType() == Urho3D::VAR_COLOR) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetColor())); }

    if (itr->second_.GetType() == Urho3D::VAR_INTRECT) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetIntRect())); }

    if (itr->second_.GetType() == Urho3D::VAR_INTVECTOR2) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetIntVector2())); }

    if (itr->second_.GetType() == Urho3D::VAR_MATRIX3) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetMatrix3())); }

    if (itr->second_.GetType() == Urho3D::VAR_MATRIX3X4) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetMatrix3x4())); }

    if (itr->second_.GetType() == Urho3D::VAR_MATRIX4) {
      configFile.Set(section, value, Urho3D::String(itr->second_.GetMatrix4())); } }

  // Save out sub-sections
  for (SettingsMap::ConstIterator itr = map.Begin(); itr != map.End(); itr++) {
    // Skip over parameter
    if (itr->second_.GetType() != Urho3D::VAR_VOIDPTR) {
      continue; }

    Urho3D::String path = section;
    path.Append(itr->first_);

    Urho3D::Variant fsdf = map[section];
    SettingsMap* value = static_cast<SettingsMap*>(itr->second_.GetVoidPtr());

    if (value) {
      // Save sub-section
      SaveSettingsMap(path, *value, configFile); } } }

SettingsMap* ConfigManager::GetSection(const Urho3D::String& section, bool create) {
  // Empty section gets assigned to main map
  if (section == Urho3D::String::EMPTY) {
    return &_map; }

  // Split section into submaps
  Urho3D::Vector<Urho3D::String> split;

  unsigned splitPos = 0;

  if (ConfigFile::ParseHeader(section).Empty()) {
    return &_map; }

  // Split sections by '.' or '/'
  // Comments will ignore splits behind them
  while (splitPos != Urho3D::String::NPOS) {
    // Find next comment split
    unsigned commentSplitPos = splitPos;
    unsigned hashPos = section.Find("#", commentSplitPos);
    unsigned slashesPos = section.Find("//", commentSplitPos);
    commentSplitPos = (hashPos < slashesPos) ? hashPos : slashesPos;

    // Find next split
    unsigned lastSplitPos = splitPos;
    unsigned dotPos = section.Find(".", lastSplitPos);
    unsigned slashPos = section.Find("/", lastSplitPos);
    splitPos = (dotPos < slashPos) ? dotPos : slashPos;

    // Ignore splits after comments
    splitPos = (commentSplitPos <= splitPos) ? Urho3D::String::NPOS : splitPos;

    int length = splitPos - lastSplitPos;
    if (splitPos == Urho3D::String::NPOS) {
      length = section.Length() - lastSplitPos; }

    Urho3D::String sub = section.Substring(lastSplitPos, length);

    if (sub != Urho3D::String::EMPTY) {
      split.Push(sub); } }

  SettingsMap* currentMap = &_map;
  for (Urho3D::Vector<Urho3D::String>::ConstIterator itr = split.Begin(); itr != split.End(); itr++) {
    Urho3D::String section = *itr;

    // Find section
    SettingsMap* newMap = 0;
    for (SettingsMap::ConstIterator map_itr = currentMap->Begin(); map_itr != currentMap->End(); map_itr++) {
      if (map_itr->first_ == section) {
        newMap = static_cast<SettingsMap*>(map_itr->second_.GetVoidPtr());

        // Key exists, but is not a SettingsMap
        if (!newMap) {
          return 0; }

        // Key exists
        break; } }

    // Key does not exist
    if (!newMap) {
      if (create) {
        currentMap->operator[](section) = new SettingsMap();
        newMap = static_cast<SettingsMap*>((*currentMap)[section].GetVoidPtr()); } }

    if (newMap) {
      currentMap = newMap; } }

  return currentMap; }
//}