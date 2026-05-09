#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"

#include "calculator_functionality.h"

/* ===== Oat Documentation =====
 * https://oatpp.io/docs/start/step-by-step/#api-low-level-components-overview
 * 
 * Ideally we want to use GET requests for building this, for example a route
 * should look like 'GET /calculate?expr=2+2', passing the expression as a query string
 * so we can easily integrate a frontend.
*/



// Define a handler for a route, go to /hello for a greeting
class HelloHandler : public oatpp::web::server::HttpRequestHandler {
public:
	std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
		return ResponseFactory::createResponse(Status::CODE_200, "Hello from Calculator API!");
	}
};

/// <summary>
/// Expression handler, this takes in the request and sends back a response.
/// 
/// Example query: http://localhost:8000/calculate?expression=2+2 
/// </summary>
class ExpressionHandler : public oatpp::web::server::HttpRequestHandler {
private:
	calculator_functionality calculator;
public:
	std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override {
		// getting the query param from URL

		// auto keyword tells the compliler to deduce the type
		auto expression = request->getQueryParameter("expression");

		if (!expression) {
			// No expression provided
			return ResponseFactory::createResponse(Status::CODE_400, "Missing 'expression' query parameter");
		}

		// expression.getValue("") converts it to a std::string
		std::string exprStr = expression.getValue("");
		std::wstring exprWstr(exprStr.begin(), exprStr.end());
		std::string computed = calculator.Compute(exprWstr);
		std::string response = "Response : " + computed; // get a string to work. we need to use the other files to get a response
		return ResponseFactory::createResponse(Status::CODE_200, response);
	}
};

void run() {

	/* Create Router for HTTP requests routing */
	auto router = oatpp::web::server::HttpRouter::createShared();

	// Register a route
	router->route("GET", "/hello", std::make_shared<HelloHandler>());

	router->route("GET", "/calculate", std::make_shared<ExpressionHandler>());

	auto connectionHandler = oatpp::web::server::HttpConnectionHandler::createShared(router);
	auto connectionProvider = oatpp::network::tcp::server::ConnectionProvider::createShared(
		{ "0.0.0.0", 8000, oatpp::network::Address::IP_4 }
	);

	oatpp::network::Server server(connectionProvider, connectionHandler);
	OATPP_LOGI("MyApp", "Server running on port %s", connectionProvider->getProperty("port").getData());
	server.run();
}

int main() {

	/* Init oatpp Environment */
	oatpp::base::Environment::init();

	/* Run App */
	run();

	/* Destroy oatpp Environment */
	oatpp::base::Environment::destroy();

	return 0;

}