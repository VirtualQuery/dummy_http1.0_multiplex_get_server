#include "opt.hh"
#include "server.hh"

int main(int argc, char* argv[]) {
	const auto params = opt::parse_cli_params(argc, argv);
	server::run<5000>(params);
	return 0;
}
