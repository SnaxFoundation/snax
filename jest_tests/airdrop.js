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
  "5JcWXD3XkpEYbwiVK9Pd3X5bLxLkaUkkJiST3Y9iA4wFrTeyeVL"
]);
const api = new snaxjs.Api({
  rpc,
  signatureProvider,
  textDecoder: new TextDecoder(),
  textEncoder: new TextEncoder()
});

jest.setTimeout(1e6);

const sleep = time => new Promise(resolve => setTimeout(resolve, time));

describe("Airdrop", async () => {
  beforeEach(async () => {
    spawn("./setup_airdrop.sh", [], {
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

  const addplatform = async platform => {
    await api.transact(
      {
        actions: [
          {
            account: "snax.airdrop",
            name: "addplatform",
            authorization: [
              {
                actor: "snax.airdrop",
                permission: "active"
              }
            ],
            data: {
              platform,
              amount_per_account: "20.0000 SNAX"
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

  const request = async (platform, account) => {
    await api.transact(
      {
        actions: [
          {
            account: "snax.airdrop",
            name: "request",
            authorization: [
              {
                actor: platform,
                permission: "active"
              }
            ],
            data: {
              platform,
              account
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

  it("should add platform correctly", () => addplatform("platform"));

  it("shouldn't be able to add platform because it's not in system", async () => {
    await tryCatchExpect(() => addplatform("platform1"));
  });

  it("shouldn't be able to request because platform isn't added", async () => {
    addplatform("test2");
    await tryCatchExpect(() => request("platform", "test1"));
  });

  it("shouldn't be able to request because already requested", async () => {
    await addplatform("platform");
    await verifyAccountsBalances(["test1", "snax.airdrop"]);
    await request("platform", "test1");
    await verifyAccountsBalances(["test1", "snax.airdrop"]);
    await request("platform", "test1");
    await verifyAccountsBalances(["test1", "snax.airdrop"]);
  });

  it("should request airdrop for account correctly", async () => {
    await addplatform("platform");
    await request("platform", "test1");
    await verifyAccountsBalances(["test1", "snax.airdrop"]);
  });
});
