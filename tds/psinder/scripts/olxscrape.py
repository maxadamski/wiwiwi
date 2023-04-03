#!/usr/bin/env python3

import re
import sys
import requests
import json
from bs4 import BeautifulSoup as Soup
from pprint import pprint

def get_page(page: int):
    url = f'https://www.olx.pl/zwierzeta/psy/psy-do-adopcji/?search%5Bfilter_enum_shelter%5D%5B0%5D=1&search%5Bphotos%5D=1&page={page}'
    return requests.get(url)

def get_advert(page: str):
    return requests.get('https://www.olx.pl/d/oferta/*')

def print_adverts(page_count: int = 1):
    for page_no in range(page_count):
        resp = get_page(page_no + 1)
        if resp.status_code != 200: break
        page = Soup(resp.text, 'html.parser')
        urls = {x.get('href') for x in page.find_all('a', class_=['detailsLink', 'detailsLinkPromoted'])}
        print(f'get page {page_no + 1} - got {len(urls)} urls', file=sys.stderr)
        for url in urls:
            print(url)

def get_photo_url(img):
    srcset = img.get('data-srcset')
    if srcset is None:
        return None
    srcset = [x.split(' ')[0] for x in srcset.split(',')]
    return srcset[0]

def scrape_advert():
    url = sys.argv[1]
    print(url, file=sys.stderr)
    resp = requests.get(url)
    if resp.status_code != 200:
        print('{}')
        sys.exit(1)

    text = resp.text
    html = Soup(text, 'html.parser')
    title = html.find('h1', **{'data-cy': 'ad_title'}).text
    descr = html.find(**{'data-cy': 'ad_description'}).div.text
    user_card = html.find('a', {'name': 'user_ads'})
    user_url = user_card.get('href')
    user_name = user_card.h2.text
    photos = [get_photo_url(x) for x in html.find_all('img', class_='swiper-lazy')]
    photos = [x for x in photos if x is not None]
    hack = json.loads(eval(re.search(r'__PRERENDERED_STATE__= .*', text).group(0)[23:-1]))

    def api(path=''):
        res = hack['ad']['ad']
        if path:
            path = path.split('->')
            for p in path:
                res = res[p]
        return res

    data = dict(
            title=title, description=descr,
            url=url,
            photos=photos,
            location=dict(
                text=api('location->pathName'),
                city=api('location->cityName'),
                region=api('location->regionName'),
                lat=api('map->lat'),
                lon=api('map->lon'),
            ),
            contact=dict(name=user_name, url=user_url),
            adopted=False
    )

    with open('dogs.jsonl', 'a') as f:
        print(json.dumps(data, ensure_ascii=False), file=f)

