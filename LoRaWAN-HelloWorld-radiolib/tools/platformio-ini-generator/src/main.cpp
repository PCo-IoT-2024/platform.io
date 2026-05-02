#include "express/legacy/in/WebApp.h"
#include "express/middleware/StaticMiddleware.h"
#include "utils/Config.h"
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
    utils::Config::configRoot.newSubCommand<subcommand::ConfigWebRoot>();

    express::WebApp::init(argc, argv);

    const std::string webRoot = utils::Config::configRoot.getSubCommand<subcommand::ConfigWebRoot>()->getWebRoot();

    if (!std::filesystem::exists(webRoot)) {
        std::cerr << "Web root does not exist: " << webRoot << '\n';
        return 1;
    }

    using WebApp = express::legacy::in::WebApp;
    using SocketAddress = WebApp::SocketAddress;

    const WebApp app;

    app.use(express::middleware::StaticMiddleware(webRoot));

    app.get("/health", [] APPLICATION(req, res) {
        res.send("OK");
    });

    app.getConfig()->setReuseAddress();

    app.listen(8080,
               [instanceName = app.getConfig()->getInstanceName()](const SocketAddress& socketAddress, const core::socket::State& state) {
                   switch (state) {
                       case core::socket::State::OK:
                           std::cout << instanceName << " listening on '" << socketAddress.toString() << "'\n";
                           break;
                       case core::socket::State::DISABLED:
                           std::cout << instanceName << " disabled\n";
                           break;
                       case core::socket::State::ERROR:
                           std::cerr << instanceName << " " << socketAddress.toString() << ": " << state.what() << '\n';
                           break;
                       case core::socket::State::FATAL:
                           std::cerr << instanceName << " " << socketAddress.toString() << ": " << state.what() << '\n';
                           break;
                   }
               });

    std::cout << "PlatformIO INI generator web root: " << webRoot << '\n';

    return express::WebApp::start();
}
