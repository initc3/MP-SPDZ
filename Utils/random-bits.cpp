/*
 * Shamir-based random bit share generator.
 *
 */

#define NO_MIXED_CIRCUITS

#include "Math/gfp.hpp"
#include "Protocols/MaliciousRepPrep.hpp"
#include "Machines/ShamirMachine.hpp"

#include "Tools/ezOptionParser.h"

using namespace ez;

int generate(ezOptionParser& opt, int nparties);

void Usage(ezOptionParser& opt) {
	string usage;
	opt.getUsage(usage);
	cout << usage;
};

int main(int argc, const char** argv)
{
    auto& opts = ShamirOptions::singleton;
    ezOptionParser opt;
    opts = {opt, argc, argv};

    opt.overview = "Generator of random bit shares using Shamir Secret Sharing.";
    opt.syntax = "./random-bits.x [OPTIONS]\n";
    opt.example = "./random-bits.x -i 0 -N 4 -T 1 --nshares 10000 --host darkmatter.io --port 9999 \n";

    opt.add(
		"", // Default.
		0, // Required?
		0, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Display usage instructions.", // Help description.
		"-h",     // Flag token. 
		"-help",  // Flag token.
		"--help", // Flag token.
		"--usage" // Flag token.
	);

    opt.add(
		"", // Default.
		1, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Player number.", // Help description.
		"-i", // Flag token.
		"--playerno" // Flag token.
	);

    opt.add(
		"20000", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Number of shares to generate (default: 20000).", // Help description.
		"-s", // Flag token.
		"--nshares" // Flag token.
	);

    opt.add(
		"52435875175126190479447740508185965837690552500527637822603658699938581184513", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Prime field (default: BLS12-381 prime field \
            '52435875175126190479447740508185965837690552500527637822603658699938581184513').", // Help description.
		"-P",
		"--prime"
	);

    opt.add(
		"localhost", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Hostname of MPC server (default: 'localhost').", // Help description.
		"--host"
	);

    opt.add(
		"9999", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Port number.", // Help description.
		"-p", // Flag token.
		"--port" // Flag token.
	);

    opt.add(
          PREP_DIR, // Default.
          0, // Required?
          1, // Number of args expected.
          0, // Delimiter if expecting multiple args.
          "Preprocessing directory (default: 'Player-Data')", // Help description.
          "--prep-dir" // Flag token.
    );

    opt.parse(argc, argv);

	if (opt.isSet("-h")) {
		Usage(opt);
		return 1;
	}

    if (!opt.isSet("-i"))
    {
		Usage(opt);
        exit(0);
    }
    cout << "opts threshold: " << opts.threshold << endl;
    cout << "opts nparties: " << opts.nparties << endl;
    return generate(opt, opts.nparties);
}


int generate(ezOptionParser& opt, int nparties)
{
    // needed because of bug in some gcc versions < 9
    // https://gitter.im/MP-SPDZ/community?at=5fcadf535be1fb21c5fce581
    //bigint::init_thread();

    // bit length of prime
    const int prime_length = 256;

    // compute number of 64-bit words needed
    const int n_limbs = (prime_length + 63) / 64;
    typedef MaliciousShamirShare<gfp_<0, n_limbs>> T;

    //int playerno, nparties, nshares, port;
    int playerno, nshares, port;
    string hostname, prime, prep_dir;
    opt.get("--playerno")->getInt(playerno);
    //opt.get("--nparties")->getInt(nparties);
    opt.get("--nshares")->getInt(nshares);
    opt.get("--prime")->getString(prime);
    opt.get("--host")->getString(hostname);
    opt.get("--port")->getInt(port);
    opt.get("--prep-dir")->getString(prep_dir);

    Names N;
    Server::start_networking(N, playerno, nparties, hostname, port);
    CryptoPlayer P(N);

    // initialize field
    //gfp_::init_field(bigint(prime));
    T::clear::init_field(bigint(prime));
    // TODO: not sure if the BLS prime should also be set on the `next`
    //T::clear::next::init_default(prime_length, false);
    T::clear::next::init_field(bigint(prime), false);

    // must initialize MAC key for security of some protocols
    typename T::mac_key_type mac_key;
    T::read_or_generate_mac_key("", P, mac_key);

    // TODO check if really needed
    // global OT setup
    BaseMachine machine;
    if (T::needs_ot)
        machine.ot_setups.push_back({P});

    // TODO check if really needed
    // keeps tracks of preprocessing usage
    DataPositions usage;
    usage.set_num_players(P.num_players());

    // output protocol
    typename T::MAC_Check output(mac_key);

    // preprocessing
    typename T::LivePrep preprocessing(0, usage);
    SubProcessor<T> processor(output, preprocessing, P);

    stringstream ss;
    ofstream outputFile;
    string prep_data_dir = get_prep_sub_dir<T>(prep_dir, P.num_players());
    ss << prep_data_dir << "Randoms-" << T::type_short() << "-P" << P.my_num();
    outputFile.open(ss.str().c_str());

    int nbits = nshares / 2 + nshares % 2;
    vector<T> bits(nbits);
    for (int i=0; i < nbits; i++)
    {
        bits[i] = preprocessing.get_bit();
        bits[i].output(outputFile, true);
        if (i != nbits - 1)
            outputFile << "\n";
    }

    cout << "\nDONE!" << endl;

    T::LivePrep::teardown();
    return 0;
}
