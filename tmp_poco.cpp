/*{
    "name":"John",
    "age":35,
    "isMarried":true,
    "address":{
        "street":"123 Main St.",
        "city":"Anytown",
        "state":"CA",
        "zip":"12345"
    },
    "hobbies":[
        "reading",
        "music",
        "hiking"
    ]
}*/
-------------------------------
#include <iostream>
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"

int main() {
    Poco::JSON::Object jsonObj;
    jsonObj.set("name", "John");
    jsonObj.set("age", 35);
    jsonObj.set("isMarried", true);

    Poco::JSON::Object addressObj;
    addressObj.set("street", "123 Main St.");
    addressObj.set("city", "Anytown");
    addressObj.set("state", "CA");
    addressObj.set("zip", "12345");

    Poco::JSON::Array hobbiesArr;
    hobbiesArr.add("reading");
    hobbiesArr.add("music");
    hobbiesArr.add("hiking");

    jsonObj.set("address", addressObj);
    jsonObj.set("hobbies", hobbiesArr);

    std::cout << jsonObj.toString() << std::endl;

    return 0;
}
