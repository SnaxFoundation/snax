const snaxjs = require("@snaxfoundation/snaxjs");
const fetch = require("node-fetch");
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
  "5JcWXD3XkpEYbwiVK9Pd3X5bLxLkaUkkJiST3Y9iA4wFrTeyeVL",
  "5JLYkoKuNXGGvUtzjRnP8DqUwt7xny3YGVaDpeqFDdCJKBoBkNC"
]);
const api = new snaxjs.Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

jest.setTimeout(1e6);

const sleep = time => new Promise(resolve => setTimeout(resolve, time));

describe("System", async () => {
  beforeEach(async () => {
    spawn("./setup_system.sh", [], {
      detached: true,
      stdio: "ignore"
    });
    await sleep(6e3);
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

  const tryCatchExpect = async action => {
    try {
      await action();
      expect(false).toBe(true);
    } catch (e) {
      expect(e.message).toMatchSnapshot();
    }
  };

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

  const undelegatebw = account =>
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
              unstake_net_quantity: "999999999.5000 SNAX",
              unstake_cpu_quantity: "999999999.5000 SNAX"
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
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
  });

  it("should call system's emitplatform correctly", async () => {
    await emitplatform("platform");
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
  });

  it("should undelegate snax.team resources correctly", async () => {
    await regproducer("test1");
    await voteproducer(["test1"]);
    await undelegatebw("snax.team");
    await verifyBWTable(["snax.team"]);
  });

  it("should call system's emitplatform correctly", async () => {
    await emitplatform("platform");
    await emitplatform("test1");
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
  });
});
