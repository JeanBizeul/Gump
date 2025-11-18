#pragma once

#include <string>

namespace Gump {

namespace Utils {

std::string openFilePickerDialog(const std::string& title = "Select a file",
    const std::string& filter = "*.*");

}
}
