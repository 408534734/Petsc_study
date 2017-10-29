
static char help[] = "Demonstrates VecStrideScatter() and VecStrideGather().\n\n";

/*T
   Concepts: vectors^sub-vectors;
   Processors: n
T*/

/*
  Include "petscvec.h" so that we can use vectors.  Note that this file
  automatically includes:
     petscsys.h       - base PETSc routines   petscis.h     - index sets
     petscviewer.h - viewers
*/

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  PetscErrorCode ierr;
  Vec            v,s;               /* vectors */
  PetscInt       n   = 20;
  PetscScalar    one = 1.0;

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = PetscOptionsGetInt(NULL,NULL,"-n",&n,NULL);CHKERRQ(ierr);

  /*
      Create multi-component vector with 2 components
  */
  ierr = VecCreate(PETSC_COMM_WORLD,&v);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)v, "v");CHKERRQ(ierr);
  ierr = VecSetSizes(v,PETSC_DECIDE,n);CHKERRQ(ierr);
  ierr = VecSetBlockSize(v,2);CHKERRQ(ierr);
  //将向量分块
  //参数1：将要分块的矩阵
  //参数2：分成多少块
  ierr = VecSetFromOptions(v);CHKERRQ(ierr);

  /*
      Create single-component vector
  */
  ierr = VecCreate(PETSC_COMM_WORLD,&s);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject)s, "s");CHKERRQ(ierr);
  ierr = VecSetSizes(s,PETSC_DECIDE,n/2);CHKERRQ(ierr);
  ierr = VecSetFromOptions(s);CHKERRQ(ierr);

  /*
     Set the vectors to entries to a constant value.
  */
  ierr = VecSet(v,one);CHKERRQ(ierr);

  /*
     Get the first component from the multi-component vector to the single vector
  */
  ierr = VecStrideGather(v,0,s,INSERT_VALUES);CHKERRQ(ierr);
  //将v的每一块里的第0个元素放入s这个没有分块的向量中

  ierr = VecView(s,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  /*
     Put the values back into the second component
  */
  ierr = VecStrideScatter(s,1,v,ADD_VALUES);CHKERRQ(ierr);
  //将没有分块的s的每个元素放到v的每一块的第一个元素中

  ierr = VecView(v,PETSC_VIEWER_STDOUT_WORLD);CHKERRQ(ierr);

  /*
     Free work space.  All PETSc objects should be destroyed when they
     are no longer needed.
  */
  ierr = VecDestroy(&v);CHKERRQ(ierr);
  ierr = VecDestroy(&s);CHKERRQ(ierr);
  ierr = PetscFinalize();
  return 0;
}

