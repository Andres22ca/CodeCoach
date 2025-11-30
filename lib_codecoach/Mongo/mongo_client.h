
// Created by andres on 5/10/25.

#ifndef LIB_CODECOACH_MONGO_CLIENT_H
#define LIB_CODECOACH_MONGO_CLIENT_H

#include <string>
#include <vector>
#include <optional>
#include <memory>

namespace cc::storage {

    class MongoClient {
    public:
        /**
         * @param uri      URI de Mongo, ej: "mongodb://localhost:27017"
         * @param dbName   Nombre de la base, ej: "codecoach"
         * @param useStub  Si es true, fuerza modo stub aunque CC_ENABLE_MONGO_REAL esté definido.
         */
        MongoClient(const std::string& uri,
                    const std::string& dbName,
                    bool useStub = true);

        // IMPORTANTE: dtor/moves declarados aquí y definidos en el .cpp
        ~MongoClient();
        MongoClient(const MongoClient&) = delete;
        MongoClient& operator=(const MongoClient&) = delete;
        MongoClient(MongoClient&&) noexcept;
        MongoClient& operator=(MongoClient&&) noexcept;

        /// Intenta establecer la conexión (en stub solo marca conectado = true).
        bool connect();

        /// Indica si el cliente está conectado.
        [[nodiscard]] bool isConnected() const;

        // --------- Operaciones CRUD genéricas ---------

        bool insertOne(const std::string& collection,
                       const std::string& jsonDocument);

        std::optional<std::string> findOne(const std::string& collection,
                                           const std::string& jsonFilter);

        std::vector<std::string> findMany(const std::string& collection,
                                          const std::string& jsonFilter);

        bool replaceOne(const std::string& collection,
                        const std::string& jsonFilter,
                        const std::string& jsonDocument);

        bool deleteOne(const std::string& collection,
                       const std::string& jsonFilter);

    private:
        std::string uri_;
        std::string dbName_;
        bool useStub_;
        bool connected_{false};

#ifdef CC_ENABLE_MONGO_REAL
        struct Impl;                    // PIMPL para ocultar mongocxx
        std::unique_ptr<Impl> impl_;
#endif
    };

} // namespace cc::storage

#endif // LIB_CODECOACH_MONGO_CLIENT_H
