import time
from lxml import etree
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as ec
from selenium.common.exceptions import WebDriverException


def createXml(xmlfile, index, title_t, link_t, date_t, isbn_t, authors_t, text_t):
    doc = etree.Element('doc')

    title = etree.SubElement(doc, 'title')
    title.text = title_t
    title.attrib['verify'] = "true"
    title.attrib['type'] = "str"
    title.attrib['auto'] = "true"

    link = etree.SubElement(doc, 'link')
    link.text = link_t
    link.attrib['verify'] = "true"
    link.attrib['type'] = "str"
    link.attrib['auto'] = "true"

    date = etree.SubElement(doc, 'date')
    date.text = date_t
    date.attrib['verify'] = "true"
    date.attrib['type'] = "str"
    date.attrib['auto'] = "true"

    isbn = etree.SubElement(doc, 'ISBN')
    isbn.text = isbn_t
    isbn.attrib['verify'] = "true"
    isbn.attrib['type'] = "str"
    isbn.attrib['auto'] = "true"

    authors = etree.SubElement(doc, 'authors')
    authors.attrib['verify'] = "true"
    authors.attrib['type'] = "str"
    authors.attrib['auto'] = "true"
    for author_t in authors_t:
        author_item = etree.SubElement(authors, 'item')
        author_item.text = author_t

    text = etree.SubElement(doc, 'text')
    text.text = etree.CDATA(text_t)
    text.attrib['verify'] = "true"
    text.attrib['type'] = "str"
    text.attrib['auto'] = "true"

    with open('ParcStore/' + xmlfile + ('_%d.xml' % index), "w") as fh:
        fh.write(etree.tostring(doc, pretty_print=True).decode("utf-8"))
        fh.close()


def getAuthors():
    return_authors = []
    search_authors = driver.find_elements_by_xpath("//*[@class ='rlist--inline loa comma']"
                                                   "/*/*/descendant-or-self::span")
    for search_author in search_authors:
        return_authors.append(search_author.text)
    return return_authors


def getText():
    try:
        return driver.find_element_by_xpath("//*[@class='content aboutBook']").text.rstrip('Show less')
    except WebDriverException:
        return 'not specified'


def linkWhenLocated(link_xpath, param):
    button = wait.until(ec.presence_of_element_located((By.XPATH, link_xpath % param)))
    return button.get_attribute("href")


def clickShowAll():
    try:
        showall = wait.until(ec.element_to_be_clickable((By.XPATH, "//*[contains(@aria-expanded, 'false')]")))
        showall.click()
    except WebDriverException:
        print('showall wasnt expanded')


def byAlphabet(choosen):
    link = "//*[text()='%s' and contains(@class, 'alphanumeric-title')]"
    if choosen.isdigit():
        driver.get(linkWhenLocated(link, '0-9'))
        return True
    elif choosen.isalpha():
        driver.get(linkWhenLocated(link, choosen.upper()))
        return True
    return False


def getInputCycled(pre_str, input_type_str):
    try:
        request_var = input(pre_str)
        if input_type_str == 'int':
            int(request_var)
        elif input_type_str == 'str':
            str(request_var)
        elif input_type_str == 'chr':
            if (len(str(request_var)) > 1) or not(request_var.isdigit() or request_var.isalpha()):
                raise ValueError
        else:
            print('wrong input class, use ' + input_type_str + ' instead')
            request_var = getInputCycled(pre_str, input_type_str)
    except ValueError:
        print('wrong input class, use ' + input_type_str + ' instead')
        request_var = getInputCycled(pre_str, input_type_str)
    return request_var


if __name__ == "__main__":

    requestedAmmount = int(getInputCycled('Requested ammount to parce: ', 'int'))     # Кол-во для парса
    xml_file_name = getInputCycled('Set Xml file сore name: ', 'str')                 #имя xml
    alphabet = getInputCycled('Filter by alphabet (any digit for only numerical): ', 'chr')  # Фильтр алфав
    driver = webdriver.Firefox()
    wait = WebDriverWait(driver, 10)
    driver.get('https://onlinelibrary.wiley.com/action/showPublications?startPage=&PubType=book')
    byAlphabet(alphabet)
    i = 1       # Обший индекс от 1
    k = 1       # Индекс объекта на странице от 1
    page = 1    # Cтраницы

    ammount_on_page = len(driver.find_elements_by_xpath("//*[contains(@class, 'search-result__body')]/*/*/*/a"))
    while True:
        if k <= ammount_on_page:
            if i > requestedAmmount:
                break
            time.sleep(0.5)
            booklink_xpath = "//*[contains(@class, 'search-result__body')]/descendant::a[@class = 'visitable'][%s]"
            booklink_text = linkWhenLocated(booklink_xpath, k)
            driver.get(booklink_text)
            wait = WebDriverWait(driver, 1)
            clickShowAll()
            wait = WebDriverWait(driver, 10)
            createXml(xml_file_name,
                      i,
                      wait.until(ec.presence_of_element_located((By.XPATH, "//*[@class=' col-sm-10']/h1"))).text,       #   Название
                      booklink_text,                                                                                    #   Ссылка
                      driver.find_element_by_xpath("//*[@class = 'info-block'][1]/span[2]").text,                       #   Дата публикации
                      driver.find_element_by_xpath("//*[@class = 'info-block'][2]/span[2]").text[0:-2],                 #   Печатный ISBM
                      getAuthors(),                                                                                     #   Список авторов
                      getText()                                                                                         #   Текст
                      )
            time.sleep(0.5)
            driver.execute_script("window.history.go(-1)")
            k += 1
            
            if requestedAmmount >= 0:
                i += 1
            else:
                break
            
        else:
            nextpage_xpath = "//*[@class= 'rlist--inline pagination__list']/descendant::a[@title = '%s']"
            driver.get(linkWhenLocated(nextpage_xpath, page + 1))
            page += 1
            k = 1

    #print('happy end')
