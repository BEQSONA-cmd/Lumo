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
