#include "commands/handler.h"
#include "config/config.h"
#include "entity/manager.h"
#include "network/clientloop.h"
#include "platform/platform.h"
#include "recipes/crafting/recipe.h"
#include "runmanager/runmanager.h"
#include "world/world.h"

#include <iostream>
#include <sockpp/tcp_acceptor.h>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
  Platform::RegisterCtrlCHandler([]() {
    std::cout << "\r";
    RunManager::stop();
  });

  // spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing libraries...");
  sockpp::initialize();
  (void)accessConfig();
  (void)accessWorld();
  {
    auto& lvl    = accessConfig().getItem("logging.level");
    auto  spdlvl = spdlog::level::from_str(lvl.getValue<const char*>());
    if (spdlvl == spdlog::level::off) spdlog::warn("Further logging was disabled by config!");
    spdlog::set_level(spdlvl);
  }

  // All the built in recipes being initialized before the main() gets called
  spdlog::info("Loaded {} crafting recipes", CraftingRecipe::getCount());
  spdlog::info("Creating the connection acceptor server loop...");
  std::thread acceptThread([]() {
    Platform::SetCurrentThreadName("TCP acceptor");

    auto& port  = accessConfig().getItem("bind.port");
    auto& qsize = accessConfig().getItem("bind.queue_size");

    const auto nport = port.getValue<uint32_t>();

    if (nport != 25565) {
      spdlog::warn("Minecraft Beta 1.7.3 does not really support custom server"
                   "ports, you should change it only if you know what you're doing.");
    }

    sockpp::tcp_acceptor server(nport, qsize.getValue<uint32_t>());

    if (!server) {
      spdlog::error("Failed to spawn server instance: {}", server.last_error_str());
      RunManager::stop();
      return;
    } else {
      server.set_non_blocking(true);
      spdlog::info("Server now listening connections on *:{}", nport);
    }

    while (RunManager::isRunning()) {
      sockpp::inet_address addr;

      if (auto sock = server.accept(&addr)) {
        sock.set_non_blocking(false);
        spdlog::trace("New connection from {}", addr.to_string());

        if (!sock) {
          spdlog::error("Failed to accept incoming connection: ", server.last_error_str());
          continue;
        }

        ClientLoop reader(sock, addr);
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });

  std::thread console([]() {
    while (RunManager::isRunning()) {
      if (!std::cin) break;

      std::wstring command, out;
      std::getline(std::wcin, command);
      if (command.empty()) continue;

      if (!accessCommandHandler().execute(nullptr, command, out)) {
        out = std::format(L"Command execution failed: {}", out);
      }

      for (auto it = out.begin(); it != out.end();) {
        if (*it == L'\u00a7') {
          it = out.erase(it, it + std::min(ptrdiff_t(2), std::distance(it, out.end())));
          continue;
        }

        ++it;
      }

      // Sadly, there's no multiplatform way to print wide chars with spdlog
      if (!out.empty()) std::wcout << out << std::endl;
    }
  });
  console.detach();

  // We don't really need main thread to do something, so just waiting til acceptor finishes to destroy the rest
  acceptThread.join();
  spdlog::info("Finishing the World routines...");
  accessWorld().finish();
  spdlog::info("Finishing the EntityManager routines...");
  accessEntityManager().finish();
  spdlog::info("No issues found, closing the process now...");

  return 0;
}
