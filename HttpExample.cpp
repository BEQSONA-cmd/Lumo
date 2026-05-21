#include "Lumo/Lumo.hpp"


class User
{
public:
    std::string username;
    std::string email;
    int age;
};
// after creating the class you have to define the DTO type,
// only if its used as a request body or response body
DTO(User, username, email, age);

User globalUser;

// the Response can handle 3 types of body
// 1. Object
Response GetUserRoute(Request req)
{
    return Response(globalUser, 200, "OK");
}

// 2. JSON object
Response GetUserRoute2(Request req)
{
    return Response(json(globalUser), 200, "OK");
}

// 3. String format JSON
Response GetUserRoute3(Request req)
{
    return Response(json(globalUser).dump(), 200, "OK");
}

Response RegisterUserRoute(Request req)
{
    // json::parse() will get the JSON from the request body and convert it to a json object
    // json j = json::parse(req.body);

    // then we can use the get() method to convert the json object to a User object
    // User newUser = j.get<User>();

    // or just do it in one line
    globalUser = json::parse(req.body).get<User>();

    return Response("User registered successfully", 201, "OK");
}

// the fetch function can be used to make HTTP requests to other servers
Response FetchTestRoute(Request req)
{
    auto res = fetch("https://jsonplaceholder.typicode.com/todos/1");
    return Response(res.body, 200, "OK");
}

// the fetch function can also be used to make requests with a different method, headers and body
Response FetchTestRoute2(Request req)
{
    const User user = {
        .username = "john_doe",
        .email = "john.doe@example.com",
        .age = 30
    };

    auto res = fetch("https://jsonplaceholder.typicode.com/posts", {
            .method = "POST",
            .headers = {
                {"Content-Type", "application/json"},
                {"Accept", "application/json"}
            },
            .body = json(user).dump()
        }
    );
    // res.body will contain the response body from the server
    // res.status will contain the HTTP status code of the response

    return Response(res.body, 200, "OK");
}

// dotenv function to read environment variables from a .env file
std::string dotenv(const std::string &key)
{
    std::ifstream file(".env");
    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        size_t pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        std::string name = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (name == key)
            return value;
    }

    return "";
}

// example of streaming response from the GROQ API using the fetch function with a stream callback
void StreamCallbackFunction(const std::string &chunk)
{
    std::stringstream localStream(chunk);
    std::string line;

    while (std::getline(localStream, line))
    {
        if (line.rfind("data: ", 0) == 0)
        {
            std::string jsonPart = line.substr(6);

            if (jsonPart == "[DONE]")
            {
                std::cout << std::endl;
                return;
            }

            try
            {
                auto json = json::parse(jsonPart);

                auto &delta = json["choices"][0]["delta"];

                if (delta.contains("content"))
                {
                    std::string content = delta["content"];
                    std::cout << content << std::flush;
                }
            }
            catch (...){}
        }
    }
}

// the fetch function can also be used to handle streaming responses, by providing a stream callback function in the options
Response FetchTestRoute3(Request req)
{
    const std::string API_KEY = dotenv("GROQ_API_KEY");

    auto res = fetch("https://api.groq.com/openai/v1/chat/completions", {
        .method = "POST",
        .headers = {
            {"Authorization", "Bearer " + API_KEY},
            {"Content-Type", "application/json"}},
        .body = json(
            {
                {"model", "llama-3.3-70b-versatile"},
                {"messages", {
                    {
                        {"role", "user"}, 
                        {"content", "Explain quantum computing in simple words"}
                    }
                }},
                {"stream", true}
            }
        ).dump(),
        .streamCallback = StreamCallbackFunction
    });

    return Response("Stream started, check the console for output", 200, "OK");
}
