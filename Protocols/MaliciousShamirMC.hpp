/*
 * MaliciousShamirMC.cpp
 *
 */

#include "MaliciousShamirMC.h"
#include "Machines/ShamirMachine.h"

template<class T>
MaliciousShamirMC<T>::MaliciousShamirMC()
{
    this->threshold = 2 * ShamirMachine::s().threshold;
}

template<class T>
void MaliciousShamirMC<T>::init_open(const Player& P, int n)
{
    int threshold = ShamirMachine::s().threshold;
    if (reconstructions.empty())
    {
        reconstructions.resize(2 * threshold + 2);
        for (int i = threshold + 1; i <= 2 * threshold + 1; i++)
        {
            int cnt = 0;
            for (int j = 0; j < i; j++, cnt++) {
                while (P.N.get_name(P.get_player(cnt)).empty()) cnt++;
            }
            reconstructions[i].resize(cnt);
            for (int j = 0; j < cnt; j++) {
                if (P.N.get_name(P.get_player(j)).empty()) continue;
                int _i = P.get_player(j);
                reconstructions[i][j] = 1;
                for (int k = 0; k < cnt; k++) {
                    if (P.N.get_name(P.get_player(k)).empty()) continue;
                    int other = positive_modulo(P.my_num() + k, P.num_players());
                    if (_i != other)
                        reconstructions[i][j] *= open_type(other + 1) / (open_type(other + 1) - open_type(_i + 1));
                }
            }
        }
    }

    ShamirMC<T>::init_open(P, n);
}

template<class T>
typename T::open_type MaliciousShamirMC<T>::finalize_raw()
{
    int threshold = ShamirMachine::s().threshold;
    shares.resize(2 * threshold + 1);
    int idx = 0, ofs = 0;
    for (size_t j = 0; j < shares.size(); j++) {
        do {
            idx = this->player->get_player(ofs);
            ++ofs;
        } while (this->player->N.get_name(idx).empty());
        shares[j].unpack((*this->os)[idx]);
    }
    return reconstruct(shares);
}

template<class T>
typename T::open_type MaliciousShamirMC<T>::reconstruct(
        const vector<open_type>& shares)
{
    int threshold = ShamirMachine::s().threshold;
    typename T::open_type value = 0;
    for (int j = 0, ofs = 0, idx = this->player->get_player(ofs); j < threshold + 1; j++, ofs++, idx = this->player->get_player(ofs)) {
        while (this->player->N.get_name(idx).empty()) {
            ofs++;
            idx = this->player->get_player(ofs);
        }
        value += shares[j] * reconstructions[threshold + 1][ofs];
    }
    for (size_t j = threshold + 2; j <= shares.size(); j++)
    {
        typename T::open_type check = 0;
        for (size_t k = 0, ofs = 0, idx = this->player->get_player(ofs); k < j; k++, ofs++, idx = this->player->get_player(ofs)) {
            while (this->player->N.get_name(idx).empty()) {
                ofs++;
                idx = this->player->get_player(ofs);
            }
            check += shares[k] * reconstructions[j][ofs];
        }
        if (check != value)
            throw mac_fail("inconsistent Shamir secret sharing");
    }
    return value;
}