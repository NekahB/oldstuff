#Отсортировать в матрице столбцы по убыванию значений элементов в первой строке.
#В первой строке вводится размер матрицы, далее матрица задается построчно.

def formmatrixfromstrings(colums_number: int, string_matrix: list) -> list:

    result = []

    for index in range(colums_number):
        column = [int(matrix_row[index]) for matrix_row in string_matrix]
        result.append(column)
    return result


st = str(input()).split()                                    #строки столбцы
raws = int(st[0])
colums = int(st[1])

matrix = []
for i in range(raws):                                       #
    matrix.append(input().split())                     # построчный ввод
#matrix = [[1,2,3,5], [1,2,3,4], [3,2,3,1], [3,2,1,3]]
column_matrix = formmatrixfromstrings(colums, matrix)
sorted_matrix = sorted(column_matrix, key=lambda x: x[0], reverse=True)
resultat = formmatrixfromstrings(raws, sorted_matrix)
for elem in resultat:
    print(' '.join(map(str, elem)))
