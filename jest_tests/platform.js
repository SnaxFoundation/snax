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

const signatureProvider = new snaxjs.JsSignatureProvider([privateKey]);
const api = new snaxjs.Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

jest.setTimeout(1e6);

const sleep = time => new Promise(resolve => setTimeout(resolve, time));

describe("Platform", async () => {
  beforeEach(async () => {
    spawn("./setup_platform.sh", [], {
      detached: true,
      stdio: "ignore"
    });
    await sleep(6e3);
  });

  const verifyStatesAndAccounts = async () => {
    const [state, accounts] = await Promise.all([
      api.rpc.get_table_rows({
        code: account,
        scope: account,
        table: "state"
      }),
      api.rpc.get_table_rows({
        code: account,
        scope: account,
        table: "paccounts"
      })
    ]);
    expect(state).toMatchSnapshot();
    expect(accounts).toMatchSnapshot();
  };

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

  const initialize = async () => {
    await api.transact(
      {
        actions: [
          {
            account,
            name: "initialize",
            authorization: [
              {
                actor: account,
                permission: "owner"
              }
            ],
            data: {
              token_dealer: "snax",
              token_symbol_str: "SNAX",
              precision: 4,
              airdrop: ""
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );
  };

  const addUser = accountObj =>
    api.transact(
      {
        actions: [
          {
            account: account,
            name: "addaccount",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: {
              ...accountObj
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const lockUpdate = () =>
    api.transact(
      {
        actions: [
          {
            account: account,
            name: "lockupdate",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: {}
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const updatePlatform = async () => {
    await lockUpdate();
    await api.transact(
      {
        actions: [
          {
            account,
            name: "nextround",
            authorization: [
              {
                actor: account,
                permission: "owner"
              }
            ],
            data: {}
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
    await api.transact(
      {
        actions: [
          {
            account,
            name: "sendpayments",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: {
              serial: 0,
              account_count: 1000
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );
  };

  const updateQualityRate = accountObj =>
    api.transact(
      {
        actions: [
          {
            account: account,
            name: "updatear",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: {
              ...accountObj,
              add_account_if_not_exist: false
            }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  const updateQualityRateMulti = updates =>
    api.transact(
      {
        actions: [
          {
            account: account,
            name: "updatearmult",
            authorization: [
              {
                actor: account,
                permission: "active"
              }
            ],
            data: { updates, add_account_if_not_exist: false }
          }
        ]
      },
      {
        blocksBehind: 1,
        expireSeconds: 30
      }
    );

  it("should process next round correctly", async () => {
    await initialize();
    await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await addUser({
      account: "test1",
      id: 1105,
      attention_rate: 225.0
    });
    await addUser({
      account: "test1",
      id: 1200,
      attention_rate: 206.0
    });
    await addUser({
      account: "test1",
      id: 1007,
      attention_rate: 206.0
    });
    await updatePlatform();
    await verifyStatesAndAccounts();
    await verifyAccountsBalances(["test2", "test1", "snax", "platform"]);
  });

  it("should initialize correctly", async () => {
    await initialize();
    await verifyStatesAndAccounts();
  });

  it("shouldn't be able to initialize second time", async () => {
    await initialize();
    await tryCatchExpect(initialize);
    await verifyStatesAndAccounts();
  });

  it("should add account correctly", async () => {
    await initialize();
    const result = await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await verifyStatesAndAccounts();
  });

  it("shouldn't be able to add user with the same id second time", async () => {
    await initialize();
    await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await tryCatchExpect(() =>
      addUser({
        account: "test2",
        id: 123,
        attention_rate: 26.0
      })
    );
    await verifyStatesAndAccounts();
  });

  it("should update account's attention rate correctly", async () => {
    await initialize();
    await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await updateQualityRate({
      id: 123,
      attention_rate: 20.0
    });
    await verifyStatesAndAccounts();
  });

  it("should update multiple account's attention rate correctly", async () => {
    await initialize();
    await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await addUser({
      account: "test1",
      id: 243,
      attention_rate: 8.0
    });
    await updateQualityRateMulti([
      { id: 243, attention_rate: 20.0 },
      { id: 123, attention_rate: 25.0 }
    ]);
    await verifyStatesAndAccounts();
  });

  it("shouldn't be able to update non-existent account's attention rate", async () => {
    await initialize();
    await addUser({
      account: "test2",
      id: 123,
      attention_rate: 15.0
    });
    await tryCatchExpect(() =>
      updateQualityRate({
        id: 250,
        attention_rate: 20.0
      })
    );

    await verifyStatesAndAccounts();
  });

  it("shouldn't be able to add account or update attention rate when platform is updating", async () => {
    await initialize();
    await addUser({
      account: "test1",
      id: 1105,
      attention_rate: 225.0
    });
    await lockUpdate();
    await tryCatchExpect(() =>
      updateQualityRate({
        id: 1105,
        attention_rate: 20.0
      })
    );
    await tryCatchExpect(() =>
      updateQualityRateMulti([
        {
          id: 1105,
          attention_rate: 20.0
        }
      ])
    );
    await tryCatchExpect(() =>
      addUser({
        account: "test2",
        id: 123,
        attention_rate: 15.0
      })
    );
    await verifyStatesAndAccounts();
  });
});
