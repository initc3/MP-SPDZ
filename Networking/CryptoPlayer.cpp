/*
 * CryptoPlayer.cpp
 *
 */

#include "CryptoPlayer.h"
#include "Math/Setup.h"
#include "Tools/Bundle.h"

void check_ssl_file(string filename)
{
    if (not ifstream(filename))
        throw runtime_error("Cannot access " + filename
                        + ". Have you set up SSL?\n"
                        "You can use `Scripts/setup-ssl.sh <nparties>`.");
}

void ssl_error(string side, string other, string me)
{
    cerr << side << "-side handshake with " << other
            << " failed. Make sure both sides "
            << " have the necessary certificate (" << SSL_DIR << me
            << ".pem in the default configuration on their side and "
            << SSL_DIR << other << ".pem on ours),"
            << " and run `c_rehash <directory>` on its location." << endl
            << "The certificates should be the same on every host. "
            << "Also make sure that it's still valid. Certificates generated "
            << "with `Scripts/setup-ssl.sh` expire after a month." << endl;
    cerr << "See also "
            "https://mp-spdz.readthedocs.io/en/latest/troubleshooting.html"
            "#handshake-failures" << endl;

    string ids[2];
    ids[side == "Client"] = other;
    ids[side != "Client"] = me;
    cerr << "Signature (should match the other side): ";
    for (int i = 0; i < 2; i++)
    {
        auto filename = SSL_DIR + ids[i] + ".pem";
        ifstream cert(filename);
        stringstream buffer;
        buffer << cert.rdbuf();
        if (buffer.str().empty())
            cerr << "<'" << filename << "' not found>";
        else
            cerr << octetStream(buffer.str()).hash();
        if (i == 0)
            cerr << "/";
    }
    cerr << endl;
}

void new_ssl_socket(vector<ssl_socket*>& sockets, int i,
    boost::asio::io_service& io_service,
    boost::asio::ssl::context& ctx,
    int plaintext_socket, string other, string me, bool client)
{
    sockets[i] = new ssl_socket(io_service, ctx, plaintext_socket, other, me, client);
}

void CryptoPlayer::create_plain_player(const Names& Nms, const string& id_base, int i, vector<int>& plaintext_sockets, int my_num) {
    PlainPlayer player(Nms, id_base + (i ? "recv" : ""));
    plaintext_sockets = player.sockets;
    close_client_socket(player.socket(my_num));
    player.sockets.clear();
}

CryptoPlayer::CryptoPlayer(const Names& Nms, const string& id_base) :
        MultiPlayer<ssl_socket*>(Nms),
        ctx("P" + to_string(my_num()))
{
    sockets.resize(num_players());
    other_sockets.resize(num_players());
    senders.resize(num_players());
    receivers.resize(num_players());

    vector<int> plaintext_sockets[2];

//    for (int i = 0; i < 2; i++)
//    {
//        PlainPlayer player(Nms, id_base + (i ? "recv" : ""));
//        plaintext_sockets[i] = player.sockets;
//        close_client_socket(player.socket(my_num()));
//        player.sockets.clear();
//    }

    vector<thread> threads;

    for (int i = 0; i < 2; i++)
    {
        threads.push_back(thread(&CryptoPlayer::create_plain_player, ref(Nms), id_base, i, std::ref(plaintext_sockets[i]), my_num()));
    }

    for (int i = 0; i < int(threads.size()); i++) {
        threads[i].join();
    }

//    for (int offset = 1; offset <= num_players() / 2; offset++)
//    {
//        int others[] = { get_player(offset), get_player(-offset) };
//        if (my_num() % (2 * offset) < offset)
//            swap(others[0], others[1]);
//
//        if (num_players() % 2 == 0 and offset == num_players() / 2)
//            connect(others[0], plaintext_sockets);
//        else
//            for (int i = 0; i < 2; i++)
//                connect(others[i], plaintext_sockets);
//    }

    threads.clear();

    for (int offset = 1; offset <= num_players() / 2; offset++)
    {
        int others[] = { get_player(offset), get_player(-offset) };
        if (my_num() % (2 * offset) < offset)
            swap(others[0], others[1]);

        if (num_players() % 2 == 0 and offset == num_players() / 2) {
            int i = others[0];
            if (N.get_name(i).empty()) continue;
            threads.push_back(thread(new_ssl_socket, ref(sockets), i, ref(io_service), ref(ctx), plaintext_sockets[0][i],
                    "P" + to_string(i), "P" + to_string(my_num()), i < my_num()));
            threads.push_back(thread(new_ssl_socket, ref(other_sockets), i, ref(io_service), ref(ctx), plaintext_sockets[1][i],
                    "P" + to_string(i), "P" + to_string(my_num()), i < my_num()));
        }
        else
            for (int j = 0; j < 2; j++) {
                int i = others[j];
                if (N.get_name(i).empty()) continue;
                threads.push_back(thread(new_ssl_socket, ref(sockets), i, ref(io_service), ref(ctx), plaintext_sockets[0][i],
                        "P" + to_string(i), "P" + to_string(my_num()), i < my_num()));
                threads.push_back(thread(new_ssl_socket, ref(other_sockets), i, ref(io_service), ref(ctx), plaintext_sockets[1][i],
                        "P" + to_string(i), "P" + to_string(my_num()), i < my_num()));
            }
    }

    for (int i = 0; i < int(threads.size()); i++) {
        threads[i].join();
    }

    for (int i = 0; i < num_players(); i++)
    {
        if (i == my_num())
        {
            sockets[i] = 0;
            senders[i] = 0;
            other_sockets[i] = 0;
            receivers[i] = 0;
            continue;
        }
        if (N.get_name(i).empty()) continue;

        senders[i] = new Sender<ssl_socket*>(i < my_num() ? sockets[i] : other_sockets[i]);
        receivers[i] = new Receiver<ssl_socket*>(i < my_num() ? other_sockets[i] : sockets[i]);
//        senders[i] = new Sender<ssl_socket*>(i < my_num() ? sockets[i] : other_sockets[i], "P" + to_string(my_num()), "P" + to_string(i));
//        receivers[i] = new Receiver<ssl_socket*>(i < my_num() ? other_sockets[i] : sockets[i], "P" + to_string(i), "P" + to_string(my_num()));
    }
}

