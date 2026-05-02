#include <express/legacy/WebApp.h>
#include <express/middleware/StaticMiddleware.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

std::string getArgumentValue(int argc, char* argv[], const std::string& optionName, const std::string& defaultValue) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == optionName) {
            return argv[i + 1];
        }
    }

    return defaultValue;
}

} // namespace

int main(int argc, char* argv[]) {
    const std::string webRoot = getArgumentValue(argc, argv, "--web-root", "./www");

    if (!std::filesystem::exists(webRoot)) {
        std::cerr << "Web root does not exist: " << webRoot << '\n';
        return EXIT_FAILURE;
    }

    try {
        express::legacy::WebApp app("platformio-ini-generator");

        app.use(express::middleware::StaticMiddleware(webRoot));

        app.get("/health", [] APPLICATION(req, res) {
            res.send("OK");
        });

        std::cout << "PlatformIO INI generator web root: " << webRoot << '\n';
        std::cout << "Open the configured SNode.C HTTP endpoint in your browser." << '\n';

        app.listen();
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
