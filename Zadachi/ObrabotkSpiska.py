import requests
from typing import *

#
# Вам нужно сделать GET запрос к веб-странице и по введенному идентификатору пользователя распечатать все заголовки его
# выполненных заметок или "False",
# если такого пользователя не существует.
#

ID = 'userId'
TITLE = 'title'

def get_user_entry(user_id: int, data: List[Dict]):

    result = [cur_data for cur_data in data if cur_data[ID] == user_id]
    return result


def get_data(url) -> Optional[List]:

    request = requests.get(url)
    try:
        data = request.json()
        return data
    except ValueError:
        return None

def print_user_entry_headers(user_id: int, url: str):
    data = get_data(url)
    if data:
        user_entry = get_user_entry(user_id, data)
        if user_entry:
            for cur_entry in user_entry:
                print(cur_entry[TITLE])
        else:
            print('False')
    else:
        print('False')


if __name__ == "__main__":
    URL = 'https://jsonplaceholder.typicode.com/todos'
    print_user_entry_headers(1, URL)