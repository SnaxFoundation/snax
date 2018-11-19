The following steps must be taken for the example script to work.

0. Create wallet
0. Create account for snax.token
0. Create account for scott
0. Create account for exchange
0. Set token contract on snax.token
0. Create SNAX token
0. Issue initial tokens to scott

**Note**:
Deleting the `transactions.txt` file will prevent replay from working.


### Create wallet
`clisnax wallet create`

### Create account steps
`clisnax create key`

`clisnax create key`

`clisnax wallet import  --private-key <private key from step 1>`

`clisnax wallet import  --private-key <private key from step 2>`

`clisnax create account snax <account_name> <public key from step 1> <public key from step 2>`

### Set contract steps
`clisnax set contract snax.token /contracts/snax.token -p snax.token@active`

### Create SNAX token steps
`clisnax push action snax.token create '{"issuer": "snax.token", "maximum_supply": "100000.0000 SNAX", "can_freeze": 1, "can_recall": 1, "can_whitelist": 1}' -p snax.token@active`

### Issue token steps
`clisnax push action snax.token issue '{"to": "scott", "quantity": "900.0000 SNAX", "memo": "testing"}' -p snax.token@active`
