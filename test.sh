#!/bin/bash

SERVER="localhost"
PORT=8080
URL="http://$SERVER:$PORT"

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

function test_endpoint() {
    METHOD=$1
    ENDPOINT=$2
    EXPECTED_CODE=$3
    DATA=$4

    echo -n "[$METHOD $ENDPOINT] Expected: $EXPECTED_CODE -> "

    if [ "$METHOD" == "GET" ]; then
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" "$URL$ENDPOINT")
    elif [ "$METHOD" == "POST" ]; then
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X POST -F "file=@$DATA" "$URL$ENDPOINT")
    elif [ "$METHOD" == "DELETE" ]; then
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X DELETE "$URL$ENDPOINT")
    elif [ "$METHOD" == "HEAD" ]; then
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -I "$URL$ENDPOINT")
    else
        RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X "$METHOD" "$URL$ENDPOINT")
    fi

    if [ "$RESPONSE" == "$EXPECTED_CODE" ]; then
        echo -e "${GREEN}PASS${NC}"
    else
        echo -e "${RED}FAIL${NC} (got $RESPONSE)"
    fi
}

echo "Starting tests on $URL"
echo "==============================="

# Basic GET
# test_endpoint GET "/" 200
# test_endpoint GET "/index.html" 200
# test_endpoint GET "/nonexistent.html" 404

# POST
# test_endpoint POST "/upload" 200 "./cat.png"
test_endpoint POST "/uploads" 404 "./cat.png"

# DELETE
# test_endpoint DELETE "/cat.png" 200
# test_endpoint DELETE "/nonexistent.txt" 404

# Redirection
# test_endpoint GET "/redirect" 301

# Forbidden
# test_endpoint GET "/forbidden/" 403


# CGI (make sure CGI is configured and the script exists)
# test_endpoint GET "/cgi/helloWorld.py" 200
# test_endpoint GET "/cgi/helloWorld.py?name=test&num=42" 200

# Invalid Method
# test_endpoint PUT "/" 405

echo "==============================="
echo "Test run complete."

# ...existing code...




