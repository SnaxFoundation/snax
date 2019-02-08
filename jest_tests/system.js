const { sleep, tryCatchExpect } = require("./helpers");

const fetch = require("node-fetch");
const snaxjs = require("@snaxfoundation/snaxjs");

const { promisify } = require("util");
const child_process = require("child_process");

const [exec, execFile, spawn] = [child_process.exec, child_process.execFile]
  .map(promisify)
  .concat(child_process.spawn);
const { TextDecoder, TextEncoder } = require("text-encoding");

const rpc = new snaxjs.JsonRpc(
  process.env.SNAXNODE || "http://127.0.0.1:8888",
  {
    fetch
  }
);

const { account, privateKey } = {
  account: "platform",
  privateKey: "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"
};

const signatureProvider = new snaxjs.JsSignatureProvider([
  privateKey,
  "5HvtgZn4wf4vNAe3nRb9vjYfLqvasemsSQckVHxmdAeBRbdPURs",
  "5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3",
  "5JD9AGTuTeD5BXZwGQ5AtwBqHK21aHmYnTetHgk1B3pjj7krT8N",
  "5JcWXD3XkpEYbwiVK9Pd3X5bLxLkaUkkJiST3Y9iA4wFrTeyeVL",
  "5JLYkoKuNXGGvUtzjRnP8DqUwt7xny3YGVaDpeqFDdCJKBoBkNC",
  "5JRjkPFeRVGMRLaAa5gkGiC2acf8KT4NkAw1SZ5R7S1gvcCawZh"
]);
const api = new snaxjs.Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

jest.setTimeout(1e6);

describe("System", async () => {
  beforeEach(async () => {
    spawn("./setup_system.sh", [], {
      detached: true,
      stdio: "ignore"
    });
    await sleep(1e4);
  });

  const verifyAccountsBalances = async accounts => {
    const tables = await Promise.all(
      accounts.map(account =>
        api.rpc.get_table_rows({
          code: "snax.token",
          scope: account,
          table: "accounts"
        })
      )
    );
    expect(tables).toMatchSnapshot();
  };

  const verifyState = async () =>
    expect(
      await api.rpc.get_table_rows({
        code: account,
        scope: account,
        table: "state"
      })
    ).toMatchSnapshot();

  const emitplatform = platform =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "emitplatform",
            authorization: [
              {
                actor: platform,
                permission: "owner"
              }
            ],
            data: {
              platform
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const claimrewards = producer =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "claimrewards",
            authorization: [
              {
                actor: producer,
                permission: "active"
              }
            ],
            data: {
              owner: producer
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const undelegatebw = (account, unstake_net_quantity, unstake_cpu_quantity) =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "undelegatebw",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: {
              from: account,
              receiver: account,
              unstake_net_quantity,
              unstake_cpu_quantity
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const verifyBWTable = async accounts => {
    const tables = await Promise.all(
      accounts.map(account =>
        api.rpc.get_table_rows({
          code: "snax",
          scope: account,
          table: "delband"
        })
      )
    );
    expect(tables).toMatchSnapshot();
  };

  const verifyBWEscrowTable = async accounts => {
    const tables = await Promise.all(
      accounts.map(async account =>
        (await api.rpc.get_table_rows({
          code: "snax",
          scope: account,
          table: "escband"
        })).rows.map(({ created, ...obj }) => obj)
      )
    );
    expect(tables).toMatchSnapshot();
  };

  const regproducer = producer =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "regproducer",
            authorization: [
              {
                actor: producer,
                permission: "active"
              }
            ],
            data: {
              producer,
              producer_key:
                "SNAX8mo3cUJW1Yy1GGxQfexWGN7QPUB2rXccQP7brrpgJXGjiw6gKR",
              url: "https://snax.one",
              location: 0
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const voteproducer = producers =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "voteproducer",
            authorization: [
              {
                actor: "snax.team",
                permission: "active"
              }
            ],
            data: {
              voter: "snax.team",
              proxy: "",
              producers
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const getBalance = async account =>
    (await rpc.get_currency_balance("snax.token", account, "SNAX"))[0];

  const transferBack = quantity =>
    api.transact(
      {
        actions: [
          {
            account: "snax.token",
            name: "transfer",
            authorization: [
              {
                actor: "platform",
                permission: "active"
              }
            ],
            data: {
              from: "platform",
              to: "snax",
              quantity,
              memo: "back"
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const escrowbw = (
    actor,
    stake_cpu_quantity,
    stake_net_quantity,
    period_count
  ) =>
    api.transact(
      {
        actions: [
          {
            account: "snax",
            name: "escrowbw",
            authorization: [
              {
                actor,
                permission: "active"
              }
            ],
            data: {
              from: actor,
              receiver: actor,
              stake_cpu_quantity,
              stake_net_quantity,
              transfer: false,
              period_count
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  it("should work correctly with escrow system", async () => {
    await regproducer("test1");
    await voteproducer(["test1"]);
    await tryCatchExpect(() =>
      undelegatebw("snax.team", "1199999921.0000 SNAX", "299999980.0000 SNAX")
    );
    await tryCatchExpect(() =>
      undelegatebw("snax.team", "1199999920.0000 SNAX", "2999999801.0000 SNAX")
    );
    await verifyBWTable(["snax.team"]);
  });

  it("should send correct amounts to special accounts after initialization", async () => {
    await verifyAccountsBalances(["snax.creator", "snax.airdrop"]);
  });

  it("should work correctly with escrow system after initialization", async () => {
    await regproducer("test1");
    await voteproducer(["test1"]);
    await undelegatebw(
      "snax.team",
      "1199200000.0000 SNAX",
      "299800000.0000 SNAX"
    );
    await verifyBWTable(["snax.team"]);
    await escrowbw("test.transf", "10.0000 SNAX", "10.0000 SNAX", 10);
    await verifyBWEscrowTable(["test.transf"]);
    await undelegatebw("test.transf", "1.0000 SNAX", "1.0000 SNAX");
  });

  it("should call system's emitplatform correctly several times", async () => {
    const stepCount = 5e1;
    const pointList = [];
    for (let stepNum = 0; stepNum < stepCount; stepNum++) {
      await emitplatform("platform");
      const balance = await getBalance("platform");
      const amountToTransferBack =
        (parseFloat(balance) * (stepCount - stepNum)) / stepCount;
      await verifyAccountsBalances(["snax", "platform"]);
      await verifyState();
      if (amountToTransferBack > 0.0001)
        await transferBack(amountToTransferBack.toFixed(4) + " SNAX");
    }
  });

  it("should initialize system correctly", async () => {
    await verifyAccountsBalances([
      "snax",
      "snax.team",
      "snax.airdrop",
      "snax.creator"
    ]);
  });

  it("should call system's emitplatform correctly", async () => {
    await emitplatform("platform");
    await verifyAccountsBalances(["snax", "platform"]);
  });

  it("should call system's emitplatform correctly", async () => {
    await emitplatform("platform");
    await emitplatform("test1");
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
  });
});
