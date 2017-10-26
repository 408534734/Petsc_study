/*
  创建一个冗余向量，冗余用于存储向量中在其他处理器上元素的信息，
*/

static char help[] = "Demonstrates use of VecCreateGhost().\n\n";

#include <petscvec.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **argv)
{
  PetscMPIInt    rank,size;
  PetscInt       nlocal = 6,nghost = 2,ifrom[2],i,rstart,rend;
  PetscErrorCode ierr;
  PetscBool      flg,flg2;
  PetscScalar    value,*array,*tarray=0;
  Vec            lx,gx,gxs;

//用于限制处理器个数为2用于显示sample
  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRQ(ierr);
  if (size != 2) SETERRQ(PETSC_COMM_SELF,1,"Must run example with two processors\n");

  /*用于设置在每个处理器上的向量要缓存哪些元素     
    ----Processor  1---------  ----Processor 2 --------
     0    1   2   3   4    5    6    7   8   9   10   11
                           0----1
     1-------------------------------------------------0
  */
  if (!rank) {
    ifrom[0] = 11; ifrom[1] = 6;
  } else {
    ifrom[0] = 0;  ifrom[1] = 5;
  }

  //有三种方法可以设置冗余向量
  ierr = PetscOptionsHasName(NULL,NULL,"-allocate",&flg);CHKERRQ(ierr);
  ierr = PetscOptionsHasName(NULL,NULL,"-vecmpisetghost",&flg2);CHKERRQ(ierr);
  if (flg) {
    ierr = PetscMalloc1(nlocal+nghost,&tarray);CHKERRQ(ierr);
    ierr = VecCreateGhostWithArray(PETSC_COMM_WORLD,nlocal,PETSC_DECIDE,nghost,ifrom,tarray,&gxs);CHKERRQ(ierr);
    /*方法一：创建一个有冗余向量，并且调用者来分配向量+冗余的存储空间
      参数1：通信子
      参数2：本处理器上向量大小
      参数3：向量总大小
      参数4：本处理器上冗余大小
      参数5：本处理器上每个冗余存储的冗余对应编号
      参数6：存储空间
      参数7：空向量指针*/
  } else if (flg2) {
    ierr = VecCreate(PETSC_COMM_WORLD,&gxs);CHKERRQ(ierr);
    ierr = VecSetType(gxs,VECMPI);CHKERRQ(ierr);
    ierr = VecSetSizes(gxs,nlocal,PETSC_DECIDE);CHKERRQ(ierr);
    ierr = VecMPISetGhost(gxs,nghost,ifrom);CHKERRQ(ierr);
    /*方法二：在向量创建好后由系统来创建冗余
      参数1：向量
      参数2：本处理器上冗余大小
      参数3：本处理器上每个冗余存储的冗余对应编号*/
  } else {
    ierr = VecCreateGhost(PETSC_COMM_WORLD,nlocal,PETSC_DECIDE,nghost,ifrom,&gxs);CHKERRQ(ierr);
    /*方法三：创建一个冗余向量，并且系统者来分配向量+冗余的存储空间【建议使用这个方法】
      参数1：通信子
      参数2：本处理器上向量大小
      参数3：向量总大小
      参数4：本处理器上冗余大小
      参数5：本处理器上每个冗余存储的冗余对应编号
      参数6：空向量指针*/
  }

  //创建一个相同类型的向量，但是向量里面的值并没有被复制，如果要复制需要使用VecCopy()
  ierr = VecDuplicate(gxs,&gx);CHKERRQ(ierr);
  ierr = VecDestroy(&gxs);CHKERRQ(ierr);

  //取出向量的本身部分和冗余部分，这里把这个向量叫做扩展向量，原来的向量叫做冗余向量
  //注意这里是相当于创建了一个新的向量【指针】，扩展向量在每个处理器上的大小相当于冗余向量在每个处理上的大小再加上冗余
  //两个向量使用相同的储存空间，所以这个扩展扩展向量【会自动更新】
  ierr = VecGhostGetLocalForm(gx,&lx);CHKERRQ(ierr);

  //更改冗余向量的值
  ierr = VecGetOwnershipRange(gx,&rstart,&rend);CHKERRQ(ierr);
  for (i=rstart; i<rend; i++) {
    value = (PetscScalar) i;
    ierr  = VecSetValues(gx,1,&i,&value,INSERT_VALUES);CHKERRQ(ierr);
  }
  ierr = VecAssemblyBegin(gx);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(gx);CHKERRQ(ierr);

  /*同步冗余部分
    参数1：冗余向量
    参数2：更改模式， ADD_VALUES or INSERT_VALUES 累加模式 或 替代模式
    参数3：同步模式， SCATTER_FORWARD or SCATTER_REVERSE 从别的处理器同步到自己的冗余 或 将自己的冗余同步到别的处理器*/
  ierr = VecGhostUpdateBegin(gx,INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
  ierr = VecGhostUpdateEnd(gx,INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);

  //取出扩展向量里面的值、访问、放回
  ierr = VecGetArray(lx,&array);CHKERRQ(ierr);
  for (i=0; i<nlocal+nghost; i++) {
    //缓冲输出，用于按处理器的编号顺序来输出，下面的语句是先把每个处理器的编号放到缓冲区
    ierr = PetscSynchronizedPrintf(PETSC_COMM_WORLD,"%D %g\n",i,(double)PetscRealPart(array[i]));CHKERRQ(ierr);
  }
  ierr = VecRestoreArray(lx,&array);CHKERRQ(ierr);
  //在这里将缓存输出
  ierr = PetscSynchronizedFlush(PETSC_COMM_WORLD,PETSC_STDOUT);CHKERRQ(ierr);
  //将扩展向量指针“归还”
  ierr = VecGhostRestoreLocalForm(gx,&lx);CHKERRQ(ierr);

  //将冗余向量释放，手动分配内存还要自行释放
  ierr = VecDestroy(&gx);CHKERRQ(ierr);
  if (flg) {ierr = PetscFree(tarray);CHKERRQ(ierr);}
  ierr = PetscFinalize();
  return 0;
}



