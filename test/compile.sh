#!/bin/bash

make malicious-shamir-party.x
cp malicious-shamir-party.x /usr/local/bin/

make mal-shamir-offline.x
cp mal-shamir-offline.x /usr/local/bin/

make random-bits.x
cp random-bits.x /usr/local/bin/

make random-triples.x
cp random-triples.x /usr/local/bin/
#
#make random-shamir.x
#cp random-bits.x /usr/local/bin/
