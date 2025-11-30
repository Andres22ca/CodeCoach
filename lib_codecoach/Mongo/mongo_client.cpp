// Created by andres on 5/10/25.
//

#include "Mongo/mongo_client.h"
#include "logging/logger.h"

#include <iostream>

#ifdef CC_ENABLE_MONGO_REAL
    #include <bsoncxx/json.hpp>
    #include <mongocxx/client.hpp>
    #include <mongocxx/instance.hpp>
    #include <mongocxx/uri.hpp>
    #include <mongocxx/exception/exception.hpp>
#endif

namespace cc::storage {

using cc::logging::Logger;

#ifdef CC_ENABLE_MONGO_REAL

struct MongoClient::Impl {
    static mongocxx::instance& global_instance() {
        static mongocxx::instance inst{};
        return inst;
    }

    mongocxx::client   client;
    mongocxx::database db;

    Impl(const std::string& uri, const std::string& dbName)
        : client(mongocxx::uri{uri}), db(client[dbName]) {}
};

#endif // CC_ENABLE_MONGO_REAL

// ====== ctor/dtor/moves ======

MongoClient::MongoClient(const std::string& uri,
                         const std::string& dbName,
                         bool useStub)
    : uri_(uri),
      dbName_(dbName),
      useStub_(useStub)
{
#ifdef CC_ENABLE_MONGO_REAL
    if (!useStub_) {
        connect();
    } else {
        Logger::info("[MongoClient] Using STUB mode; no real Mongo connection.");
    }
#else
    (void)uri_;
    (void)dbName_;
    (void)useStub_;
    Logger::info("[MongoClient] Built without Mongo driver (stub only).");
#endif
}

// Definimos fuera de la clase, después de que Impl está completo
MongoClient::~MongoClient() = default;
MongoClient::MongoClient(MongoClient&&) noexcept = default;
MongoClient& MongoClient::operator=(MongoClient&&) noexcept = default;

// ====== API ======

bool MongoClient::connect() {
    if (connected_) {
        return true;
    }

    if (useStub_) {
        connected_ = true;
        Logger::debug("[MongoClient] connect() in STUB mode");
        return true;
    }

#ifndef CC_ENABLE_MONGO_REAL
    Logger::warn("[MongoClient] connect() requested real mode, but CC_ENABLE_MONGO_REAL is not defined.");
    connected_ = false;
    return false;
#else
    try {
        (void)Impl::global_instance();
        impl_ = std::make_unique<Impl>(uri_, dbName_);
        connected_ = true;
        Logger::info("[MongoClient] Connected (real) to " + uri_ +
                     " db=" + dbName_);
        return true;
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] connect() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] connect() std::exception: ") + ex.what());
    }

    connected_ = false;
    return false;
#endif
}

bool MongoClient::isConnected() const {
    return connected_;
}

bool MongoClient::insertOne(const std::string& collection,
                            const std::string& jsonDocument)
{
    if (useStub_) {
        std::cout << "[MongoStub] insertOne into '" << collection << "'\n";
        return true;
    }

#ifndef CC_ENABLE_MONGO_REAL
    (void)collection;
    (void)jsonDocument;
    Logger::warn("[MongoClient] insertOne() called but real Mongo is not enabled.");
    return false;
#else
    if (!connected_ || !impl_) {
        Logger::error("[MongoClient] insertOne() called but not connected.");
        return false;
    }

    try {
        auto coll = impl_->db[collection];
        auto doc  = bsoncxx::from_json(jsonDocument);
        auto res  = coll.insert_one(std::move(doc));

        // 'res' es std::optional<insert_one>; si hay valor, consideramos que fue OK
        return static_cast<bool>(res);
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] insertOne() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] insertOne() std::exception: ") + ex.what());
    }

    return false;
#endif
}

