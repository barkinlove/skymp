#include "FormDesc.h"
#include <cstdio>

std::string FormDesc::ToString(char delimiter) const
{
  char buffer[32];
  if (!file.empty()) {
    std::sprintf(buffer, "%0x%c%s", shortFormId, delimiter, file.c_str());
  } else {
    std::sprintf(buffer, "%0x", shortFormId);
  }
  return buffer;
}

FormDesc FormDesc::FromString(std::string str, char delimiter)
{
  for (auto& ch : str) {
    if (ch == delimiter) {
      ch = ' ';
    }
  }

  FormDesc res;
  char buffer[32];
  std::sscanf(str.c_str(), "%x %s", &res.shortFormId, buffer);
  res.file = std::move(buffer);
  return res;
}

uint32_t FormDesc::ToFormId(const std::vector<std::string>& files) const
{
  // Workaround legacy tests throwing exceptions (drop support for PartOne
  // instances without espm to remove this)
  static const std::string kSkyrimEsm = "Skyrim.esm";
  if (shortFormId == 0x3c && file == kSkyrimEsm) {
    return 0x3c;
  }

  uint32_t realFormId;
  if (file.empty()) {
    realFormId = 0xff000000 + shortFormId;
  } else {
    int fileIdx = -1;
    int numFiles = static_cast<int>(files.size());
    for (int i = 0; i < numFiles; ++i) {
      if (files[i] == file) {
        fileIdx = i;
        break;
      }
    }
    if (fileIdx == -1) {
      throw std::runtime_error(file + " not found in loaded files");
    }

    realFormId = fileIdx * 0x01000000 + shortFormId;
  }
  return realFormId;
}

FormDesc FormDesc::FromFormId(uint32_t formId,
                              const std::vector<std::string>& files)
{
  // Workaround legacy tests throwing exceptions (drop support for PartOne
  // instances without espm to remove this)
  if (formId == 0x3c) {
    return FormDesc::Tamriel();
  }

  FormDesc res;
  if (formId < 0xff000000) {
    int fileIdx = formId / 0x01000000;
    if (fileIdx >= static_cast<int>(files.size())) {
      throw std::runtime_error("FromFormId failed due to invalid file index " +
                               std::to_string(fileIdx));
    }
    res.file = files[fileIdx];
    res.shortFormId = formId % 0x01000000;
  } else {
    res.shortFormId = formId - 0xff000000;
  }
  return res;
}

static const FormDesc s_tamriel = FormDesc::FromString("3c:Skyrim.esm");

FormDesc FormDesc::Tamriel()
{
  return s_tamriel;
}
