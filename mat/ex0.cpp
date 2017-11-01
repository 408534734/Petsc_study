#include <petscmat.h>//包含和矩阵相关的所有文件

char help[] = "An example of basic useages of matrix in petsc."

int main(int argc, char* argv[]) {
	PetscInitialize(&argc, &args, (char*)0, help);
	Mat A;
	//创建一个矩阵（指针）
	MatCreate(PETSC_COMM_WORLD, &A);
	//初始化这个矩阵
	PetscInt rows = 16, cloumns = 16;
	MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, rows, columns);
	/*设置矩阵的大小
	参数1：矩阵
	参数2：本地行数量
	参数3：本地列数量
	参数4：全局行数量
	参数5：全局列数量
	本地和全局中可以有一个是PETSC_DECIDE*/
	MatSetType(A, MATMPIAIJ);
	/*设置矩阵类型为第二个参数所指定的类型
	常见的类型有：
	MATSEQAIJ：串行稀疏矩阵
	MATMPIAIJ：并行稀疏矩阵
	MATAIJ：稀疏矩阵，默认为串行
	MATSEQDENSE：串行稠密矩阵
	MATMPIDENSE：并行稠密矩阵
	MATDENSE：稠密矩阵，默认为串行
	*/
	//MatSetFromOptions(A);
	/*矩阵的类型s也可以在运行时用参数指定
	详细见http://www.mcs.anl.gov/petsc/petsc-3.7/docs/manualpages/Mat/MatSetType.html#MatSetType*/
	return 0;
}