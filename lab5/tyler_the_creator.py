import argparse
import random


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--size", type=int, required=True, help="Размер матрицы")
    args = parser.parse_args()
    return args.size


def create_matrix(size: int) -> list[list]:
    matrix = []
    for i in range(size):
        row = []
        for j in range(size):
            row.append(random.randint(-99,99))
        matrix.append(row)
    return matrix


def write_matrix(matrix: list[list], filename: str) -> None:
    with open(filename, "w", encoding="utf-8") as file:
        n = len(matrix)
        for i in range(n):
            for j in range(n-1):
                file.write(str(matrix[i][j])+" ")
            file.write(str(matrix[i][n-1]))
            file.write('\n')
    return


def main():
    size = parse_args()
    print(size)
    a = create_matrix(size)
    b = create_matrix(size)
    write_matrix(a,"A.txt")
    write_matrix(b,"B.txt")


if __name__ == "__main__":
    main()