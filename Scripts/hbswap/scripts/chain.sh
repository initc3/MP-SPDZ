dataDir=./poa/data
keyStore=./poa/keystore/account_0

#$GOPATH/src/github.com/ethereum/go-ethereum/build/bin/geth --datadir $dataDir init ./Scripts/hbswap/poa/genesis.json
#geth --datadir $dataDir init ./Scripts/hbswap/poa/genesis.json
geth --datadir $dataDir init ./poa/genesis.json

#$GOPATH/src/github.com/ethereum/go-ethereum/build/bin/geth \
geth \
    --datadir $dataDir \
    --keystore $keyStore \
    --mine --allow-insecure-unlock --unlock 0 \
    --password ./poa/empty_password.txt \
    --rpc --rpcaddr 0.0.0.0 --rpccorsdomain '*' --rpcapi admin,debug,eth,miner,net,personal,shh,txpool,web3 \
    --ws --wsaddr 0.0.0.0 --wsorigins '*' --wsapi admin,debug,eth,miner,net,personal,shh,txpool,web3 \
    --syncmode full \
    --ipcpath "$dataDir/geth.ipc"
    #2>> $dataDir/geth.log &
