import requests

from typing import *
import xml.etree.cElementTree as etree

#
# Print hightest currency
#

URL = 'http://www.cbr.ru/scripts/XML_daily.asp'
VALUE = 'Value'
VALUTE = 'Valute'
CHARCODE = 'CharCode'
NMINAL = 'Nominal'

def get_highest_currency(data: str) -> Optional[etree.Element]:

    # загрузка в xml
    try:
        xml_data = etree.fromstring(data)
        result = xml_data[0]
        for cur_elem in xml_data.iter(tag=VALUTE):
            cur_value_element = cur_elem.find(VALUE)
            result_value_element = result.find(VALUE)

            if cur_value_element.text > result_value_element.text:
                result = cur_elem
        return result
    except BaseException:
        return None

def get_data(url: str) -> Optional[str]:

    request = requests.get(url)
    try:
        data = request.text
        return data
    except ValueError:
        return None

def print_highest_currency(url: str):

    data = get_data(url)
    if data:
        highest_currency = get_highest_currency(data)
        if highest_currency:
            result = highest_currency.find(CHARCODE)
            print(result.text)
        else:
            print('False')

if __name__ == '__main__':
    print_highest_currency(URL)