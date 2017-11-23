"""Minimal collect example. Takes url as argv[1]"""
import sys
import requests


def get(url):
    return requests.get(url, headers={'user-agent': 'collectc/0.0'})


def main():
    if len(sys.argv) < 2:
        reddit_url = "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10"
    else:
        reddit_url = sys.argv[1]

    for post in get(reddit_url).json()['data']['children']:
        url = post['data']['url']
        req = get(url)
        if req.headers['content-type'].startswith('image'):
            print('image')

            with open('image', 'wb') as file:
                file.write(req.content)

            break


if __name__ == '__main__':
    main()
