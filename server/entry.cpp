#include "commands/handler.h"
#include "config/config.h"
#include "entity/manager.h"
#include "network/clientloop.h"
#include "platform/platform.h"
#include "runmanager.h"
#include "world/world.h"

#include <sockpp/tcp_acceptor.h>
#include <spdlog/spdlog.h>

std::atomic<bool> g_isServerRunning = true;

int main(int argc, char* argv[]) {
  Platform::RegisterCtrlCHandler([]() {
    // todo move kicking to another function to avoid this fuckery
    std::wstring out;
    accessCommandHandler().execute(nullptr, L"/stop", out);
  });

  // spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing libraries...");
  sockpp::initialize();
  (void)accessConfig();
  (void)accessWorld();
  spdlog::info("Loading config...");
  {
    auto& lvl    = accessConfig().getItem("logging.level");
    auto  spdlvl = spdlog::level::from_str(lvl.getValue<const char*>());
    if (spdlvl == spdlog::level::off) spdlog::warn("Further logging was disabled by config!");
    spdlog::set_level(spdlvl);
  }

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
      g_isServerRunning = false;
      return;
    } else {
      server.set_non_blocking(true);
      spdlog::info("Server now listening connections on *:{}", nport);
    }

    while (g_isServerRunning) {
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

  spdlog::info("Starting the main program loop...");
  while (g_isServerRunning) {
    // todo handle server commands
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  spdlog::info("Finishing TCP acceptations...");
  acceptThread.join();
  spdlog::info("Finishing World routines...");
  accessWorld().finish();
  spdlog::info("Finishing EntityManager routines...");
  accessEntityManager().finish();
  spdlog::info("No issues found, closing process now...");

  return 0;
}
