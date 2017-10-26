
static char help[] = "Demonstrates using a local ordering to set values into a parallel vector.\n\n";

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  PetscErrorCode ierr;
  PetscMPIInt    rank;
  PetscInt       i,N,ng,*gindices,rstart,rend,M;
  PetscScalar    one = 1.0;
  Vec            x;

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);

  ierr = VecCreate(PETSC_COMM_WORLD,&x);CHKERRQ(ierr);
  ierr = VecSetSizes(x,rank+1,PETSC_DECIDE);CHKERRQ(ierr);
  ierr = VecSetFromOptions(x);CHKERRQ(ierr);
  ierr = VecGetSize(x,&N);CHKERRQ(ierr);
  ierr = VecSet(x,one);CHKERRQ(ierr);

  ierr = VecGetSize(x,&M);CHKERRQ(ierr);
  ierr = VecGetOwnershipRange(x,&rstart,&rend);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_SELF, "Processor %d : from %d to %d\n",rank, rstart, rend);CHKERRQ(ierr);
  ng   = rend - rstart + 2;
  //这里的gindices用于建立映射关系
  ierr = PetscMalloc1(ng,&gindices);CHKERRQ(ierr);
  gindices[0] = rstart - 1;
  for (i=0; i<ng-1; i++) gindices[i+1] = gindices[i] + 1;
  if (gindices[0]    == -1) gindices[0]    = M - 1;
  if (gindices[ng-1] == M)  gindices[ng-1] = 0;

  for (i = 0; i < ng; i++)
      ierr = PetscPrintf(PETSC_COMM_SELF, "Processor %d[%d] : %d\n",rank, i, gindices[i]);CHKERRQ(ierr);
  {//将映射关系与我们要重建映射关系的x放在建立联系
    ISLocalToGlobalMapping ltog;
    ierr = ISLocalToGlobalMappingCreate(PETSC_COMM_SELF,1,ng,gindices,PETSC_COPY_VALUES,&ltog);CHKERRQ(ierr);
    ierr = VecSetLocalToGlobalMapping(x,ltog);CHKERRQ(ierr);
    ierr = ISLocalToGlobalMappingDestroy(&ltog);CHKERRQ(ierr);
  }
  ierr = PetscFree(gindices);CHKERRQ(ierr);

  for (i=0; i<ng; i++) {
      //注意这里再使用i访问的x的元素不是x中的第i个元素了
      //而是刚刚通过gindices设置的在本处理上通过gindices[i]要访问到的元素编号
    ierr = VecSetValuesLocal(x,1,&i,&one,ADD_VALUES);CHKERRQ(ierr);
  }

  ierr = VecAssemblyBegin(x);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(x);CHKERRQ(ierr);

  //通过输出检查设置是否正确
  ierr = VecView(x,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);
  ierr = VecDestroy(&x);CHKERRQ(ierr);

  ierr = PetscFinalize();
  return 0;
}
