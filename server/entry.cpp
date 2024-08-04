#include "network/reader.h"

#include <sockpp/tcp_acceptor.h>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
  // spdlog::set_level(spdlog::level::info);
  spdlog::info("Initializing sockpp...");
  sockpp::initialize();

  sockpp::tcp_acceptor server {25565, 4};

  if (!server) {
    spdlog::error("Failed to spawn server instance {}", server.last_error_str());
    return 1;
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

    CreateReader reader(sock, addr);
  }

  spdlog::info("Closing...");

  return 0;
}