void CryptoPlayer::connect(int i, vector<int>* plaintext_sockets)
{
    if (N.get_name(i).empty()) return;
    sockets[i] = new ssl_socket(io_service, ctx, plaintext_sockets[0][i],
            "P" + to_string(i), "P" + to_string(my_num()), i < my_num());
    other_sockets[i] = new ssl_socket(io_service, ctx, plaintext_sockets[1][i],
            "P" + to_string(i), "P" + to_string(my_num()), i < my_num());

}

CryptoPlayer::CryptoPlayer(const Names& Nms, int id_base) :
        CryptoPlayer(Nms, to_string(id_base))
{
}

CryptoPlayer::~CryptoPlayer()
{
    for (int i = 0; i < num_players(); i++)
    {
        delete sockets[i];
        delete other_sockets[i];
        delete senders[i];
        delete receivers[i];
    }
}

void CryptoPlayer::send_to_no_stats(int other, const octetStream& o) const
{
    assert(other != my_num());
    senders[other]->request(o);
    senders[other]->wait(o);
}

void CryptoPlayer::receive_player_no_stats(int other, octetStream& o) const
{
    assert(other != my_num());
    receivers[other]->request(o);
    receivers[other]->wait(o);
}

void CryptoPlayer::exchange_no_stats(int other, const octetStream& to_send,
        octetStream& to_receive) const
{
    assert(other != my_num());
    if (&to_send == &to_receive)
    {
        MultiPlayer<ssl_socket*>::exchange_no_stats(other, to_send, to_receive);
    }
    else
    {
        senders[other]->request(to_send);
        receivers[other]->request(to_receive);
        senders[other]->wait(to_send);
        receivers[other]->wait(to_receive);
    }
}

void CryptoPlayer::pass_around_no_stats(const octetStream& to_send,
        octetStream& to_receive, int offset) const
{
    assert(get_player(offset) != my_num());
    if (&to_send == &to_receive)
    {
        MultiPlayer<ssl_socket*>::pass_around_no_stats(to_send, to_receive, offset);
    }
    else
    {
#ifdef TIME_ROUNDS
        Timer recv_timer;
        TimeScope ts(recv_timer);
#endif
        senders[get_player(offset)]->request(to_send);
        receivers[get_player(-offset)]->request(to_receive);
        senders[get_player(offset)]->wait(to_send);
        receivers[get_player(-offset)]->wait(to_receive);
#ifdef TIME_ROUNDS
        cout << "Exchange time: " << recv_timer.elapsed() << " seconds to receive "
            << 1e-3 * to_receive.get_length() << " KB" << endl;
#endif
    }
}

void CryptoPlayer::send_receive_all_no_stats(const vector<vector<bool>>& channels,
        const vector<octetStream>& to_send,
        vector<octetStream>& to_receive) const
{
    to_receive.resize(num_players());
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        // TODO: send to offline nodes
        bool receive = channels[other][my_num()];
        if (channels[my_num()][other])
            this->senders[other]->request(to_send[other]);
        if (receive)
            this->receivers[other]->request(to_receive[other]);
    }
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        // TODO: send to offline nodes
        bool receive = channels[other][my_num()];
        if (channels[my_num()][other])
            this->senders[other]->wait(to_send[other]);
        if (receive)
            this->receivers[other]->wait(to_receive[other]);
    }
}

void CryptoPlayer::partial_broadcast(const vector<bool>& my_senders,
        const vector<bool>& my_receivers,
        vector<octetStream>& os) const
{
    TimeScope ts(comm_stats["Partial broadcasting"].add(os[my_num()]));
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        bool receive = my_senders[other];
        if (my_receivers[other])
        {
            this->senders[other]->request(os[my_num()]);
            sent += os[my_num()].get_length();
        }
        if (receive)
            this->receivers[other]->request(os[other]);
    }
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        bool receive = my_senders[other];
        if (my_receivers[other])
            this->senders[other]->wait(os[my_num()]);
        if (receive)
            this->receivers[other]->wait(os[other]);
    }
}

void CryptoPlayer::Broadcast_Receive_no_stats(vector<octetStream>& os) const
{
    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        this->senders[other]->request(os[my_num()]);
        receivers[other]->request(os[other]);
    }

    for (int offset = 1; offset < num_players(); offset++)
    {
        int other = get_player(offset);
        if (N.get_name(other).empty()) continue;
        this->senders[other]->wait(os[my_num()]);
        receivers[other]->wait(os[other]);
    }
}
