#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#define SDK_HEADER_FILEPATH "/usr/local/zed/include/sl/Camera.hpp"
#define EDGE_AGENT_VERSION_FILEPATH "/usr/local/sl_hub/versions/edge_agent_version.txt"

class Version
{
public:
    Version() = default;
    Version(int major, int minor, int patch)
        : major_(major), minor_(minor), patch_(patch)
    {}

    inline std::string to_string() const
    {
        std::stringstream ss;

        ss << major_ << "." << minor_ << "." << patch_;
        return ss.str();
    }

    friend std::ostream &operator<<(std::ostream &stream,
                                    const Version &version);

private:
    int major_ = 0;
    int minor_ = 0;
    int patch_ = 0;
};

Version getZedSDKVersion();
Version getHubSDKVersion();