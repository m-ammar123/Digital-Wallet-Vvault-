#include "crow_all.h"
#include "VaultApp.h"
#include <fstream>
#include <sstream>

// ── Helper: build full user JSON (what login & statement return) ──
crow::json::wvalue buildUserJson(VaultApp& app, const std::string& phone) {
    auto p   = app.getProfile(phone);
    auto txs = app.getStatement(phone);

    double totalIn = 0, totalOut = 0;
    std::vector<crow::json::wvalue> history;
    for (auto* t : txs) {
        crow::json::wvalue j;
        j["type"]        = t->getType();
        j["amount"]      = t->getAmount();
        j["balance"]     = t->getBalanceAfter();
        j["description"] = t->getDescription();
        j["timestamp"]   = t->getTimestamp();
        history.push_back(std::move(j));

        std::string tp = t->getType();
        if (tp == "DEPOSIT" || tp == "RECEIVED")
            totalIn  += t->getAmount();
        else
            totalOut += t->getAmount();
    }

    crow::json::wvalue wallet;
    wallet["walletID"] = p.walletID;
    wallet["balance"]  = p.balance;
    wallet["totalIn"]  = totalIn;
    wallet["totalOut"] = totalOut;
    wallet["txCount"]  = (int)txs.size();
    wallet["history"]  = std::move(history);

    crow::json::wvalue user;
    user["name"]        = p.name;
    user["phone"]       = p.phone;
    user["cnic"]        = p.cnic;
    user["accountType"] = "personal";
    user["wallet"]      = std::move(wallet);

    return user;
}

crow::response jsonErr(int code, const std::string& msg) {
    crow::json::wvalue e;
    e["ok"] = false; e["error"] = msg;
    return crow::response(code, e);
}

int main() {
    VaultApp app;
    crow::SimpleApp server;

    // ── Serve index.html ──────────────────────────────────────────
    CROW_ROUTE(server, "/")([]() {
        std::ifstream f("index.html");
        std::stringstream ss; ss << f.rdbuf();
        crow::response res(ss.str());
        res.add_header("Content-Type", "text/html");
        return res;
    });

    // ── GET /api/health ───────────────────────────────────────────
    CROW_ROUTE(server, "/api/health")([]() {
        crow::json::wvalue r;
        r["ok"]     = true;
        r["server"] = "vVault C++ Crow";
        return crow::response(200, r);
    });

    // ── POST /api/register ────────────────────────────────────────
    // Body: { name, phone, cnic, pin, accountType, bizName, initialDeposit }
    CROW_ROUTE(server, "/api/register").methods("POST"_method)
    ([&app](const crow::request& req) {
        auto b = crow::json::load(req.body);
        if (!b) return jsonErr(400, "Invalid JSON");

        std::string name  = b["name"].s();
        std::string phone = b["phone"].s();
        std::string cnic  = b["cnic"].s();
        std::string pin   = b["pin"].s();
        double deposit    = b["initialDeposit"].d();

        auto err = app.registerUser(name, phone, cnic, pin, deposit);
        if (!err.empty()) return jsonErr(400, err);

        crow::json::wvalue r;
        r["ok"]       = true;
        r["walletID"] = app.getProfile(phone).walletID;
        return crow::response(200, r);
    });

    // ── POST /api/login ───────────────────────────────────────────
    // Body: { phone, pin }
    // Returns: { ok, user: { name, phone, cnic, accountType, wallet: {...} } }
    CROW_ROUTE(server, "/api/login").methods("POST"_method)
    ([&app](const crow::request& req) {
        auto b = crow::json::load(req.body);
        if (!b) return jsonErr(400, "Invalid JSON");

        std::string phone = b["phone"].s();
        std::string pin   = b["pin"].s();

        if (!app.verifyLogin(phone, pin))
            return jsonErr(401, "Invalid phone or PIN");

        crow::json::wvalue r;
        r["ok"]   = true;
        r["user"] = buildUserJson(app, phone);
        return crow::response(200, r);
    });

    // ── POST /api/deposit ─────────────────────────────────────────
    // Body: { phone, pin, amount, description }
    CROW_ROUTE(server, "/api/deposit").methods("POST"_method)
    ([&app](const crow::request& req) {
        auto b = crow::json::load(req.body);
        if (!b) return jsonErr(400, "Invalid JSON");

        std::string phone = b["phone"].s();
        std::string pin   = b["pin"].s();
        double amount     = b["amount"].d();

        if (!app.verifyLogin(phone, pin))
            return jsonErr(401, "Invalid phone or PIN");

        auto err = app.deposit(phone, amount);
        if (!err.empty()) return jsonErr(400, err);

        crow::json::wvalue r;
        r["ok"]      = true;
        r["balance"] = app.getBalance(phone);
        return crow::response(200, r);
    });

    // ── POST /api/withdraw ────────────────────────────────────────
    // Body: { phone, pin, amount, description }
    CROW_ROUTE(server, "/api/withdraw").methods("POST"_method)
    ([&app](const crow::request& req) {
        auto b = crow::json::load(req.body);
        if (!b) return jsonErr(400, "Invalid JSON");

        std::string phone = b["phone"].s();
        std::string pin   = b["pin"].s();
        double amount     = b["amount"].d();

        if (!app.verifyLogin(phone, pin))
            return jsonErr(401, "Invalid phone or PIN");

        auto err = app.withdraw(phone, amount);
        if (!err.empty()) return jsonErr(400, err);

        crow::json::wvalue r;
        r["ok"]      = true;
        r["balance"] = app.getBalance(phone);
        return crow::response(200, r);
    });

    // ── POST /api/send ────────────────────────────────────────────
    // Body: { fromPhone, pin, toPhone, amount }
    CROW_ROUTE(server, "/api/send").methods("POST"_method)
    ([&app](const crow::request& req) {
        auto b = crow::json::load(req.body);
        if (!b) return jsonErr(400, "Invalid JSON");

        std::string fromPhone = b["fromPhone"].s();
        std::string pin       = b["pin"].s();
        std::string toPhone   = b["toPhone"].s();
        double amount         = b["amount"].d();

        if (!app.verifyLogin(fromPhone, pin))
            return jsonErr(401, "Invalid phone or PIN");

        auto err = app.sendMoney(fromPhone, toPhone, amount);
        if (!err.empty()) return jsonErr(400, err);

        crow::json::wvalue r;
        r["ok"]            = true;
        r["balance"]       = app.getBalance(fromPhone);
        r["recipientName"] = app.getUserName(toPhone);
        return crow::response(200, r);
    });

    // ── GET /api/statement/:phone/:pin ────────────────────────────
    CROW_ROUTE(server, "/api/statement/<string>/<string>")
    ([&app](const std::string& phone, const std::string& pin) {
        if (!app.verifyLogin(phone, pin))
            return crow::response(jsonErr(401, "Invalid phone or PIN"));

        auto user = buildUserJson(app, phone);
        return crow::response(200, user);
    });

    std::cout << "\n🚀 vVault running at http://localhost:8080\n";
    server.port(8080).multithreaded().run();
}