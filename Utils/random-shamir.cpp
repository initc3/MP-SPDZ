/*
 * Shamir-based random share generator.
 *
 * Based on paper-example.cpp
 *
 */

#define NO_MIXED_CIRCUITS

// FIXME must appear first
#include "Machines/SPDZ.hpp"

#include "Machines/MalRep.hpp"
#include "Machines/ShamirMachine.hpp"
#include "Protocols/ProtocolSet.h"

using namespace ez;

template <class T>
void run(ez::ezOptionParser &opt, bigint prime, int prime_length);

void Usage(ez::ezOptionParser &opt) {
  string usage;
  opt.getUsage(usage);
  cout << usage;
};

int main(int argc, const char **argv) {
    Timer timer;
    timer.start();

  auto &opts = ShamirOptions::singleton;
  ez::ezOptionParser opt;
  opts = {opt, argc, argv};

  opt.overview = "Generator of random shares using Shamir Secret Sharing.";
  opt.syntax = "./random-shamir.x [OPTIONS]\n";
  opt.example = "./random-shamir.x -i 0 -N 4 -T 1 --nshares 10000 --host "
                "darkmatter.io --port 9999 \n";

  opt.add("", 0, 0, 0, "Display usage instructions.", "-h", "--help");

  opt.add("", 1, 1, 0, "Player number.", "-i", "--playerno");

  opt.add("20000", 0, 1, 0, "Number of shares to generate (default: 20000).",
          "-s", "--nshares");

  opt.add("52435875175126190479447740508185965837690552500527637822603658699938"
          "581184513",
          0, 1, 0, "Prime field (default: BLS12-381 prime field \
            '52435875175126190479447740508185965837690552500527637822603658699938581184513').",
          "-P", "--prime");

  opt.add("localhost", 0, 1, 0,
          "Hostname of MPC server (default: 'localhost').", "--host");

  opt.add("9999", 0, 1, 0, "Port number.", "-p", "--port");

  opt.add(PREP_DIR, 0, 1, 0, "Preprocessing directory (default: 'Player-Data')",
          "--prep-dir");

  opt.add("malshamir", 0, 1, 0,
          "Protocol (shamir or malshamir) (default: 'malshamir')",
          "--protocol");

  opt.parse(argc, argv);

  if (opt.isSet("-h")) {
    Usage(opt);
    return 1;
  }

  if (!opt.isSet("-i")) {
    Usage(opt);
    exit(0);
  }

  string prime, protocol;
  opt.get("--prime")->getString(prime);
  opt.get("--protocol")->getString(protocol);

  // TODO compute prime length from given prime
  const int prime_length = 256;
  const int n_limbs = (prime_length + 63) / 64;

  if (protocol == "shamir")
    run<ShamirShare<gfp_<0, n_limbs>>>(opt, bigint(prime), prime_length);
  else if (protocol == "malshamir")
    run<MaliciousShamirShare<gfp_<0, n_limbs>>>(opt, bigint(prime),
                                                prime_length);
  else {
    cerr << "Unsupported protocol: " << protocol << endl;
    cerr << "Use shamir or malshamir" << endl;
    exit(1);
  }
    cerr << "total_time " << timer.elapsed() << endl;
}

template <class T>
void run(ez::ezOptionParser &opt, bigint prime, int prime_length) {
  int playerno, nparties, nshares, port;
  string hostname, prep_dir;
  opt.get("--playerno")->getInt(playerno);
  opt.get("--nparties")->getInt(nparties);
  opt.get("--nshares")->getInt(nshares);
  opt.get("--host")->getString(hostname);
  opt.get("--port")->getInt(port);
  opt.get("--prep-dir")->getString(prep_dir);

  Names names(playerno, nparties, hostname, port);
  CryptoPlayer player(names);

  cout << "prime length: " << prime_length << endl;
  ProtocolSetup<T> setup(prime, player);

  ProtocolSet<T> set(player, setup);

  auto &preprocessing = set.preprocessing;

  stringstream ss;
  ofstream outputFile;
  string prep_data_dir = get_prep_sub_dir<T>(prep_dir, player.num_players());
  ss << prep_data_dir << "Randoms-" << T::type_short() << "-P"
     << player.my_num();
  outputFile.open(ss.str().c_str());

  int ntriples = nshares / 2 + nshares % 2;
  vector<T> Sa(ntriples), Sb(ntriples), Sc(ntriples);
preprocessing.buffer_size = nshares;
int nused = 1;
  for (int i = 0; i < nused; i++) {
    preprocessing.get_three(DATA_TRIPLE, Sa[i], Sb[i], Sc[i]);
    Sa[i].output(outputFile, true);
    if (i == ntriples - 1 && nshares % 2)
      break;
    outputFile << "\n";
    Sb[i].output(outputFile, true);
    if (i != ntriples - 1)
      outputFile << "\n";
  }

  cout << "\nDONE!" << endl;

  T::LivePrep::teardown();
}