std::optional<std::string> MongoClient::findOne(const std::string& collection,
                                                const std::string& jsonFilter)
{
    if (useStub_) {
        std::cout << "[MongoStub] findOne in '" << collection << "'\n";
        return std::nullopt;
    }

#ifndef CC_ENABLE_MONGO_REAL
    (void)collection;
    (void)jsonFilter;
    Logger::warn("[MongoClient] findOne() called but real Mongo is not enabled.");
    return std::nullopt;
#else
    if (!connected_ || !impl_) {
        Logger::error("[MongoClient] findOne() called but not connected.");
        return std::nullopt;
    }

    try {
        auto coll   = impl_->db[collection];
        auto filter = bsoncxx::from_json(jsonFilter);
        auto result = coll.find_one(filter.view());

        if (!result) {
            return std::nullopt;
        }

        return bsoncxx::to_json(result->view());
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] findOne() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] findOne() std::exception: ") + ex.what());
    }

    return std::nullopt;
#endif
}

std::vector<std::string> MongoClient::findMany(const std::string& collection,
                                               const std::string& jsonFilter)
{
    if (useStub_) {
        std::cout << "[MongoStub] findMany in '" << collection << "'\n";
        return {};
    }

#ifndef CC_ENABLE_MONGO_REAL
    (void)collection;
    (void)jsonFilter;
    Logger::warn("[MongoClient] findMany() called but real Mongo is not enabled.");
    return {};
#else
    std::vector<std::string> out;

    if (!connected_ || !impl_) {
        Logger::error("[MongoClient] findMany() called but not connected.");
        return out;
    }

    try {
        auto coll   = impl_->db[collection];
        auto filter = bsoncxx::from_json(jsonFilter);

        auto cursor = coll.find(filter.view());
        for (auto&& doc : cursor) {
            out.push_back(bsoncxx::to_json(doc));
        }
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] findMany() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] findMany() std::exception: ") + ex.what());
    }

    return out;
#endif
}

bool MongoClient::replaceOne(const std::string& collection,
                             const std::string& jsonFilter,
                             const std::string& jsonDocument)
{
    if (useStub_) {
        std::cout << "[MongoStub] replaceOne in '" << collection << "'\n";
        return true;
    }

#ifndef CC_ENABLE_MONGO_REAL
    (void)collection;
    (void)jsonFilter;
    (void)jsonDocument;
    Logger::warn("[MongoClient] replaceOne() called but real Mongo is not enabled.");
    return false;
#else
    if (!connected_ || !impl_) {
        Logger::error("[MongoClient] replaceOne() called but not connected.");
        return false;
    }

    try {
        auto coll   = impl_->db[collection];
        auto filter = bsoncxx::from_json(jsonFilter);
        auto doc    = bsoncxx::from_json(jsonDocument);

        auto res = coll.replace_one(filter.view(), doc.view());
        return (res && res->modified_count() > 0);
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] replaceOne() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] replaceOne() std::exception: ") + ex.what());
    }

    return false;
#endif
}

bool MongoClient::deleteOne(const std::string& collection,
                            const std::string& jsonFilter)
{
    if (useStub_) {
        std::cout << "[MongoStub] deleteOne in '" << collection << "'\n";
        return true;
    }

#ifndef CC_ENABLE_MONGO_REAL
    (void)collection;
    (void)jsonFilter;
    Logger::warn("[MongoClient] deleteOne() called but real Mongo is not enabled.");
    return false;
#else
    if (!connected_ || !impl_) {
        Logger::error("[MongoClient] deleteOne() called but not connected.");
        return false;
    }

    try {
        auto coll   = impl_->db[collection];
        auto filter = bsoncxx::from_json(jsonFilter);

        auto res = coll.delete_one(filter.view());
        return (res && res->deleted_count() > 0);
    } catch (const mongocxx::exception& ex) {
        Logger::error(std::string("[MongoClient] deleteOne() failed: ") + ex.what());
    } catch (const std::exception& ex) {
        Logger::error(std::string("[MongoClient] deleteOne() std::exception: ") + ex.what());
    }

    return false;
#endif
}

} // namespace cc::storage
