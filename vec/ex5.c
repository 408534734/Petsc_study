
static char help[] = "Tests binary I/O of vectors and illustrates the use of user-defined event logging.\n\n";

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
    PetscErrorCode ierr;
    PetscMPIInt    rank,size;
    PetscInt       i,m = 10,low,high,ldim,iglobal;
    PetscScalar    v;
    Vec            u;
    PetscViewer    viewer;
    //这里的viewer是用来输出到文件的
#if defined(PETSC_USE_LOG)
    PetscLogEvent  VECTOR_GENERATE,VECTOR_READ;
    //定义两个程序运行事件
#endif
    PetscInitialize(&argc,&args,(char*)0,help);
    ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
    ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
    ierr = PetscOptionsGetInt(NULL,NULL,"-m",&m,NULL);CHKERRQ(ierr);
     /* PART 1:  Generate vector, then write it in binary format */
    ierr = PetscLogEventRegister("Generate Vector",VEC_CLASSID,&VECTOR_GENERATE);CHKERRQ(ierr);
    //初始化事件，定义事件名
    ierr = PetscLogEventBegin(VECTOR_GENERATE,0,0,0,0);CHKERRQ(ierr);
    //记录事件的开始，这里后四个参数用法未知？？？

    /* Generate vector */
    ierr = VecCreate(PETSC_COMM_WORLD,&u);CHKERRQ(ierr);
    ierr = VecSetSizes(u,PETSC_DECIDE,m);CHKERRQ(ierr);
    ierr = VecSetFromOptions(u);CHKERRQ(ierr);
    ierr = VecGetOwnershipRange(u,&low,&high);CHKERRQ(ierr);
    ierr = VecGetLocalSize(u,&ldim);CHKERRQ(ierr);
    for (i=0; i<ldim; i++) {
        iglobal = i + low;
        v       = (PetscScalar)(i + 100*rank);
        ierr    = VecSetValues(u,1,&iglobal,&v,INSERT_VALUES);CHKERRQ(ierr);
    }
    ierr = VecAssemblyBegin(u);CHKERRQ(ierr);
    ierr = VecAssemblyEnd(u);CHKERRQ(ierr);
    ierr = VecView(u,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
    //将向量u输出到标准输出
    ierr = PetscPrintf(PETSC_COMM_WORLD,"writing vector in binary to vector.dat ...\n");CHKERRQ(ierr);
    //输出字符串到标准输出
    ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,"vector.dat",FILE_MODE_WRITE,&viewer);CHKERRQ(ierr);
    //打开输出文件
    ierr = VecView(u,viewer);CHKERRQ(ierr);
    //将向量u输出到viewer指向的文件
    ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);
    //关闭viewer打开的文件
    ierr = VecDestroy(&u);CHKERRQ(ierr);
    ierr = PetscOptionsSetValue(NULL,"-viewer_binary_mpiio","");CHKERRQ(ierr);
    ierr = PetscLogEventEnd(VECTOR_GENERATE,0,0,0,0);CHKERRQ(ierr);
    //记录事件结束，这里后四个参数用法未知？？？

     /* PART 2:  Read in vector in binary format */
     /* Read new vector in binary format */
    ierr = PetscLogEventRegister("Read Vector",VEC_CLASSID,&VECTOR_READ);CHKERRQ(ierr);
    ierr = PetscLogEventBegin(VECTOR_READ,0,0,0,0);CHKERRQ(ierr);
    ierr = PetscPrintf(PETSC_COMM_WORLD,"reading vector in binary from vector.dat ...\n");CHKERRQ(ierr);
    ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,"vector.dat",FILE_MODE_READ,&viewer);CHKERRQ(ierr);
    ierr = VecCreate(PETSC_COMM_WORLD,&u);CHKERRQ(ierr);
    //这里只需要将用于接收的u初始化即可
    ierr = VecLoad(u,viewer);CHKERRQ(ierr);
    //将文件中的向量载入到u
    ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);
    ierr = PetscLogEventEnd(VECTOR_READ,0,0,0,0);CHKERRQ(ierr);
    ierr = VecView(u,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
     /* Free data structures */
    ierr = VecDestroy(&u);CHKERRQ(ierr);
    ierr = PetscFinalize();
    return 0;
}
