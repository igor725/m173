#include "commands/command.h"
#include "config/config.h"
#include "network/clientloop.h"

#include <sockpp/tcp_acceptor.h>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
  // spdlog::set_level(spdlog::level::trace);
  spdlog::info("Initializing libraries...");
  sockpp::initialize();
  (void)accessCommandHandler();
  (void)accessConfig();
  spdlog::info("Loading config...");
  {
    auto& lvl    = accessConfig().getItem("logging.level");
    auto  spdlvl = spdlog::level::from_str(lvl.getValue<const char*>());
    if (spdlvl == spdlog::level::off) spdlog::warn("Further logging was disabled by config!");
    spdlog::set_level(spdlvl);
  }
  spdlog::info("Registering base commands...");
  {
    class Test: public Command {
  public:
      Test(): Command(L"test", L"Test message") {}

      bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
        (void)caller;
        (void)args;
        out = L"Yay! Test command!!!";
        return true;
      }
    };

    static Test reg;
  }

  auto& port  = accessConfig().getItem("bind.port");
  auto& qsize = accessConfig().getItem("bind.queue_size");

  const auto nport = port.getValue<uint32_t>();

  sockpp::tcp_acceptor server(nport, qsize.getValue<uint32_t>());

  if (!server) {
    spdlog::error("Failed to spawn server instance {}", server.last_error_str());
    return 1;
  } else {
    spdlog::info("Server now listening connections on *:{}", nport);
  }

  spdlog::info("Starting the main server loop...");

  while (true) {
    sockpp::inet_address addr;

    sockpp::tcp_socket sock = server.accept(&addr);
    spdlog::trace("New connection from {}", addr.to_string());

    if (!sock) {
      spdlog::error("Failed to accept incoming connection: ", server.last_error_str());
      continue;
    }

    ClientLoop reader(sock, addr);
  }

  spdlog::info("Closing...");

  return 0;
}
