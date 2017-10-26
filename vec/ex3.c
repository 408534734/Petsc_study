
static char help[] = "Parallel vector layout.\n\n";

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
    PetscErrorCode ierr;
    PetscMPIInt    rank;
    PetscInt       i,istart,iend,n = 6,nlocal;
    PetscScalar    v,*array;
    Vec            x;
    //PetscViewer    viewer;
    //无法使用因为需要WINDOWS-X
    PetscInitialize(&argc,&argv,(char*)0,help);
    ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
    PetscBool flag;
    ierr = PetscOptionsGetInt(NULL,NULL,"-n",&n,NULL);CHKERRQ(ierr);
    //获取参数中的“-n”的参数, 使用方法： ./ex3 -n 4
    ierr = VecCreate(PETSC_COMM_WORLD,&x);CHKERRQ(ierr);
    ierr = VecSetSizes(x,PETSC_DECIDE,n);CHKERRQ(ierr);
    ierr = VecSetFromOptions(x);CHKERRQ(ierr);
    ierr = VecGetOwnershipRange(x,&istart,&iend);CHKERRQ(ierr);
    //获取本地处理器上向量编号的范围

    printf("%d\n", n);
    for (i=0; i<n; i++) {
          v    = (PetscReal)(rank*i);
          ierr = VecSetValues(x,1,&i,&v,ADD_VALUES);CHKERRQ(ierr);
    }
    ierr = VecAssemblyBegin(x);CHKERRQ(ierr);
    ierr = VecAssemblyEnd(x);CHKERRQ(ierr);

    ierr = VecView(x,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

    //ierr = PetscViewerDrawOpen(PETSC_COMM_WORLD,NULL,NULL,0,0,300,300,&viewer);CHKERRQ(ierr);
    //ierr = PetscObjectSetName((PetscObject)viewer,"Line graph Plot");CHKERRQ(ierr);
    //ierr = PetscViewerPushFormat(viewer,PETSC_VIEWER_DRAW_LG);CHKERRQ(ierr);
    //无法使用因为需要WINDOWS-X

    ierr = VecGetLocalSize(x,&nlocal);CHKERRQ(ierr);
    //获取本地处理器上向量元素的数量
    ierr = VecGetArray(x,&array);CHKERRQ(ierr);
    //把指针指向本地处理器上向量元素的首地址，注意这里指针是借出来的，是要还的
    for (i=0; i<nlocal; i++) array[i] = rank + 1;
    ierr = VecRestoreArray(x,&array);CHKERRQ(ierr);
    //在这里还！
    ierr = VecView(x,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

    //ierr = VecView(x,viewer);CHKERRQ(ierr);
    //ierr = PetscViewerPopFormat(viewer);CHKERRQ(ierr);
    //ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);
    //无法使用因为需要WINDOWS-X

    ierr = VecDestroy(&x);CHKERRQ(ierr);
    ierr = PetscFinalize();
    return 0;
}
