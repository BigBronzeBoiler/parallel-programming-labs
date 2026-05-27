import numpy
import subprocess

def main():
    for i in (200,400,800,1200,1600,2000):
        try:
            subprocess.run(["python","./tyler_the_creator.py", "-s "+str(i)], check=True)
        except Exception as e:
            print(f"Не получилось запустить код на Python: {e}, увы(")
            return

        matrix_a = numpy.loadtxt("a.txt")
        matrix_b = numpy.loadtxt("b.txt")
        
        try:
            subprocess.run(["mpiexec", "-n", "8", "./main.exe", str(i)], check=True)
        except Exception as e:
            print(f"Не получилось запустить код на C++: {e}, увы(")
            return
    
    result_matrix = numpy.loadtxt("result.txt")
    true_result = matrix_a @ matrix_b
    if result_matrix.all() != true_result.all():
        print("Матрицы разные, увы(")
    else:
        print("Это правда, я проверил")


if __name__ == "__main__":
    main()