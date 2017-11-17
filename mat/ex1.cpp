#include <petscmat.h>//包含和矩阵相关的所有文件

char help[] = "An example of basic useages of matrix in petsc";

int main(int argc, char* argv[]) {
	PetscInitialize(&argc, &argv, (char*)0, help);
	PetscInt size, rank;
	MPI_Comm_size(PETSC_COMM_WORLD, &size);
	MPI_Comm_rank(PETSC_COMM_WORLD, &rank);
	PetscErrorCode ierr;

	//创建矩阵步骤一
	Mat A;
	//创建一个矩阵（指针）

	//创建矩阵步骤二
	ierr = MatCreate(PETSC_COMM_WORLD, &A);CHKERRQ(ierr);
	//初始化这个矩阵

	//创建矩阵步骤三
	PetscInt rows = 16, columns = 20;
	//MatCreateAIJ(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, rows, columns, 2, NULL, 14, NULL, &A);
	ierr = MatSetSizes(A, PETSC_DECIDE, PETSC_DECIDE, rows, columns);CHKERRQ(ierr);
	/*设置矩阵的大小
	参数1：矩阵
	参数2：本地行数量
	参数3：本地列数量
	参数4：全局行数量
	参数5：全局列数量
	本地和全局中可以有一个是PETSC_DECIDE*/

	//创建矩阵步骤四
	ierr = MatSetType(A, MATMPIDENSE);CHKERRQ(ierr);
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

	PetscInt globalRow, globalColumn;
	ierr = MatGetSize(A, &globalRow, &globalColumn);CHKERRQ(ierr);
	/*获取矩阵全局大小
	参数1：矩阵
	参数2：用于传回行的指针
	参数3：用于传回列的指针*/

	//创建矩阵步骤五（完）
	ierr = MatMPIDenseSetPreallocation(A, NULL);CHKERRQ(ierr);
	/*为稠密矩阵分配内存空间，调用矩阵相应的函数
	参数2：为NULL则由petsc分配内存，否则自己申请内存*/
	//MatSetOption(A, MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);
	//当程序中出现分配的内存空间不够的时候，使用这条语句可以在运行过程中再次分配内存

	PetscInt localRow, localColumn;
	ierr = MatGetLocalSize(A, &localRow, &localColumn);CHKERRQ(ierr);
	//获取矩阵的对角块在本地的大小，为了为下面的内存分配作参考，所以是对角块的列不是所有列

	PetscInt start, end;
	ierr = MatGetOwnershipRange(A, &start, &end);CHKERRQ(ierr);
	//获取本处理器处理哪row行，start <= row < end

	//输出查看矩阵的相关信息
	if(rank == 0) {
		PetscPrintf(PETSC_COMM_SELF, "The matrix's global size is %d x %d\n", globalRow, globalColumn);
	}
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "The Matrix's local size on processor %d is %d x %d\n", rank, localRow, localColumn);
	PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT);

	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "Processor %d owns from %dth to %dth row\n", rank, start, end-1);
	PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT);

	//获取矩阵在本处理器上的详细信息，需要使用复制即可
	MatInfo info;
	MatGetInfo(A, MAT_LOCAL, &info);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "Processor %d:\n", rank);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		block_size			= %d\n", (int)info.block_size);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		nz_allocated		= %d\n", (int)info.nz_allocated);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		nz_used				= %d\n", (int)info.nz_used);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		nz_unneeded			= %d\n", (int)info.nz_unneeded);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		memory				= %d\n", (int)info.memory);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		assemblies			= %d\n", (int)info.assemblies);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		mallocs				= %d\n", (int)info.mallocs);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		fill_ratio_given	= %d\n", (int)info.fill_ratio_given);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		fill_ratio_needed	= %d\n", (int)info.fill_ratio_needed);
	PetscSynchronizedPrintf(PETSC_COMM_WORLD, "		factor_mallocs		= %d\n", (int)info.factor_mallocs);
	PetscSynchronizedFlush(PETSC_COMM_WORLD, PETSC_STDOUT);

	//if (rank == 0)
	for (PetscInt row = start; row < end; row++) {
		for (PetscInt column = 0; column < globalColumn; column++) {
			PetscScalar tmp = row+(column/100.0);
			ierr = MatSetValues(A, 1, &row, 1, &column, &tmp, INSERT_VALUES);CHKERRQ(ierr);
			/*为矩阵赋值
			参数2：放n个行
			参数3：指向n个行号的指针
			参数4：放m个列
			参数5：指向m个行号的指针*/
		}
	}
	ierr = MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
	ierr = MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
	//处理器通讯赋值，虽然这里通过只为本地的赋值，实际并没有通讯
	//当完成所有工作后使用MAT_FINAL_ASSEMBLY
	//如果不是所有工作完成，后面还要处理，但要暂时将所有操作应用到目标处理器要用MAT_FLUSH_ASSEMBLY
	//据说使用 -info 能查看运行详细情况

	ierr = MatView(A, PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
	//输出矩阵到命令行查看

	PetscViewer fileWriter;
	ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD, "Matrix.dat", FILE_MODE_WRITE, &fileWriter);CHKERRQ(ierr);
	ierr = PetscViewerPushFormat(fileWriter ,PETSC_VIEWER_NATIVE);CHKERRQ(ierr);
	ierr = MatView(A, fileWriter);CHKERRQ(ierr);
	//输出矩阵到文件
	ierr = MatDestroy(&A);CHKERRQ(ierr);
	//释放矩阵内存

    if (rank == 0)
    	PetscPrintf(PETSC_COMM_WORLD, "\n\n\n\nRead matrix from file:\n");
	PetscViewer fileReader;
	ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD, "Matrix.dat", FILE_MODE_READ, &fileReader);CHKERRQ(ierr);
	Mat B;
    ierr = MatCreate(PETSC_COMM_WORLD, &B);CHKERRQ(ierr);
	ierr = MatSetType(B, MATMPIDENSE);CHKERRQ(ierr);
    //指定准备接受的类型
    ierr = MatLoad(B, fileReader);CHKERRQ(ierr);
    ierr = MatView(B, PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
	//输出矩阵到命令行查看

	PetscFinalize();
	return 0;
}
