#include "apps/http/model/servers.h"
#include "express/middleware/StaticMiddleware.h"
#include "utils/SubCommand.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace subcommand {

    class ConfigWebRoot : public utils::SubCommand {
    public:
        constexpr static std::string_view NAME{"www"};
        constexpr static std::string_view DESCRIPTION{"Web behavior of platformio-ini-generator"};

        explicit ConfigWebRoot(utils::SubCommand* parent)
            : utils::SubCommand(parent, this, "Applications") {
            webRootOpt = addOption("--web-root", "Web root directory", "directory", std::string("./www"), CLI::ExistingDirectory);
        }

        std::string getWebRoot() const {
            return webRootOpt->as<std::string>();
        }

    private:
        CLI::Option* webRootOpt = nullptr;
    };

} // namespace subcommand

int main(int argc, char* argv[]) {
    using WebApp = apps::http::STREAM::WebApp;

    const WebApp webApp(apps::http::STREAM::getWebApp("platformio-ini-generator"));

    webApp.getConfig()->Instance::newSubCommand<subcommand::ConfigWebRoot>();

    WebApp::init(argc, argv);

    const std::string webRoot = webApp.getConfig()->Instance::getSubCommand<subcommand::ConfigWebRoot>()->getWebRoot();

    if (!std::filesystem::exists(webRoot)) {
        std::cerr << "Web root does not exist: " << webRoot << '\n';
        return 1;
    }

    webApp.use(express::middleware::StaticMiddleware(webRoot));

    webApp.get("/health", [] APPLICATION(req, res) {
        res.send("OK");
    });

    std::cout << "PlatformIO INI generator web root: " << webRoot << '\n';
    std::cout << "Open the configured SNode.C HTTP endpoint in your browser." << '\n';

    webApp.listen();

    return WebApp::start();
}
