#include <petscvec.h>

static char help[] = "Builds a parallel vector with 1 component on the first processor, 2 on the second, etc.\n\
  Then each processor adds one to all elements except the last rank.\n\n";

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
    PetscErrorCode ierr;//错误码,每一次调用都会返回一个错误码
    PetscMPIInt    rank;//返回当前处理器编号，注意这里的数据类型
    PetscInt       i,N;//就是int，但是注意这里的数据类型，默认为32位的，如果要改变，需要在编译？时指定
    PetscScalar    one = 1.0;//就是double
    Vec            x;//petsc中的向量，底层实现为一个指针

    PetscInitialize(&argc,&argv,(char*)0,help);
    //初始化petsc，注意从这里开始就已经开始并行了
    //参数使用方法未知
    ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
    //获得当前处理器编号
    //参数2：注意这里的数据类型是 PetscMPIInt

    ierr = VecCreate(PETSC_COMM_WORLD,&x);CHKERRQ(ierr);
    //初始化向量，
    //参数1：在哪些处理器上创建该向量
    //参数2：向量，要取地址，因为在底层它本身就是指针，所以后面就不用取地址了
    //注意：这里创建向量还有别的函数！！！
    ierr = VecSetSizes(x,rank+1,PETSC_DECIDE);CHKERRQ(ierr);
    //将向量设置大小
    //参数1：向量
    //参数2：在本处理器上元素的个数，当参数3指定时，这里可以为由petsc指定→PETSC_DECIDE
    //参数3：该向量的总个数，当参数2指定时，这里可以由petsc指定→PETSC_DECIDE
    ierr = VecSetFromOptions(x);CHKERRQ(ierr);
    //将向量进行一些设置，属于高级用法，暂时不用管
    ierr = VecGetSize(x,&N);CHKERRQ(ierr);
    //获取向量总大小
    ierr = VecSet(x,one);CHKERRQ(ierr);
    //将整个向量的每个元素都设置为参数2指定的数

    for (i=0; i<N-rank; i++) {
        ierr = VecSetValues(x,1,&i,&one,ADD_VALUES);CHKERRQ(ierr);
        //将第i个元素设置/加上参数4，注意这里是对向量在所有处理器上的部分都进行操作
        //参数1：要处理的向量
        //参数2：要加上的元素的数量
        //参数3：加在/更改向量的第几个元素
        //参数4：要加的若干个元素的首地址/要更改为的元素的地址
        //参数5：加法模式：ADD_VALUES / 更改模式：INSERT_VALUES
    }

    ierr = VecAssemblyBegin(x);CHKERRQ(ierr);
    ierr = VecAssemblyEnd(x);CHKERRQ(ierr);
    //由于VecSetValues通常需要长时间的处理器间通讯时间，故中间有很多的空闲时间，所以在等待通讯的过程中可以做一些事情
    //要做的事情在Begin和End中间，并且这两条语句是强制要求有的

    ierr = VecView(x,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
    //查看向量
    ierr = VecDestroy(&x);CHKERRQ(ierr);
    //释放不用的矩阵的内存
    ierr = PetscFinalize();
    //结束petsc程序，如果并行initialize是由PetscInitialize开始的
    //这里就结束并行了自动执行Finalize，否则需要手动调用
    return 0;
}
