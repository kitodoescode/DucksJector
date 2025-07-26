#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>
#include <C:\Users\leonc\Desktop\Injector - AutoImports\nlohmann.hpp>
#include <cctype>

using json = nlohmann::json;

struct TokenInfo {
    std::string token;
    long createdAt = 0;
    long expiresAfter = 0;
};

struct TokenValidationResult {
    bool valid = false;
    bool deleted = false;
    TokenInfo info;
};

class WorkInk {
private:
    static std::string decodeApiKey(const std::string& encoded) {
        std::string result;
        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++)
            T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val = 0, valb = -8;
        for (unsigned char c : encoded) {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                result.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return result;
    }
    static inline std::string _apiKey = decodeApiKey("MGU1NjQxMzItYzc2OC00MTA1LWFkMzQtN2I4NDI1NjA1MTIy");



    static std::string buildQuery(bool deleteToken, bool forbiddenOnFail) {
        std::ostringstream oss;
        bool first = true;
        if (deleteToken) {
            oss << (first ? "?" : "&") << "deleteToken=1";
            first = false;
        }
        if (forbiddenOnFail) {
            oss << (first ? "?" : "&") << "forbiddenOnFail=1";
        }
        return oss.str();
    }

public:
    static std::string getTokenUrl() {
        return "https://work.ink/token/" + _apiKey;
    }

    // Simulated version — replace this with your HTTP call
    static std::string simulateHttpGet(const std::string& url) {
        // Simulated JSON response
        return R"({
            "valid": true,
            "deleted": false,
            "info": {
                "token": "abc123",
                "createdAt": 1714387200,
                "expiresAfter": 3600
            }
        })";
    }

    static TokenValidationResult validateToken(const std::string& token, bool deleteToken = false, bool forbiddenOnFail = false) {
        std::string query = buildQuery(deleteToken, forbiddenOnFail);
        std::string url = "https://work.ink/_api/v2/token/isValid/" + token + query;

        std::string response = simulateHttpGet(url); // <-- Replace with real HTTP request if needed

        auto j = json::parse(response);

        TokenValidationResult result;
        result.valid = j.value("valid", false);
        result.deleted = j.value("deleted", false);

        if (j.contains("info")) {
            result.info.token = j["info"].value("token", "");
            result.info.createdAt = j["info"].value("createdAt", 0L);
            result.info.expiresAfter = j["info"].value("expiresAfter", 0L);
        }

        return result;
    }
};
