"""Minimal collect example. Takes url as argv[1]"""
import sys
import requests

for post in requests.get(
    sys.argv[1], headers={'user-agent': 'collectc/0.0'}
).json()['data']['children']:
    url = post['data']['url']
    req = requests.get(
        url, headers={'user-agent': 'collectc/0.0'}
    )
    if req.headers['content-type'].startswith('image'):
        with open('image', 'wb') as file:
            file.write(req.content)

        break
